#include "stdafx.h"
#include <stdio.h>
#include <allegro5/allegro.h> // Include the allegro header file.
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "header.h"


void playerMovement(ALLEGRO_DISPLAY *display, ALLEGRO_TIMER *timer, ALLEGRO_BITMAP *imageCrosshair, ALLEGRO_EVENT_QUEUE *event_queue, Crosshair * crosshair, struct abmData * abm) {

	//ship.x = SCREEN_W / 2.0 - BOUNCER_SIZE / 2.0;
	//ship.y = SCREEN_H / 2.0 - BOUNCER_SIZE / 2.0;
	bool done = false;
	bool draw = true;

	//move key[5] to global variable? 
	bool key[5] = { false, false, false, false, false };  //array with members KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT; each member is true or false 

	al_hide_mouse_cursor(display);


	while (!done) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {  //update every 1/60 of a second 
			draw = true;
			updateAbm(abm); 
		}

		else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			break;
		}

		else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES || ev.type == ALLEGRO_EVENT_MOUSE_ENTER_DISPLAY) {
			if (ev.mouse.x >= 4 && ev.mouse.x <= SCREEN_W - crosshair->width)
				crosshair->x = ev.mouse.x;
			if (ev.mouse.y >= 4 && ev.mouse.y <= SCREEN_H - crosshair->height/*-150*/)
				crosshair->y = ev.mouse.y;
		}

		else if (ev.type = ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			if (ev.mouse.button & 1) {
				crosshair->target_x = ev.mouse.x;   //get coordinate of target 
				crosshair->target_y = ev.mouse.y;
				printf("(%d, %d) ", ev.mouse.x, ev.mouse.y);
				fire(abm, *crosshair);
			}
		}

		//if key is being pressed 
		else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			switch (ev.keyboard.keycode) {

			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = true;
				crosshair->target_x = ev.mouse.x;
				crosshair->target_y = ev.mouse.y;
				//fire(crosshair, abm); 
				break;
			}
		}

		//if key is released 
		else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
			switch (ev.keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				key[KEY_UP] = false;
				break;

			case ALLEGRO_KEY_DOWN:
				key[KEY_DOWN] = false;
				break;

			case ALLEGRO_KEY_LEFT:
				key[KEY_LEFT] = false;
				break;

			case ALLEGRO_KEY_RIGHT:
				key[KEY_RIGHT] = false;
				break;

			case ALLEGRO_KEY_SPACE:
				key[KEY_SPACE] = false;
				break;

			case ALLEGRO_KEY_ESCAPE:
				done = true;
				break;
			}
		}

		//must draw everything every tick of timer 
		if (draw && al_is_event_queue_empty(event_queue)) {
			draw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));  //clear screen to black to create illusion of animation; draw & clear screen, draw & clear screen... 

			drawCrosshair(imageCrosshair, *crosshair);

			drawAbm(abm); 

			al_flip_display();
		}
	}
	al_destroy_bitmap(imageCrosshair);
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);
}

void drawCrosshair(ALLEGRO_BITMAP *imageCrosshair, struct crosshairData crosshair) {
	al_draw_bitmap(imageCrosshair, crosshair.x-27, crosshair.y-23, 0);
}