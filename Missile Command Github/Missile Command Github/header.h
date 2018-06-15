/*
MISSILE COMMAND SUMMATIVE BY KEVIN YANG & EDWARD LI
FINAL VERSION
*/

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define NUM_COLORS 7
#define COLORS_PER_THEME 3  //3 colours generated per level; one for regular enemy missiles, one for player anti-ballistic missiles, one for enemy cruise missiles

const float FPS = 60;
const int SCREEN_W = 900;
const int SCREEN_H = 900;
const int SPLIT_COUNT = 4;	//each enemy missile can split into four
const int SIZE = 1.5;  //half the size of a 3x3 square
const int ABM_COUNT = 30;	//player starts with 30 anti-ballistic missiles (abm) every level 

enum KEYS {
	ENTER, SPACE, ESCAPE
};

enum COLORS {
	BLUE, GREEN, RED, YELLOW, PINK, ORANGE, MAGENTA
};

enum RGB {
	R, G, B
};

enum DIRECTION {
	LEFT, RIGHT
};

enum FONTS {
	TEXT, TITLE, HEADING, BOLD
};

typedef struct vector {
	float x;
	float y;
} Vector;

//crosshair 
typedef struct crosshairData {
	float height;
	float width;
	Vector pos;
	Vector target;
} Crosshair;


//player's anti-ballistic missiles
typedef struct abmData {
	Vector dest; //destination 
	Vector launch; //launch coordinate 
	Vector inc;	//increment for trajectory
	Vector pos;	//current position 
	float dx;	//delta x between launchpoint and destination
	float dy;	//delta y between launchpoint and destination
	float step;	//same as inc
	int speed;	
	bool launched;	//abm is en route if true 
	bool arrived;	//used up
	int num_increment;	//number of increments in x or y required to reach destination
} Abm;


//abm explodes after reaching target
typedef struct explosionData {
	bool ongoing;
	int radius;	
	bool increaseRadius;	//explosion expands and contracts
	bool expandedRadius;	//explosion radius increases when it destroys an enemy

	Vector center;	//center of explosion circle

	//bounds
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Explosion;


//enemy missiles; same as Abm 
typedef struct enemyData {
	Vector dest;
	Vector launch;
	float dx;
	float dy;
	Vector inc;
	Vector pos;
	float step;
	bool launched;
	//bounds
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Enemy;


//enemy flying saucer
typedef struct ufoData {
	bool spawned;
	int origin; //x-coordinate of spawn location
	int colour; //red or blue 
	Enemy missile[2];	//each ufo can fire two cruise missiles that target bases
	Vector pos;	//current position
	//bounds
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Ufo;

//scm = smart cruise missile
typedef struct smartCruiseData {
	bool spawned;
	bool moveLeft;
	int timerCount;
	Vector origin;
	Vector target;
	Vector pos;
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Scm; 

//player's bases to be defended
typedef struct baseData {
	bool destroyed;	
	Vector pos;	//location
	//bounds
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Base;


//variables that change each level 
typedef struct levelData {
	int round; //level 1, 2, etc.
	int lives;	//number of bases remaining
	int score;	
	int abmLeft;	//number of anti-ballistic missiles remaining
	int highScores[5];	//5 highscores displayed on title screen
	int highScoreCount;	//5
	bool newHighScore;	//true if player's current score is higher than the lowest highscore
	bool initialSort;	//true at start of game to rank highscores in textfile  
	bool speedUp;		//all enemies speed up if player ran out of ABMs to finish level quicker
	int multiplier;		//score multiplier; see user guide

	//spawn regular enemy missile if a random number is generated in this range
	int spawnRangeMin;	//decreased in successive levels to increase probability of enemy spawning
	int spawnRangeMax;	//increased in successive levels to increase probability of enemy spawning

	//regular enemy missile splits up if a random number is generated in this range 
	int splitRangeMin;
	int splitRangeMax;

	//range in which random number must fall for flying saucer to spawn 
	int ufoSpawnRangeMin;
	int ufoSpawnRangeMax;

	//max # of enemies on screen at once 
	int maxEnemyOnScreen;
	int maxUfoOnScreen;
	int maxScmOnScreen;

	int enemyLastSpawned;	//time since last enemy missile spawned; can only spawn 0.15 sec after each other
	int spawnLimit;		//max # of enemy missiles that can spawn each level
	int num_spawned;	//# of enemy missiles spawned for current level
	float enemySpeed;	//speed of enemy missiles
	int splitAngle;		//how far an enemy missile can split from the previous
	int spawnRate;		//range for generating random number
	int splitRate;		//range for generating random number

	//ufo
	int ufoSpawnLimit;
	int ufoSpawnRate;	//range for generating random number
	int ufoNumSpawned;	//# of ufos spawned for current level
	int ufoSpawnSide[2];	//ufo can spawn on left or right of screen
	int ufoMissileSpawnRate;
	bool spawnUfo;
	float ufoSpeed;
	int ufoLastSpawned; //time since last ufo spawned
	Vector ufoSize;

	//smart cruise missile
	bool spawnScm;	//scm spawned from level 2 onwards
	int scmSpawnLimit;	//max # of scms that can be spawned
	int scmNumSpawned;	//# of scms spawned in current level
	float scmSpeed;
	int scmSpawnRate;	//range for generating random number
	int scmLastSpawned;	//time since last scm spawned; prevent spawning mulitple at same time 
	Vector scmSize;		//size of scm image

	//base
	Vector baseSize;	//size of base image
	float base_x[6];	//x-coordinate of bases

} Level;

//sound effects
typedef struct audio {
	ALLEGRO_SAMPLE * siren;
	ALLEGRO_SAMPLE * missileLaunch;
	ALLEGRO_SAMPLE * explosion[6];
} Audio;


//prototypes
void initAllegro(ALLEGRO_DISPLAY **display, ALLEGRO_TIMER **timer, ALLEGRO_BITMAP **imageCrosshair, ALLEGRO_EVENT_QUEUE **event_queue, ALLEGRO_FONT ** font,
	ALLEGRO_BITMAP ** background, ALLEGRO_BITMAP ** imageUfo, Level * level, ALLEGRO_BITMAP ** imageScm, ALLEGRO_BITMAP ** imageMissile, ALLEGRO_BITMAP ** ground,
	ALLEGRO_BITMAP ** imageBase, Audio * audio);

void initCrosshair(Crosshair * crosshair, ALLEGRO_BITMAP * imageCrosshair);
void initAbm(Abm * abm, Explosion * explosion);

void gameLoop(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_EVENT_QUEUE *event_queue,
	Crosshair crosshair, Abm * abm, Enemy ** enemy, ALLEGRO_FONT ** font, Base * base, Explosion * explosion, int * theme, int colorMap[][3],
	Level * level, ALLEGRO_BITMAP * background, ALLEGRO_BITMAP ** imageUfo, Ufo * ufo, ALLEGRO_BITMAP ** imageScm, Scm * scm,
	ALLEGRO_BITMAP * imageMissile, ALLEGRO_BITMAP * ground, ALLEGRO_BITMAP * imageBase,	FILE * fptr, Audio * audio);

void drawCrosshair(ALLEGRO_BITMAP *imageCrosshair, Crosshair * crosshair);

void fire(Abm * abm, Crosshair crosshair, Level * level, Audio * audio);
void calcAbmInc(Abm * abm);
void updateAbm(Abm * abm);
void drawAbm(Abm * abm, int abmColour, int colorMap[][3]);
void abmArrival(Abm * abm, Explosion * explosion);  //check if abm arrived
void drawExplosion(Abm * abm, Explosion * explosion, int colorMap[][3], Level * level);

void initEnemy(Enemy ** enemy, Level * level, Ufo * ufo, Scm * scm);
void spawnEnemy(Enemy ** enemy, Level * level, Ufo * ufo, Scm * scm, Base * base, ALLEGRO_TIMER * timer);
void calcEnemyInc(Enemy * enemy);
void drawEnemy(Enemy ** enemy, int * theme, int colorMap[][3], Level * level, Ufo * ufo, ALLEGRO_BITMAP ** imageUfo, ALLEGRO_BITMAP ** imageScm, Scm * scm);
void updateEnemy(Enemy ** enemy, Level * level);
void enemyArrival(Enemy ** enemy, Level * level, Ufo * ufo, Scm * scm);

void collision(Abm * abm, Enemy ** enemy, Explosion * explosion, Level * level, Ufo * ufo, Audio * audio);

void drawInfo(ALLEGRO_FONT ** font, Abm * abm, Level * level);
void transition(ALLEGRO_FONT ** font, ALLEGRO_TIMER * timer, Abm * abm, Level * level, int numUnusedAbm);

void drawObjects(Base * base, int baseCount, ALLEGRO_BITMAP * imageMissile, Abm * abm, ALLEGRO_BITMAP * imageBase, ALLEGRO_BITMAP * background,
	ALLEGRO_BITMAP * ground);

void initBase(Base * base, int baseCount, Level level);
void baseCollision(Base * base, Enemy ** enemy, int baseCount, Level * level, Ufo * ufo, Scm * scm);

void initColorMap(int colorMap[][3]);
void generateTheme(int * theme);
void clampSquare(Explosion * explosion, Enemy enemy, Vector * clamp);
bool calcDistance(Vector distance, Explosion explosion, Vector clamp);

void initLevel(Level * level);

void updateScm(Level * level, Scm * scm, Explosion * explosion);

void oneTimeInit(Level * level);

void horizontalEvasion(Scm * scm, Explosion * explosion, Level * level);
void verticalEvasion(Scm * scm, Explosion * explosion, Level * level);

void scmCollision(Scm * scm, Explosion * explosion, Level * level, Audio * audio);

void clampScm(Explosion * explosion, Scm * scm, Vector * clamp);

void updateUfo(Ufo * ufo, Level * level);
void spawnUfoMissile(Ufo * ufo, Level * level, Base * base);
void updateUfoMissile(Ufo * ufo, Level * level);
void calcUfoMissileInc(Enemy * missile);

void titleScreen(ALLEGRO_BITMAP * imageBase, ALLEGRO_BITMAP * background, ALLEGRO_BITMAP * imageMissile, ALLEGRO_BITMAP * ground, 
	ALLEGRO_EVENT_QUEUE * event_queue, ALLEGRO_FONT ** font, Audio audio, Level level);

bool calcBoundingBox(Base base, Enemy enemy);

void readScore(FILE * fptr, Level * level);
void calcScore(Level * level);
void sortScore(FILE * fptr, Level * level);

int pickTarget(Base * base);

bool levelProceed(Level * level, Explosion * explosion, Enemy ** enemy, Ufo * ufo, Scm * scm);
void loadNextLevel(Level * level, Abm * abm, Base * base);