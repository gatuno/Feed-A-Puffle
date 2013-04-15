/*
 * feed.c
 * This file is part of Feed a Puffle
 *
 * Copyright (C) 2013 - Félix Arreola Rodríguez
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* ----------------
 * LEGAL NOTICE
 * ----------------
 *
 * This game is NOT related to Club Penguin in any way. Also,
 * this game is not intended to infringe copyrights, the graphics and
 * sounds used are Copyright of Disney.
 *
 * The new SDL code is written by Gatuno, and is released under
 * the term of the GNU General Public License.
 */

#include <stdlib.h>
#include <stdio.h>

#include <SDL.h>
#include <SDL_image.h>

/*#include "config.h"*/

#define FPS (1000/24)

/* Enumerar las imágenes */
enum {
	IMG_INTERFACE,
	
	IMG_ROW_1,
	IMG_ROW_2,
	IMG_ROW_3,
	IMG_ROW_4,
	
	NUM_IMAGES
};

#define GAMEDATA_DIR "../data/"

const char *images_names[NUM_IMAGES] = {
	GAMEDATA_DIR "images/interface.png",
	
	GAMEDATA_DIR "images/row1.png",
	GAMEDATA_DIR "images/row2.png",
	GAMEDATA_DIR "images/row3.png",
	GAMEDATA_DIR "images/row4.png"
};

/* TODO: Listar aquí los automátas */

/* Codigos de salida */
enum {
	GAME_NONE = 0, /* No usado */
	GAME_CONTINUE,
	GAME_QUIT
};

/* Estructuras */

/* Prototipos de función */
int game_loop (void);
void setup (void);
SDL_Surface * set_video_mode(unsigned flags);

/* Variables globales */
SDL_Surface * screen;
SDL_Surface * images [NUM_IMAGES];

int main (int argc, char *argv[]) {
	
	setup ();
	
	do {
		if (game_loop () == GAME_QUIT) break;
	} while (1 == 0);
	
	SDL_Quit ();
	return EXIT_SUCCESS;
}

int game_loop (void) {
	int done = 0;
	SDL_Event event;
	SDLKey key;
	Uint32 last_time, now_time;
	SDL_Rect rect;
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
			}
		}
		
		/* Dibujar el escenario */
		rect.x = -6;
		rect.y = -1;
		rect.h = images[IMG_ROW_4]->h; rect.w = images[IMG_ROW_4]->w;
		
		SDL_BlitSurface (images [IMG_ROW_4], NULL, screen, &rect);
		
		rect.x = -5;
		rect.y = 179;
		rect.h = images[IMG_ROW_3]->h; rect.w = images[IMG_ROW_3]->w;
		
		SDL_BlitSurface (images [IMG_ROW_3], NULL, screen, &rect);
		
		rect.x = -5;
		rect.y = 262;
		rect.h = images[IMG_ROW_2]->h; rect.w = images[IMG_ROW_2]->w;
		
		SDL_BlitSurface (images [IMG_ROW_2], NULL, screen, &rect);
		
		rect.x = -5;
		rect.y = 398;
		rect.h = images[IMG_ROW_1]->h; rect.w = images[IMG_ROW_1]->w;
		
		SDL_BlitSurface (images [IMG_ROW_1], NULL, screen, &rect);
		
		SDL_Flip (screen);
		
		now_time = SDL_GetTicks ();
		if (now_time < last_time + FPS) SDL_Delay(last_time + FPS - now_time);
		
	} while (!done);
	
	return done;
}

/* Set video mode: */
/* Mattias Engdegard <f91-men@nada.kth.se> */
SDL_Surface * set_video_mode (unsigned flags) {
	/* Prefer 16bpp, but also prefer native modes to emulated 16bpp. */

	int depth;

	depth = SDL_VideoModeOK (760, 480, 16, flags);
	return depth ? SDL_SetVideoMode (760, 480, depth, flags) : NULL;
}

void setup (void) {
	SDL_Surface * image;
	int g;
	
	/* Inicializar el Video SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf (stderr,
			"Error: Can't initialize the video subsystem\n"
			"The error returned by SDL is:\n"
			"%s\n", SDL_GetError());
		exit (1);
	}
	
	/* Crear la pantalla de dibujado */
	screen = set_video_mode (0);
	
	if (screen == NULL) {
		fprintf (stderr,
			"Error: Can't setup 760x480 video mode.\n"
			"The error returned by SDL is:\n"
			"%s\n", SDL_GetError());
		exit (1);
	}
	
	for (g = 0; g < NUM_IMAGES; g++) {
		image = IMG_Load (images_names[g]);
		
		if (image == NULL) {
			fprintf (stderr,
				"Failed to load data file:\n"
				"%s\n"
				"The error returned by SDL is:\n"
				"%s\n", images_names[g], SDL_GetError());
			SDL_Quit ();
			exit (1);
		}
		
		images[g] = image;
		/* TODO: Mostrar la carga de porcentaje */
	}
	
	srand (SDL_GetTicks ());
}
