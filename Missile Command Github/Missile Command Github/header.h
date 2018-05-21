#pragma once

//globals 

const float FPS = 60;
const int SCREEN_W = 900;
const int SCREEN_H = 900;
const int BOUNCER_SIZE = 32;
const int NUM_BULLETS = 100;
const int ROWS = 5;   //rows of enemies
const int COLS = 3;  //columns of enemies 
const int frameCount = 33; 
const int MAX_ENEMY = 5; //max enemies on screen at one time
const int ENEMY_COUNT = 20; //20 enemies 
const int SIZE = 1.5;  //3x3 square
const int ABM_COUNT = 30; 

enum KEYS {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE  //by default member 1 = 0, member 2 = 1, member 3 = 2, member 4 = 3... 
};

enum ID { PLAYER, BULLET, ENEMY };

enum BATTERY { LEFT, CENTER, RIGHT };

typedef struct vectorData {
	int x; 
	int y; 
} Vector; 

//crosshair 
typedef struct crosshairData {
	float x;
	float y;
	float height;
	float width;
	float target_x;
	float target_y;
} Crosshair;


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
	float distance; 
	bool exploded; 
	float explosionRadius; 
	bool increaseRadius; 
	Vector topRight; 
	Vector topLeft;
	Vector bottomRight;
	Vector bottomLeft;
}Abm;

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
	int speed;
	bool launched;
	bool arrived;
	float distance;
	bool exploded;
	bool split; 
	Vector topRight;
	Vector topLeft;
	Vector bottomRight;
	Vector bottomLeft;
	int relativeX;
	int relativeY;
	int distX;
	int distY; 
	int distTotal; 
	bool hit; 
	int dist_to_dest; 
	//bounds
} Enemy;


//multiple independently targetable reentry vehicle
typedef struct mirvData { 
	Enemy branch[3]; 
} Mirv; 



//prototypes
int initAllegro(ALLEGRO_DISPLAY **display, ALLEGRO_TIMER **timer, ALLEGRO_BITMAP **imageCrosshair, ALLEGRO_BITMAP **frameExplosion, ALLEGRO_EVENT_QUEUE **event_queue);
void initCrosshair(Crosshair * crosshair, ALLEGRO_BITMAP * imageCrosshair);
void initAbm(struct abmData * abm);

void playerMovement(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_BITMAP **frameExplosion, ALLEGRO_EVENT_QUEUE *event_queue, Crosshair * crosshair, struct abmData * abm, Enemy * enemy, int * curr_enemy_num, int * num_spawned, Mirv * mirv);

void drawCrosshair(ALLEGRO_BITMAP *imageCrosshair, struct crosshairData data);

void fire(struct abmData * abm, Crosshair crosshair);
void calcIncrement(Abm * abm, int i); 
void updateAbm(struct abmData * abm);
void drawAbm(struct abmData * abm);
void abmArrival(Abm * abm);  //check if abm arrived
void drawExplosion(Abm * abm); 

void initEnemy(Enemy * enemy, Mirv * mirv);
void spawnEnemy(Enemy * enemy, int * curr_enemy_count, int * num_spawned);
void drawEnemy(Enemy * enemy);
void updateEnemy(Enemy * enemy);
void enemyArrival(Enemy * enemy, int * curr_num_enemy);

void spawnMirv(Enemy * enemy, Mirv * mirv);
void updateMirv(Mirv * mirv);
void drawMirv(Mirv * mirv);
void mirvArrival(Mirv * mirv, Enemy * enemy); 

void hitDetection(struct abmData * abm, Enemy * enemy); 