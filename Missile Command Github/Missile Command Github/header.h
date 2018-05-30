//#pragma once

//globals 
#define NUM_COLORS 9
#define COLORS_PER_THEME 4
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
const int BOUNCER_SIZE = 32;
const int NUM_BULLETS = 100;
const int ROWS = 5;   //rows of enemies
const int COLS = 3;  //columns of enemies 
const int frameCount = 33;
const int MAX_ENEMY = 5; //max enemies on screen at one time
const int MAX_SPLIT = 10;
const int SPLIT_COUNT = 4;
const int SIZE = 1.5;  //3x3 square
const int ABM_COUNT = 30;
const int MAX_UFO = 3; 

enum KEYS {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE  //by default member 1 = 0, member 2 = 1, member 3 = 2, member 4 = 3... 
};

enum COLORS {
	NEON_RED, NEON_GREEN, NEON_YELLOW, NEON_BLUE, LIME, NEON_PINK, RED, TEAL, YELLOW
};

enum RGB {
	R, G, B
};

enum DIRECTION {
	LEFT, RIGHT
};


//crosshair 
typedef struct crosshairData {
	float x;
	float y;
	float height;
	float width;
	float target_x;
	float target_y;
} Crosshair;

typedef struct coordinates {
	int x;
	int y;
} Vector;

//each individual ABM has these properties 
typedef struct abmData {
	int dest_x;
	int dest_y;
	int launch_x;
	int launch_y;
	float dx;
	float dy;
	float x_inc;
	float y_inc;
	float x_pos;
	float y_pos;
	float step;
	int speed;
	bool launched;
	bool arrived;
	int num_increment;
} Abm;

typedef struct explosionData {
	bool ongoing;
	float radius;
	bool increaseRadius;
	bool expandedRadius;
	Vector center;
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Explosion;

typedef struct enemyData {
	int dest_x;
	int dest_y;
	int launch_x;
	int launch_y;
	float dx;
	float dy;
	float x_inc;
	float y_inc;
	float x_pos;
	float y_pos;
	float step;
	bool launched;
	bool arrived;
	int relativeX;
	int relativeY;
	int distX;
	int distY;
	int distTotal;
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Enemy;

typedef struct ufoData {
	bool spawned;
	bool arrived; 
	int origin; 
	Vector pos; 
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft; 
} Ufo;

typedef struct baseData {
	bool destroyed;
	Vector pos;
	Vector topRight;
	Vector topLeft;
	Vector bottomLeft;
} Base;

typedef struct levelData {
	int spawnRate;
	int splitRate;
	int splitAngle;
	int enemySpeed; 
	int round; //round 1, round 2...
	int curr_enemy_count; 
	int num_spawned;
	int spawnLimit; 
	int lives;
	int abmLeft; 
	int batteryAbmLeft[3]; 
	int score; 
	bool spawnUfo;
	int ufoSpeed; 
	int curr_ufo_count;
	//int maxUfoOnScreen;
	int ufoSpawnLimit; 
	int ufoSpawnRate; 
	int ufoNumSpawned; 
	int ufoSpawnSide[2]; 
} Level;

//prototypes
int initAllegro(ALLEGRO_DISPLAY **display, ALLEGRO_TIMER **timer, ALLEGRO_BITMAP **imageCrosshair, ALLEGRO_EVENT_QUEUE **event_queue, ALLEGRO_FONT ** font, 
	ALLEGRO_BITMAP ** background, ALLEGRO_BITMAP ** imageUfo);
void initCrosshair(Crosshair * crosshair, ALLEGRO_BITMAP * imageCrosshair);
void initAbm(struct abmData * abm, Explosion * explosion);

void playerMovement(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_EVENT_QUEUE *event_queue, Crosshair crosshair,
	struct abmData * abm, Enemy ** enemy, ALLEGRO_FONT * font, Base * base, Explosion * explosion, int * theme, int colorMap[][3], Level * level, ALLEGRO_BITMAP * background,
	ALLEGRO_BITMAP * imageUfo, Ufo * ufo);

void drawCrosshair(ALLEGRO_BITMAP *imageCrosshair, Crosshair * crosshair);

void fire(Abm * abm, Crosshair crosshair, Level * level);
void calcAbmInc(Abm * abm);
void updateAbm(struct abmData * abm);
void drawAbm(struct abmData * abm, int * theme, int colorMap[][3]);
void abmArrival(Abm * abm, Explosion * explosion);  //check if abm arrived
void drawExplosion(Abm * abm, Explosion * explosion, int colorMap[][3]);

void initEnemy(Enemy ** enemy, Level * level, Ufo * ufo);
void spawnEnemy(Enemy ** enemy, Level * level, Ufo * ufo);
void calcEnemyInc(Enemy * enemy);
void drawEnemy(Enemy ** enemy, int * theme, int colorMap[][3], Level * level, Ufo * ufo, ALLEGRO_BITMAP * imageUfo);
void updateEnemy(Enemy ** enemy, Level * level, Ufo * ufo);
void enemyArrival(Enemy ** enemy, Level * level, Ufo * ufo);

void hitDetection(struct abmData * abm, Enemy ** enemy, Explosion * explosion, Level * level);

void drawInfo(ALLEGRO_FONT * font, Abm * abm, Level * level);
void transition(ALLEGRO_FONT * font, ALLEGRO_TIMER * timer, Abm * abm, Level * level);

void drawObjects(Base * base, int baseCount, int * theme, int colorMap[][3]);

void initBase(Base * base, int baseCount);
void baseCollision(Base * base, Enemy ** enemy, int baseCount, Level * level);

void initColorMap(int colorMap[][3]);
void generateTheme(int * theme);
void clampSquare(Explosion * explosion, Enemy * enemy, Vector * clamp);

void initLevel(Level * level); 