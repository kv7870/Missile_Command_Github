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

enum KEYS {
	KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT, KEY_SPACE  //by default member 1 = 0, member 2 = 1, member 3 = 2, member 4 = 3... 
};

enum ID { PLAYER, BULLET, ENEMY };

enum BATTERY { LEFT, CENTER, RIGHT };

//crosshair 
struct crosshairData {
	float x;
	float y;
	float height;
	float width;
};


//each individual ABM has these properties 
struct abmData {
	float dest_x;
	float dest_y;
	float launch_x;
	float launch_y;
	int speed;
	bool used;
};


//prototypes
int initAllegro(ALLEGRO_DISPLAY **display, ALLEGRO_TIMER **timer, ALLEGRO_BITMAP **imageCrosshair, ALLEGRO_EVENT_QUEUE **event_queue);
void initCrosshair(struct crosshairData * crosshair, ALLEGRO_BITMAP * imageCrosshair);
void initAbm(struct abmData * abm);

void playerMovement(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_EVENT_QUEUE *event_queue, struct crosshairData crosshair, struct abmData * abm);

void drawCrosshair(ALLEGRO_BITMAP *imageCrosshair, struct crosshairData data);
