/*
MISSILE COMMAND SUMMATIVE BY KEVIN YANG 
FINAL VERSION
*/

#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h> // Include the allegro header file.
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include "header.h"
#include <time.h>
#include <stdlib.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

//draw title screen 
void titleScreen(ALLEGRO_BITMAP * imageBase, ALLEGRO_BITMAP * background, ALLEGRO_BITMAP * imageMissile, ALLEGRO_BITMAP * ground, 
	ALLEGRO_EVENT_QUEUE * event_queue, ALLEGRO_FONT ** font, Audio audio, Level level) {

	ALLEGRO_SAMPLE_ID siren_id;		//for stopping title screen sound
	bool start = false;
	int key[3] = { false, false, false };
	int color;

	ALLEGRO_EVENT ev;

	//colours for flashing letters 
	int palette[7][3] = { { 255, 0, 0 },{ 0, 255, 0 },{ 0, 0, 255 },{ 128, 128, 128 },{ 255, 255, 255 },{ 248, 6, 248 },{ 0, 255, 255 } };

	al_play_sample(audio.siren, 1.5, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &siren_id);	//play title screen "music" 

	do {

		color = rand() % 7;

		al_wait_for_event(event_queue, &ev);	

		//draw planet surface
		al_draw_bitmap(ground, -100, 750, 0);

		//draw first ABM battery
		al_draw_bitmap(imageMissile, 0, 770, 0);
		al_draw_bitmap(imageMissile, 10, 770, 0);
		al_draw_bitmap(imageMissile, 20, 770, 0);
		al_draw_bitmap(imageMissile, 30, 770, 0);
		al_draw_bitmap(imageMissile, 40, 770, 0);
		al_draw_bitmap(imageMissile, -10, 800, 0);
		al_draw_bitmap(imageMissile, 0, 800, 0);
		al_draw_bitmap(imageMissile, 10, 800, 0);
		al_draw_bitmap(imageMissile, 20, 800, 0);
		al_draw_bitmap(imageMissile, 30, 800, 0);

		//draw second ABM battery
		al_draw_bitmap(imageMissile, 400, 770, 0);
		al_draw_bitmap(imageMissile, 410, 770, 0);
		al_draw_bitmap(imageMissile, 420, 770, 0);
		al_draw_bitmap(imageMissile, 430, 770, 0);
		al_draw_bitmap(imageMissile, 440, 770, 0);
		al_draw_bitmap(imageMissile, 400, 800, 0);
		al_draw_bitmap(imageMissile, 410, 800, 0);
		al_draw_bitmap(imageMissile, 420, 800, 0);
		al_draw_bitmap(imageMissile, 430, 800, 0);
		al_draw_bitmap(imageMissile, 440, 800, 0);

		//draw third ABM battery
		al_draw_bitmap(imageMissile, 800, 770, 0);
		al_draw_bitmap(imageMissile, 810, 770, 0);
		al_draw_bitmap(imageMissile, 820, 770, 0);
		al_draw_bitmap(imageMissile, 830, 770, 0);
		al_draw_bitmap(imageMissile, 840, 770, 0);
		al_draw_bitmap(imageMissile, 810, 800, 0);
		al_draw_bitmap(imageMissile, 820, 800, 0);
		al_draw_bitmap(imageMissile, 830, 800, 0);
		al_draw_bitmap(imageMissile, 840, 800, 0);
		al_draw_bitmap(imageMissile, 850, 800, 0);

		//draw leftmost bases
		al_draw_bitmap(imageBase, 90, 810, 0);
		al_draw_bitmap(imageBase, 190, 810, 0);
		al_draw_bitmap(imageBase, 290, 810, 0);

		//draw rightmost bases
		al_draw_bitmap(imageBase, 505, 810, 0);
		al_draw_bitmap(imageBase, 605, 810, 0);
		al_draw_bitmap(imageBase, 705, 810, 0);

		//draw title screen letters
		al_draw_text(font[TITLE], al_map_rgb(255, 0, 0), 110, 200, 0, "MISSILE COMMAND");
		al_draw_text(font[TEXT], al_map_rgb(255, 0, 0), 450, 400, ALLEGRO_ALIGN_CENTRE, "HIGH SCORES");
		al_draw_text(font[TEXT], al_map_rgb(palette[color][0], palette[color][1], palette[color][2]), 450, 650, ALLEGRO_ALIGN_CENTER, "PRESS ANY KEY TO PLAY");

		//draw high scores
		for (int i = 0, y = 450; i < 5; i++, y += 25) {
			al_draw_textf(font[TEXT], al_map_rgb(255, 0, 0), 450, y, ALLEGRO_ALIGN_CENTRE, "%d", level.highScores[i]);
		}

		al_flip_display();
	}

	while (ev.type != ALLEGRO_EVENT_KEY_DOWN && ev.type != ALLEGRO_EVENT_MOUSE_BUTTON_DOWN);	//start game if mouse or key pressed

	al_stop_sample(&siren_id);	//stop title screen siren sound

	al_clear_to_color(al_map_rgb(0, 0, 0));
}


void instructionScreen(ALLEGRO_FONT ** font, ALLEGRO_EVENT_QUEUE * event_queue) {

	ALLEGRO_EVENT ev;

	do {

		al_wait_for_event(event_queue, &ev);

		al_draw_text(font[HEADING], al_map_rgb(255, 0, 0), 450, 225, ALLEGRO_ALIGN_CENTER, "OBJECTIVE");
		al_draw_text(font[TEXT], al_map_rgb(255, 0, 0), 450, 270, ALLEGRO_ALIGN_CENTER, "Shoot down alien missiles");
		al_draw_text(font[TEXT], al_map_rgb(255, 0, 0), 450, 310, ALLEGRO_ALIGN_CENTER, "before they reach your bases!");

		al_draw_text(font[HEADING], al_map_rgb(255, 0, 0), 450, 450, ALLEGRO_ALIGN_CENTER, "CONTROLS");
		al_draw_text(font[TEXT], al_map_rgb(255, 0, 0), 450, 495, ALLEGRO_ALIGN_CENTER, "Fire: Left mouse button");
		al_draw_text(font[TEXT], al_map_rgb(255, 0, 0), 450, 535, ALLEGRO_ALIGN_CENTER, "Aim: move mouse");
		al_draw_text(font[TEXT], al_map_rgb(255, 0, 0), 450, 575, ALLEGRO_ALIGN_CENTER, "Pause: escape");
		al_draw_text(font[HEADING], al_map_rgb(255, 0, 0), 450, 720, ALLEGRO_ALIGN_CENTER, "Press any key to continue");
		al_flip_display();
	}

	while (ev.type != ALLEGRO_EVENT_KEY_DOWN && ev.type != ALLEGRO_EVENT_MOUSE_BUTTON_DOWN);
}

//draw reticle
void drawCrosshair(ALLEGRO_BITMAP *imageCrosshair, Crosshair * crosshair) {
	al_draw_bitmap(imageCrosshair, crosshair->pos.x - 30, crosshair->pos.y - 25, 0);
}


//draw level info at top of screen
void drawInfo(ALLEGRO_FONT ** font, Abm * abm, Level * level) {
	al_draw_textf(font[TEXT], al_map_rgb(255, 0, 0), 15, 10, 0, "Bases: %d", level->lives);
	al_draw_textf(font[TEXT], al_map_rgb(255, 0, 0), 150, 10, 0, "Missiles: %d", level->abmLeft);
	al_draw_textf(font[TEXT], al_map_rgb(255, 0, 0), 315, 10, 0, "Score: %d", level->score);
	al_draw_textf(font[TEXT], al_map_rgb(255, 0, 0), 530, 10, 0, "Level: %d", level->round);
	al_draw_textf(font[TEXT], al_map_rgb(255, 0, 0), 655, 10, 0, "Enemy missiles: %d", level->spawnLimit - level->num_spawned);
}


//draw transition screen between levels 
void transition(ALLEGRO_FONT ** font, ALLEGRO_TIMER * timer, Abm * abm, Level * level, int numUnusedAbm) {

	al_clear_to_color(al_map_rgb(0, 0, 0));

	//award bonus points
	al_draw_text(font[HEADING], al_map_rgb(255, 0, 0), 450, 250, ALLEGRO_ALIGN_CENTER, "BONUS POINTS");
	al_draw_textf(font[TEXT], al_map_rgb(255, 0, 0), 450, 300, ALLEGRO_ALIGN_CENTER, "Bases: %d", level->lives * 100);

	if (numUnusedAbm > 0)
		al_draw_textf(font[TEXT], al_map_rgb(255, 0, 0), 450, 350, ALLEGRO_ALIGN_CENTER, "Unused missiles: %d ", numUnusedAbm * 25);

	al_draw_text(font[HEADING], al_map_rgb(255, 0, 0), 450, 500, ALLEGRO_ALIGN_CENTER, "TOTAL SCORE");  //365
	al_draw_textf(font[TEXT], al_map_rgb(255, 0, 0), 450, 545, ALLEGRO_ALIGN_CENTER, "%d", level->score); //440

	al_flip_display();
	al_rest(3);
	al_clear_to_color(al_map_rgb(0, 0, 0)); 

	//info about next level
	al_draw_textf(font[BOLD], al_map_rgb(255, 0, 0), 420, 300, 0, "Level %d", level->round);
	al_draw_textf(font[BOLD], al_map_rgb(255, 0, 0), 400, 400, 0, "Bases left: %d", level->lives);
	al_draw_textf(font[BOLD], al_map_rgb(255, 0, 0), 415, 500, 0, "%dx points", level->multiplier);

	al_flip_display();
	al_rest(3);

	al_resume_timer(timer);
}


//draw various objects
void drawObjects(Base * base, int baseCount, ALLEGRO_BITMAP * imageMissile, Abm * abm, ALLEGRO_BITMAP * imageBase,
	ALLEGRO_BITMAP * background, ALLEGRO_BITMAP * ground) {

	al_draw_bitmap(background, 0, 0, 0);	//draw spacebackground
	al_draw_bitmap(ground, -100, 750, 0);	//draw planet surface

	//draw first abm battery (left)
	if (!abm[0].launched && !abm[0].arrived)
		al_draw_bitmap(imageMissile, 0, 770, 0);
	if (!abm[1].launched && !abm[1].arrived)
		al_draw_bitmap(imageMissile, 10, 770, 0);
	if (!abm[2].launched && !abm[2].arrived)
		al_draw_bitmap(imageMissile, 20, 770, 0);
	if (!abm[3].launched && !abm[3].arrived)
		al_draw_bitmap(imageMissile, 30, 770, 0);
	if (!abm[4].launched && !abm[4].arrived)
		al_draw_bitmap(imageMissile, 40, 770, 0);

	if (!abm[5].launched && !abm[5].arrived)
		al_draw_bitmap(imageMissile, -10, 800, 0);
	if (!abm[6].launched && !abm[6].arrived)
		al_draw_bitmap(imageMissile, 0, 800, 0);
	if (!abm[7].launched && !abm[7].arrived)
		al_draw_bitmap(imageMissile, 10, 800, 0);
	if (!abm[8].launched && !abm[8].arrived)
		al_draw_bitmap(imageMissile, 20, 800, 0);
	if (!abm[9].launched && !abm[9].arrived)
		al_draw_bitmap(imageMissile, 30, 800, 0);

	//draw second abm battery (center)
	if (!abm[10].launched && !abm[10].arrived)
		al_draw_bitmap(imageMissile, 400, 770, 0);
	if (!abm[11].launched && !abm[11].arrived)
		al_draw_bitmap(imageMissile, 410, 770, 0);
	if (!abm[12].launched && !abm[12].arrived)
		al_draw_bitmap(imageMissile, 420, 770, 0);
	if (!abm[13].launched && !abm[13].arrived)
		al_draw_bitmap(imageMissile, 430, 770, 0);
	if (!abm[14].launched && !abm[14].arrived)
		al_draw_bitmap(imageMissile, 440, 770, 0);

	if (!abm[15].launched && !abm[15].arrived)
		al_draw_bitmap(imageMissile, 400, 800, 0);
	if (!abm[16].launched && !abm[16].arrived)
		al_draw_bitmap(imageMissile, 410, 800, 0);
	if (!abm[17].launched && !abm[17].arrived)
		al_draw_bitmap(imageMissile, 420, 800, 0);
	if (!abm[18].launched && !abm[18].arrived)
		al_draw_bitmap(imageMissile, 430, 800, 0);
	if (!abm[19].launched && !abm[19].arrived)
		al_draw_bitmap(imageMissile, 440, 800, 0);

	//draw third abm battery (right) 
	if (!abm[20].launched && !abm[20].arrived)
		al_draw_bitmap(imageMissile, 800, 770, 0);
	if (!abm[21].launched && !abm[21].arrived)
		al_draw_bitmap(imageMissile, 810, 770, 0);
	if (!abm[22].launched && !abm[22].arrived)
		al_draw_bitmap(imageMissile, 820, 770, 0);
	if (!abm[23].launched && !abm[23].arrived)
		al_draw_bitmap(imageMissile, 830, 770, 0);
	if (!abm[24].launched && !abm[24].arrived)
		al_draw_bitmap(imageMissile, 840, 770, 0);

	if (!abm[25].launched && !abm[25].arrived)
		al_draw_bitmap(imageMissile, 810, 800, 0);
	if (!abm[26].launched && !abm[26].arrived)
		al_draw_bitmap(imageMissile, 820, 800, 0);
	if (!abm[27].launched && !abm[27].arrived)
		al_draw_bitmap(imageMissile, 830, 800, 0);
	if (!abm[28].launched && !abm[28].arrived)
		al_draw_bitmap(imageMissile, 840, 800, 0);
	if (!abm[29].launched && !abm[29].arrived)
		al_draw_bitmap(imageMissile, 850, 800, 0);

	//draw leftmost bases
	if (!base[0].destroyed)
		al_draw_bitmap(imageBase, 90, 810, 0);

	if (!base[1].destroyed)
		al_draw_bitmap(imageBase, 190, 810, 0);

	if (!base[2].destroyed)
		al_draw_bitmap(imageBase, 290, 810, 0);


	//draw rightmost bases
	if (!base[3].destroyed)
		al_draw_bitmap(imageBase, 505, 810, 0);

	if (!base[4].destroyed)
		al_draw_bitmap(imageBase, 605, 810, 0);

	if (!base[5].destroyed)
		al_draw_bitmap(imageBase, 705, 810, 0);


}

//recursive function; generates three different colours for ABM, regular enemy missile, and cruise missile
void generateOneColor(int * theme, int i)
{
	theme[i] = rand() % NUM_COLORS;

	if (i > 0) {
		for (int j = i - 1; j >= 0; j--) {	//compare the colour index generated with previous ones
			if (theme[i] == theme[j]) {
				generateOneColor(theme, i); //re-generate the current colour index if it is a duplicate of a previously generated one 
				break;
			}
		}
	}
}

//calls another function to generate a colour index 
void generateTheme(int * theme) {
	for (int i = 0; i < COLORS_PER_THEME; i++) {
		generateOneColor(theme, i);
	}
}