//#pragma once

//globals 
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define NUM_COLORS 7 //missile colours
#define COLORS_PER_THEME 3  //3 colours generated each level; enemy missiles, player missiles, and ufo missiles have different colours

const float FPS = 60;
const int SCREEN_W = 900;
const int SCREEN_H = 900;
const int ABM_COUNT = 30;  //player can fire 30 anti-ballistic missiles every level
const int SPLIT_COUNT = 4; //each enemy missile can split into 4 
const int SIZE = 1.5;  //half the size of a 3x3 missile 

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

typedef struct vector {
	float x;
	float y;
} Vector;


//reticle for aiming
typedef struct crosshairData {
	float height;
	float width;
	Vector pos;
	Vector target;
} Crosshair;


//player anti-ballistic missiles
typedef struct abmData {
	Vector dest;   //destination
	Vector launch; //launch coordinate 
	float dx;   //delta x; for calculating linear trajectory
	float dy;   //delta y; for calculating linear trajectory
	Vector inc; //increment for calculating linear trajectory 
	float step; //similar to increment
	Vector pos; //current position (coordinates) 
	int speed; 
	bool launched;  
	bool arrived;
	int num_increment;
} Abm;

typedef struct explosionData {
	bool ongoing;
	int radius;
	bool increaseRadius;
	bool expandedRadius;

	Vector center;
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Explosion;

typedef struct enemyData {
	Vector dest;
	Vector launch;
	float dx;
	float dy;
	Vector inc;
	Vector pos;
	float step;
	bool launched;
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Enemy;

typedef struct ufoData {
	bool spawned;

	//launch x
	int origin;

	Enemy missile[2];

	Vector pos;
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Ufo;

typedef struct bombData {
	bool spawned;
	bool moveLeft; 
	int timerCount; 
	Vector origin;
	Vector target;
	Vector pos;
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Bomb;

typedef struct baseData {
	bool destroyed;
	Vector pos;
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Base;

typedef struct levelData {
	int round; //round 1, round 2...
	int lives;
	int score;
	int abmLeft;
	int highScores[5];
	int highScoreCount;
	bool newHighScore;
	bool initialSort;

	int spawnRangeMin;
	int spawnRangeMax;

	int splitRangeMin;
	int splitRangeMax;

	int ufoSpawnRangeMin;
	int ufoSpawnRangeMax; 


	int maxEnemyOnScreen;
	int maxUfoOnScreen;
	int maxBombOnScreen;

	int spawnRate;
	int splitRate;
	int splitAngle;
	float enemySpeed;
	int num_spawned;
	int spawnLimit;

	//ufo
	int ufoSpawnLimit;
	int ufoSpawnRate;
	int ufoNumSpawned;
	int ufoSpawnSide[2];
	int ufoMissileSpawnRate;
	bool spawnUfo;
	int ufoSpeed;
	Vector ufoSize;

	//bomb
	bool spawnBomb;
	int bombSpawnLimit;
	int bombNumSpawned;
	int bombSpeed;
	int bombSpawnRate;
	Vector bombSize;

	//base
	Vector baseSize;
	int baseX[6];
} Level;

typedef struct audio {
	ALLEGRO_SAMPLE * siren;
	ALLEGRO_SAMPLE * missileLaunch;
	ALLEGRO_SAMPLE * explosion[6];
} Audio;

//prototypes
void initAllegro(ALLEGRO_DISPLAY **display, ALLEGRO_TIMER **timer, ALLEGRO_BITMAP **imageCrosshair, ALLEGRO_EVENT_QUEUE **event_queue, ALLEGRO_FONT ** font,
	ALLEGRO_BITMAP ** background, ALLEGRO_BITMAP ** imageUfo, Level * level, ALLEGRO_BITMAP ** imageBomb, ALLEGRO_BITMAP ** imageMissile, ALLEGRO_BITMAP ** ground,
	ALLEGRO_BITMAP ** imageBase, ALLEGRO_FONT ** titleFont, Audio * audio);

void initCrosshair(Crosshair * crosshair, ALLEGRO_BITMAP * imageCrosshair);
void initAbm(struct abmData * abm, Explosion * explosion);

void gameLoop(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_EVENT_QUEUE *event_queue, Crosshair crosshair,
	struct abmData * abm, Enemy ** enemy, ALLEGRO_FONT * font, Base * base, Explosion * explosion, int * theme, int colorMap[][3], Level * level, ALLEGRO_BITMAP * background,
	ALLEGRO_BITMAP * imageUfo, Ufo * ufo, ALLEGRO_BITMAP ** imageBomb, Bomb * bomb, ALLEGRO_BITMAP * imageMissile, ALLEGRO_BITMAP * ground, ALLEGRO_BITMAP * imageBase,
	ALLEGRO_FONT * titleFont, FILE * fptr, Audio * audio);

void drawCrosshair(ALLEGRO_BITMAP *imageCrosshair, Crosshair * crosshair);

void fire(Abm * abm, Crosshair crosshair, Level * level, Audio * audio);
void calcAbmInc(Abm * abm);
void updateAbm(struct abmData * abm);
void drawAbm(struct abmData * abm, int abmColour, int colorMap[][3]);
void abmArrival(Abm * abm, Explosion * explosion);  //check if abm arrived
void drawExplosion(Abm * abm, Explosion * explosion, int colorMap[][3]);

void initEnemy(Enemy ** enemy, Level * level, Ufo * ufo, Bomb * bomb);
void spawnEnemy(Enemy ** enemy, Level * level, Ufo * ufo, Bomb * bomb, Base * base);
void calcEnemyInc(Enemy * enemy);
void drawEnemy(Enemy ** enemy, int * theme, int colorMap[][3], Level * level, Ufo * ufo, ALLEGRO_BITMAP * imageUfo, ALLEGRO_BITMAP ** imageBomb, Bomb * bomb);
void updateEnemy(Enemy ** enemy, Level * level);
void enemyArrival(Enemy ** enemy, Level * level, Ufo * ufo, Bomb * bomb);

void hitDetection(struct abmData * abm, Enemy ** enemy, Explosion * explosion, Level * level, Ufo * ufo, Audio * audio);

void drawInfo(ALLEGRO_FONT * font, Abm * abm, Level * level);
void transition(ALLEGRO_FONT * font, ALLEGRO_TIMER * timer, Abm * abm, Level * level);

void drawObjects(Base * base, int baseCount, ALLEGRO_BITMAP * imageMissile, Abm * abm, ALLEGRO_BITMAP * imageBase, ALLEGRO_BITMAP * background,
	ALLEGRO_BITMAP * ground);

void initBase(Base * base, int baseCount, Level level);
void baseCollision(Base * base, Enemy ** enemy, int baseCount, Level * level, Ufo * ufo, Bomb * bomb);

void initColorMap(int colorMap[][3]);
void generateTheme(int * theme);
void clampSquare(Explosion * explosion, Enemy enemy, Vector * clamp);
bool calcDistance(Vector distance, Explosion explosion, Vector clamp);

void initLevel(Level * level);

void updateBomb(Level * level, Bomb * bomb, Explosion * explosion);

void oneTimeInit(Level * level);

void horizontalEvasion(Bomb * bomb, Explosion * explosion, Level * level);
void verticalEvasion(Bomb * bomb, Explosion * explosion, Level * level);

void bombHitDetection(Bomb * bomb, Explosion * explosion, Level * level, Audio * audio);

void clampBomb(Explosion * explosion, Bomb * bomb, Vector * clamp);

void updateUfo(Ufo * ufo, Level * level);
void spawnUfoMissile(Ufo * ufo, Level * level);
void updateUfoMissile(Ufo * ufo, Level * level);
void calcUfoMissileInc(Enemy * missile);

void titleScreen(ALLEGRO_BITMAP * imageBase, ALLEGRO_BITMAP * background, ALLEGRO_BITMAP * imageMissile, ALLEGRO_BITMAP * ground, ALLEGRO_EVENT_QUEUE * event_queue,
	ALLEGRO_FONT * titleFont, ALLEGRO_FONT * font, Audio audio, Level level);

bool calcBoundingBox(Base base, Enemy enemy);

void readScore(FILE * fptr, Level * level);
void calcScore(Level * level);
void sortScore(FILE * fptr, Level * level);

int pickTarget(Base * base);

bool levelProceed(Level * level, Explosion * explosion, Enemy ** enemy, Ufo * ufo, Bomb * bomb);
void loadNextLevel(Level * level, Abm * abm, Base * base);