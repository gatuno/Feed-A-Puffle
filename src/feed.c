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

#include "config.h"

#define FPS (1000/24)

#define SWAP(a, b, t) ((t) = (a), (a) = (b), (b) = (t))

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE !FALSE
#endif

/* Enumerar las imágenes */
enum {
	IMG_INTERFACE,
	
	IMG_ROW_1,
	IMG_ROW_2,
	IMG_ROW_3,
	IMG_ROW_4,
	
	IMG_PUFFLEO_1,
	IMG_PUFFLEO_2,
	IMG_PUFFLEO_3,
	
	NUM_IMAGES
};

const char *images_names[NUM_IMAGES] = {
	GAMEDATA_DIR "images/interface.png",
	
	GAMEDATA_DIR "images/row1.png",
	GAMEDATA_DIR "images/row2.png",
	GAMEDATA_DIR "images/row3.png",
	GAMEDATA_DIR "images/row4.png",
	
	GAMEDATA_DIR "images/puffleo1.png",
	GAMEDATA_DIR "images/puffleo2.png",
	GAMEDATA_DIR "images/puffleo3.png"
};

/* TODO: Listar aquí los automátas */

/* Codigos de salida */
enum {
	GAME_NONE = 0, /* No usado */
	GAME_CONTINUE,
	GAME_QUIT
};

/* Estructuras */
typedef struct _PuffleO {
	struct _PuffleO *next;
	struct _PuffleO *prev;
	int x, y;
	int z;
	int frame;
	int fuerzax, fuerzay;
	int scale;
	int holderay, holderaz;
	int holderby, holderbz;
	int holdercy, holdercz;
} PuffleO;

/* Prototipos de función */
int game_loop (void);
void setup (void);
SDL_Surface * set_video_mode(unsigned);
void lanzar_puffleO (int, int, int, int);
void eliminar_puffleO (PuffleO *);

/* Variables globales */
SDL_Surface * screen;
SDL_Surface * images [NUM_IMAGES];
PuffleO * first_puffleO = NULL;
PuffleO * last_puffleO = NULL;
int n_puffleOs = 1;

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
	
	int handposx2, handposx1, handposx, handposy2, handposy1, handposy; /* Para calcular los desplazamientos del mouse */
	int fuerzax, fuerzay;
	int totalpuffleos = 100;
	int launcher_x, launcher_y;
	PuffleO * this_puffleO;
	int mouseisdown = FALSE;
	int mouseisready = TRUE;
	
	SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
	SDL_GetMouseState (&handposx, &handposy);
	
	launcher_x = handposx2 = handposx1 = handposx;
	launcher_y = handposy2 = handposy1 = handposy;
	
	do {
		last_time = SDL_GetTicks ();
		
		while (SDL_PollEvent(&event) > 0) {
			switch (event.type) {
				case SDL_QUIT:
					/* Vamos a cerrar la aplicación */
					done = GAME_QUIT;
					break;
				case SDL_MOUSEBUTTONDOWN:
					/* Lanzar un puffle O */
					/* Si el mouse no entra dentro de los botones, lanzar un puffleO */
					mouseisdown = TRUE;
					break;
				case SDL_MOUSEBUTTONUP:
					mouseisdown = FALSE;
					break;
			}
		}
		/* TODO: Stamp unlocked */
		
		/* TODO: Contador / timer generador de puffles */
		
		handposy2 = handposy1;
		handposy1 = handposy;
		
		handposx2 = handposx1;
		handposx1 = handposx;
		
		SDL_GetMouseState (&handposx, &handposy);
		
		fuerzay = (handposy2 - handposy + 60) / 3;
		if (fuerzay > 40) {
			fuerzay = 40;
		} else if (fuerzay < -20) {
			fuerzay = -20;
		}
		
		fuerzax = (handposx2 - handposx) / 3;
		if (fuerzax > 30) {
			fuerzax = 30;
		} else if (fuerzax < -30) {
			fuerzax = -30;
		}
		
		/* TODO: Actualizar los valores del launcher */
		
		if (mouseisready) {
			if (mouseisdown) {
				if (totalpuffleos > 0) {
					totalpuffleos--;
				}
				mouseisready = FALSE;
				lanzar_puffleO (handposx, handposy, fuerzax, fuerzay);
			}
		}
		
		if (!mouseisdown) mouseisready = TRUE;
		
		/* Recorrer y animar todos los PuffleOs */
		this_puffleO = first_puffleO;
		while (this_puffleO != NULL) {
			printf ("Debug: fuerza X: %i\n", this_puffleO->fuerzax);
			this_puffleO->scale = 30 + this_puffleO->y / 3;
			
			if (this_puffleO->scale < 50) {
				this_puffleO->scale = 50;
			} else if (this_puffleO->scale > 180) {
				this_puffleO->scale = 180;
			}
			
			this_puffleO->x -= this_puffleO->fuerzax;
			this_puffleO->y -= this_puffleO->fuerzay;
			this_puffleO->fuerzay--;
			
			/* Recorrer los 4 agujeros de la fila superior, para darles de comer */
			if (this_puffleO->y < this_puffleO->holderay - 10 && this_puffleO->z > this_puffleO->holderaz) {
				/* Recorrer los 4 y darles de comer */
			} else if (this_puffleO->y > this_puffleO->holderay - 20 && this_puffleO->z < this_puffleO->holderaz) {
				/* Golpeo contra los puffles */
			}
			
			if (this_puffleO->z < this_puffleO->holderaz && this_puffleO->y > 430) {
				if (this_puffleO->prev != NULL) {
					this_puffleO = this_puffleO->prev;
					eliminar_puffleO (this_puffleO->next);
				} else {
					eliminar_puffleO (this_puffleO);
					this_puffleO = first_puffleO;
				}
				continue;
			}
			if (this_puffleO->z < this_puffleO->holderbz && this_puffleO->y > 288) {
				if (this_puffleO->prev != NULL) {
					this_puffleO = this_puffleO->prev;
					eliminar_puffleO (this_puffleO->next);
				} else {
					eliminar_puffleO (this_puffleO);
					this_puffleO = first_puffleO;
				}
				continue;
			}
			if (this_puffleO->z < this_puffleO->holdercz && this_puffleO->y > 198) {
				if (this_puffleO->prev != NULL) {
					this_puffleO = this_puffleO->prev;
					eliminar_puffleO (this_puffleO->next);
				} else {
					eliminar_puffleO (this_puffleO);
					this_puffleO = first_puffleO;
				}
				continue;
			}
			if (this_puffleO != NULL) this_puffleO = this_puffleO->next;
		}
		
		/* Dibujar el escenario */
		/* Row 4 tiene la menor profundidad, dibujar primero */
		/* Z = 100 */
		rect.x = -7;
		rect.y = -1;
		rect.h = images[IMG_ROW_4]->h; rect.w = images[IMG_ROW_4]->w;
		
		SDL_BlitSurface (images [IMG_ROW_4], NULL, screen, &rect);
		
		for (this_puffleO = first_puffleO; this_puffleO != NULL; this_puffleO = this_puffleO->next) {
			if (this_puffleO->z > 100 && this_puffleO->z < 300) {
				rect.x = this_puffleO->x;
				rect.y = this_puffleO->y;
				rect.h = images[this_puffleO->frame]->h; rect.w = images[this_puffleO->frame]->w;
				SDL_BlitSurface (images[this_puffleO->frame], NULL, screen, &rect);
			}
		}
		
		/* Z = 300 */
		rect.x = -5;
		rect.y = 179;
		rect.h = images[IMG_ROW_3]->h; rect.w = images[IMG_ROW_3]->w;
		
		SDL_BlitSurface (images [IMG_ROW_3], NULL, screen, &rect);
		
		for (this_puffleO = first_puffleO; this_puffleO != NULL; this_puffleO = this_puffleO->next) {
			if (this_puffleO->z > 300 && this_puffleO->z < 500) {
				rect.x = this_puffleO->x;
				rect.y = this_puffleO->y;
				rect.h = images[this_puffleO->frame]->h; rect.w = images[this_puffleO->frame]->w;
				SDL_BlitSurface (images[this_puffleO->frame], NULL, screen, &rect);
			}
		}
		
		/* Z = 500 */
		rect.x = -5;
		rect.y = 262;
		rect.h = images[IMG_ROW_2]->h; rect.w = images[IMG_ROW_2]->w;
		
		SDL_BlitSurface (images [IMG_ROW_2], NULL, screen, &rect);
		
		for (this_puffleO = first_puffleO; this_puffleO != NULL; this_puffleO = this_puffleO->next) {
			if (this_puffleO->z > 500 && this_puffleO->z < 700) {
				rect.x = this_puffleO->x;
				rect.y = this_puffleO->y;
				rect.h = images[this_puffleO->frame]->h; rect.w = images[this_puffleO->frame]->w;
				SDL_BlitSurface (images[this_puffleO->frame], NULL, screen, &rect);
			}
		}
		
		/* Row 1 tien la mayor profundidad, debe ir encima de todo */
		/* Z = 700 */
		rect.x = -5;
		rect.y = 398;
		rect.h = images[IMG_ROW_1]->h; rect.w = images[IMG_ROW_1]->w;
		
		SDL_BlitSurface (images [IMG_ROW_1], NULL, screen, &rect);
		
		for (this_puffleO = first_puffleO; this_puffleO != NULL; this_puffleO = this_puffleO->next) {
			if (this_puffleO->z > 700) {
				rect.x = this_puffleO->x;
				rect.y = this_puffleO->y;
				rect.h = images[this_puffleO->frame]->h; rect.w = images[this_puffleO->frame]->w;
				SDL_BlitSurface (images[this_puffleO->frame], NULL, screen, &rect);
			}
		}
		
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

void lanzar_puffleO (int x, int y, int fuerzax, int fuerzay) {
	PuffleO *nuevo;
	int t;
	
	nuevo = (PuffleO *) malloc (sizeof (PuffleO));
	
	/* Inicializar este puffle O */
	nuevo->z = 700 + n_puffleOs;
	nuevo->x = x;
	nuevo->y = y;
	nuevo->frame = IMG_PUFFLEO_1 + (int) (3.0 * rand () / (RAND_MAX + 1.0));
	printf ("Debug: frame: %i\n", nuevo->frame);
	printf ("Debug: fuerza X: %i\n", fuerzax);
	nuevo->frame = IMG_PUFFLEO_1;
	nuevo->fuerzax = fuerzax;
	nuevo->fuerzay = fuerzay;
	
	nuevo->holderaz = 500 + n_puffleOs;
	nuevo->holderbz = 300 + n_puffleOs;
	nuevo->holdercz = 100 + n_puffleOs;
	
	nuevo->holderay = 380;
	nuevo->holderby = 245;
	nuevo->holdercy = 172;
	
	if (nuevo->y < nuevo->holderay && nuevo->z > nuevo->holderaz) {
		SWAP (nuevo->z, nuevo->holderaz, t);
	} else if (nuevo->y > nuevo->holderay && nuevo->z < nuevo->holderaz) {
		SWAP (nuevo->z, nuevo->holderaz, t);
	}
	
	if (nuevo->y < nuevo->holderby && nuevo->z > nuevo->holderbz) {
		SWAP (nuevo->z, nuevo->holderbz, t);
	} else if (nuevo->y > nuevo->holderby && nuevo->z < nuevo->holderbz) {
		SWAP (nuevo->z, nuevo->holderbz, t);
	}
	
	if (nuevo->y < nuevo->holdercy && nuevo->z > nuevo->holdercz) {
		SWAP (nuevo->z, nuevo->holdercz, t);
	} else if (nuevo->y > nuevo->holdercy && nuevo->z < nuevo->holdercz) {
		SWAP (nuevo->z, nuevo->holdercz, t);
	}
	
	n_puffleOs++;
	if (n_puffleOs > 40) n_puffleOs = 1;
	
	/* Ahora los campos de la lista doble ligada */
	nuevo->next = NULL;
	nuevo->prev = last_puffleO;
	
	if (last_puffleO == NULL) {
		first_puffleO = last_puffleO = nuevo;
	} else {
		last_puffleO->next = nuevo;
		last_puffleO = nuevo;
	}
}

void eliminar_puffleO (PuffleO *p) {
	if (p == NULL) return;
	
	if (p->prev == NULL) { /* El primero de la lista */
		first_puffleO = p->next;
	} else {
		p->prev->next = p->next;
	}
	
	if (p->next == NULL) {
		last_puffleO = p->prev;
	} else {
		p->next->prev = p->prev;
	}
	
	free (p);
}
