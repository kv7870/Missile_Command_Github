#include "stdafx.h"
#include <stdio.h>
#include <allegro5/allegro.h> // Include the allegro header file.
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "header.h"																					
																												   //or *frameExplosion[]
int initAllegro(ALLEGRO_DISPLAY ** display, ALLEGRO_TIMER ** timer, ALLEGRO_BITMAP ** imageCrosshair, ALLEGRO_BITMAP ** frameExplosion, ALLEGRO_EVENT_QUEUE ** event_queue) {

	if (!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

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

	frameExplosion[0] = al_load_bitmap("slice_0_0.png");
	frameExplosion[1] = al_load_bitmap("slice_0_1.png");
	frameExplosion[2] = al_load_bitmap("slice_0_2.png");
	frameExplosion[3] = al_load_bitmap("slice_0_3.png");
	frameExplosion[4] = al_load_bitmap("slice_0_4.png");
	frameExplosion[5] = al_load_bitmap("slice_0_5.png");
	frameExplosion[6] = al_load_bitmap("slice_1_0.png");
	frameExplosion[7] = al_load_bitmap("slice_1_1.png");
	frameExplosion[8] = al_load_bitmap("slice_1_2.png");
	frameExplosion[9] = al_load_bitmap("slice_1_3.png");
	frameExplosion[10] = al_load_bitmap("slice_1_4.png");
	frameExplosion[11] = al_load_bitmap("slice_1_5.png");
	frameExplosion[12] = al_load_bitmap("slice_2_0.png");
	frameExplosion[13] = al_load_bitmap("slice_2_1.png");
	frameExplosion[14] = al_load_bitmap("slice_2_2.png");
	frameExplosion[15] = al_load_bitmap("slice_2_3.png");
	frameExplosion[16] = al_load_bitmap("slice_2_4.png");
	frameExplosion[17] = al_load_bitmap("slice_2_5.png");
	frameExplosion[18] = al_load_bitmap("slice_3_0.png");
	frameExplosion[19] = al_load_bitmap("slice_3_1.png");
	frameExplosion[20] = al_load_bitmap("slice_3_2.png");
	frameExplosion[21] = al_load_bitmap("slice_3_3.png");
	frameExplosion[22] = al_load_bitmap("slice_3_4.png");
	frameExplosion[23] = al_load_bitmap("slice_3_5.png");
	frameExplosion[24] = al_load_bitmap("slice_4_0.png");
	frameExplosion[25] = al_load_bitmap("slice_4_1.png");
	frameExplosion[26] = al_load_bitmap("slice_4_2.png");
	frameExplosion[27] = al_load_bitmap("slice_4_3.png");
	frameExplosion[28] = al_load_bitmap("slice_4_4.png");
	frameExplosion[29] = al_load_bitmap("slice_4_5.png");
	frameExplosion[30] = al_load_bitmap("slice_5_0.png");
	frameExplosion[31] = al_load_bitmap("slice_5_1.png");
	frameExplosion[32] = al_load_bitmap("slice_5_2.png");

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



//initialize abm 
void initAbm(struct abmData * abm) {
	int i;

	for (i = 0; i < 30; i++) {
		abm[i].dest_x = 0;
		abm[i].dest_y = 0;
		abm[i].used = false;
		abm[i].launched = false;
		abm[i].dx = 0;
		abm[i].dy = 0;
		abm[i].x_inc = 0;
		abm[i].y_inc = 0;
		abm[i].x_pos = 0;
		abm[i].y_pos = 0;
		abm[i].step = 0;
		abm[i].speed = 0;
		abm[i].arrived = false;
	};

	//1st battery (left)
	for (i = 0; i < 10; i++) {
		abm[i].launch_x = 55;
		abm[i].launch_y = 830;
		abm[i].speed = 5;
	}

	//2nd battery (center)
	for (i = 10; i < 20; i++) {
		abm[i].launch_x = 445;
		abm[i].launch_y = 830;
		abm[i].speed = 10;
	}

	//3rd battery (right)
	for (i = 20; i < 30; i++) {
		abm[i].launch_x = 845;
		abm[i].launch_y = 830;
		abm[i].speed = 5;
	}
}