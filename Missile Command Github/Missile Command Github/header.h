#pragma once

//globals 

const float FPS = 60;
const int SCREEN_W = 900;
const int SCREEN_H = 900;
const int BOUNCER_SIZE = 32;
const int NUM_BULLETS = 100;
const int ROWS = 5;   //rows of enemies
const int COLS = 3;  //columns of enemies 
const int NUM_ENEMIES_TOTAL = 8;
const int frameCount = 33; 
const int MAX_ENEMY = 5; //max enemies on screen at one time
const int ENEMY_COUNT = 20; //20 enemies 

enum KEYS {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE  //by default member 1 = 0, member 2 = 1, member 3 = 2, member 4 = 3... 
};

enum ID { PLAYER, BULLET, ENEMY };

enum BATTERY { LEFT, CENTER, RIGHT };

enum MOUSE {LMB_UP, LMB_DOWN };

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
struct abmData {
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
	int distance; 
	bool exploded; 
	int curr; 
	float explosionRadius; 
	bool increaseRadius; 
};

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
	int distance;
	bool exploded;
	float explosionRadius;
	bool increaseRadius;
	int branch; 
}Enemy;


//prototypes
int initAllegro(ALLEGRO_DISPLAY **display, ALLEGRO_TIMER **timer, ALLEGRO_BITMAP **imageCrosshair, ALLEGRO_BITMAP **frameExplosion, ALLEGRO_EVENT_QUEUE **event_queue);
void initCrosshair(Crosshair * crosshair, ALLEGRO_BITMAP * imageCrosshair);
void initAbm(struct abmData * abm);
void initEnemy(Enemy * enemy); 

void playerMovement(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_BITMAP **frameExplosion, ALLEGRO_EVENT_QUEUE *event_queue, Crosshair * crosshair, struct abmData * abm, Enemy * enemy, int * curr_enemy_num, int * num_spawned);

void drawCrosshair(ALLEGRO_BITMAP *imageCrosshair, struct crosshairData data);

void fire(struct abmData * abm, Crosshair crosshair);
void updateAbm(struct abmData * abm);
void drawAbm(struct abmData * abm);
void abmArrival(struct abmData * abm);  //check if abm arrived
void drawExplosion(struct abmData * abm); 

void spawnEnemy(Enemy * enemy, int * curr_enemy_count, int * num_spawned);
void drawEnemy(Enemy * enemy); 
void updateEnemy(Enemy * enemy); 
void enemyArrival(Enemy * enemy, int * curr_num_enemy); 
