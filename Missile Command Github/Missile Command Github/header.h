//#pragma once

//globals 
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define NUM_COLORS 8
#define COLORS_PER_THEME 3  //3 colours generated per level; one for enemy missiles, one for player missiles, one for enemy ufo missiles 

/*#define RED		  al_map_rgb(230, 25, 75)
#define GREEN	  al_map_rgb(60, 180, 75)
#define YELLOW	  al_map_rgb(255, 225, 25)
#define BLUE	  al_map_rgb(0, 130, 200)
#define ORANGE    al_map_rgb(245, 130, 48)
#define CYAN	  al_map_rgb(70, 240, 240)
#define MAGENTA	  al_map_rgb(240, 50, 230)
#define LIME	  al_map_rgb(210, 245, 60)
#define PINK	  al_map_rgb(250, 190, 190)
#define TEAL	  al_map_rgb(0, 128, 128)
#define LAVENDER  al_map_rgb(230, 190, 255)
#define BEIGE	  al_map_rgb(255, 250, 200)
#define MINT	  al_map_rgb(170, 255, 195)
#define CORAL	  al_map_rgb(255, 215, 180)
#define WHITE	  al_map_rgb(255, 255, 255)
#define TURQUOISE al_map_rgb(64, 224, 207)*/

const float FPS = 60;
const int SCREEN_W = 900;
const int SCREEN_H = 900;
const int NUM_BULLETS = 100;
const int ROWS = 5;   //rows of enemies
const int COLS = 3;  //columns of enemies 
const int frameCount = 33;
const int MAX_SPLIT = 10;
const int SPLIT_COUNT = 4;
const int SIZE = 1.5;  //3x3 square
const int ABM_COUNT = 30;

enum KEYS {
	ENTER, SPACE, ESCAPE
};

enum COLORS {
	BLUE, GREEN, RED, YELLOW, PINK, ORANGE, LIGHT_BLUE, MAGENTA
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

//crosshair 
typedef struct crosshairData {
	float height;
	float width;
	Vector pos;
	Vector target;
} Crosshair;

//each individual ABM has these properties 
typedef struct abmData {
	Vector dest; //destination 
	Vector launch; //launch coordinate 
	float dx;
	float dy;

	Vector inc;
	Vector pos;

	/*float x_inc;
	float y_inc;
	float x_pos;
	float y_pos;*/

	float step;
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

	double xNew;
	double yNew;

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
	bool arrived;
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
	int batteryAbmLeft[3];
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

void playerMovement(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_EVENT_QUEUE *event_queue, Crosshair crosshair,
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