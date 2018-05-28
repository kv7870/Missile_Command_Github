#include "stdafx.h"
#include <stdio.h>
#include <allegro5/allegro.h> // Include the allegro header file.
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "header.h"					
#include <time.h>
#include <stdlib.h>

int initAllegro(ALLEGRO_DISPLAY ** display, ALLEGRO_TIMER ** timer, ALLEGRO_BITMAP ** imageCrosshair, ALLEGRO_EVENT_QUEUE ** event_queue, ALLEGRO_FONT ** font) {

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	al_init_font_addon();
	al_init_ttf_addon();

	//create display
	*display = al_create_display(SCREEN_W, SCREEN_H);
	if (!(*display)) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(*timer);
		return -1;
	}

	//create timer
	*timer = al_create_timer(1.0 / FPS);   //1/60 seconds per frame, i.e. 60 fps
	if (!(*timer)) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	*font = al_load_ttf_font("Roboto-Regular.ttf", 24, 0);
	//load font
	if (!font) {
		fprintf(stderr, "Could not load 'Roboto-Regular.ttf'.\n");
		return -1;
	}

	//create event queue 
	*event_queue = al_create_event_queue();
	if (!(*event_queue)) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_bitmap(*imageCrosshair);
		al_destroy_display(*display);
		al_destroy_timer(*timer);
		return -1;
	}

	//install keyboard 
	if (!al_install_keyboard()) {
		fprintf(stderr, "failed to initialize the keyboard!\n");
		return -1;
	}

	//install mouse 
	if (!al_install_mouse()) {
		fprintf(stderr, "failed to initialize the mouse!\n");
		return -1;
	}

	if (!al_init_primitives_addon()) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to initialize image addon!",
			nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	//initialize image addon 
	if (!al_init_image_addon()) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to initialize image addon!",
			nullptr, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}

	//create ship image
	*imageCrosshair = al_load_bitmap("crosshair.png");
	if (!imageCrosshair) {
		al_show_native_message_box(*display, "Error", "Error", "Failed to load image!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);
		al_destroy_display(*display);
		return 0;
	}

	//register 
	al_register_event_source(*event_queue, al_get_display_event_source(*display));
	al_register_event_source(*event_queue, al_get_timer_event_source(*timer));
	al_register_event_source(*event_queue, al_get_keyboard_event_source());
	al_register_event_source(*event_queue, al_get_mouse_event_source());

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_start_timer(*timer);
}



//spawn crosshair at center of screen y
void initCrosshair(Crosshair * crosshair, ALLEGRO_BITMAP * imageCrosshair) {
	crosshair->x = 450;
	crosshair->y = 450;
	crosshair->height = al_get_bitmap_height(imageCrosshair);
	crosshair->width = al_get_bitmap_width(imageCrosshair);
}


void initLevel(Level * level) {
	level->num_spawned = 0;
	level->curr_enemy_count = 0;
	level->abmLeft = 30;

	for (int i = 0; i < 3; i++) {
		level->batteryAbmLeft[i] = 10;
	}
}

//initialize abm 
void initAbm(struct abmData * abm, Explosion * explosion) {
	int i;

	for (i = 0; i < ABM_COUNT; i++) {
		abm[i].dest_x = 0;
		abm[i].dest_y = 0;
		abm[i].launch_y = 850;
		abm[i].launched = false;
		abm[i].dx = 0;
		abm[i].dy = 0;
		abm[i].x_inc = 0;
		abm[i].y_inc = 0;
		abm[i].x_pos = 0;
		abm[i].y_pos = 0;
		abm[i].step = 0;
		abm[i].arrived = false;
		abm[i].num_increment = 1;

		explosion[i].ongoing = false;
		explosion[i].radius = 0;
		explosion[i].increaseRadius = true;
		explosion[i].expandedRadius = false;
		explosion[i].topRight.x = 0;
		explosion[i].topRight.y = 0;
		explosion[i].topLeft.x = 0;
		explosion[i].topLeft.y = 0;
		explosion[i].bottomLeft.x = 0;
		explosion[i].bottomLeft.y = 0;
		explosion[i].center.x = 0;
		explosion[i].center.y = 0;
	};

	//1st battery (left)
	for (i = 0; i < 10; i++) {
		abm[i].launch_x = 40;
		abm[i].speed = 7;
	}

	//2nd battery (center)
	for (i = 10; i < 20; i++) {
		abm[i].launch_x = 425;
		abm[i].speed = 14;
	}

	//3rd battery (right)
	for (i = 20; i < 30; i++) {
		abm[i].launch_x = 855;
		abm[i].speed = 7;
	}
}

void initEnemy(Enemy ** enemy, Level * level) {
	for (int i = 0; i < level->spawnLimit; i++) {
		for (int j = 0; j < SPLIT_COUNT; j++) {

			//init main enemy missile 
			enemy[i][j].dest_x = 0;
			enemy[i][j].dest_y = 0;
			enemy[i][j].launched = false;
			enemy[i][j].dx = 0;
			enemy[i][j].dy = 0;
			enemy[i][j].x_inc = 0;
			enemy[i][j].y_inc = 0;
			enemy[i][j].x_pos = 0;
			enemy[i][j].y_pos = 0;
			enemy[i][j].step = 0;
			enemy[i][j].speed = 0;
			enemy[i][j].arrived = false;
			enemy[i][j].launch_y = 50;
			enemy[i][j].dest_y = 900;
			enemy[i][j].relativeX = 0;
			enemy[i][j].relativeY = 0;
			enemy[i][j].distX = 0;
			enemy[i][j].distY = 0;
			enemy[i][j].distTotal = 0;
			enemy[i][j].topRight.x = 0;
			enemy[i][j].topRight.y = 0;
			enemy[i][j].topLeft.x = 0;
			enemy[i][j].topLeft.y = 0;
			enemy[i][j].bottomLeft.x = 0;
			enemy[i][j].bottomLeft.y = 0;
		}

	}
}


void initBase(Base * base, int baseCount) {
	int i, j;

	for (i = 0, j = 120; i < baseCount / 2; i++, j += 90) {
		base[i].pos.x = j;
		base[i].pos.y = 870;
	}

	for (i = 3, j = 505; i < 6; i++, j += 100) {
		base[i].pos.x = j;
		base[i].pos.y = 870;
	}

	//bounds
	for (i = 0; i < baseCount; i++) {
		base[i].destroyed = false;
		base[i].topLeft.x = base[i].pos.x;
		base[i].topLeft.y = base[i].pos.y;
		base[i].topRight.x = base[i].pos.x + 50;
		base[i].topRight.y = base[i].pos.y;
		base[i].bottomLeft.x = base[i].pos.x;
		base[i].bottomLeft.y = base[i].pos.y + 30;
	}
}


void initColorMap(int colorMap[][3]) {

	colorMap[RED][0] = 230;
	colorMap[RED][1] = 25;
	colorMap[RED][2] = 75;

	colorMap[GREEN][0] = 60;
	colorMap[GREEN][1] = 180;
	colorMap[GREEN][2] = 75;

	colorMap[YELLOW][0] = 255;
	colorMap[YELLOW][1] = 225;
	colorMap[YELLOW][2] = 25;

	colorMap[BLUE][0] = 0;
	colorMap[BLUE][1] = 130;
	colorMap[BLUE][2] = 200;

	colorMap[ORANGE][0] = 245;
	colorMap[ORANGE][1] = 130;
	colorMap[ORANGE][2] = 48;

	colorMap[CYAN][0] = 70;
	colorMap[CYAN][1] = 240;
	colorMap[CYAN][2] = 240;

	colorMap[MAGENTA][0] = 240;
	colorMap[MAGENTA][1] = 50;
	colorMap[MAGENTA][2] = 230;

	colorMap[LIME][0] = 210;
	colorMap[LIME][1] = 245;
	colorMap[LIME][2] = 60;

	colorMap[PINK][0] = 250;
	colorMap[PINK][1] = 190;
	colorMap[PINK][2] = 190;

	colorMap[TEAL][0] = 0;
	colorMap[TEAL][1] = 128;
	colorMap[TEAL][2] = 128;

	colorMap[LAVENDER][0] = 230;
	colorMap[LAVENDER][1] = 190;
	colorMap[LAVENDER][2] = 255;

	colorMap[BEIGE][0] = 255;
	colorMap[BEIGE][1] = 250;
	colorMap[BEIGE][2] = 200;

	colorMap[MINT][0] = 170;
	colorMap[MINT][1] = 255;
	colorMap[MINT][2] = 195;

	colorMap[WHITE][0] = 255;
	colorMap[WHITE][1] = 225;
	colorMap[WHITE][2] = 255;

}



