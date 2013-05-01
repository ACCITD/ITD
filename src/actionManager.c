#include "actionManager.h"
#include <SDL/SDL.h>
#ifdef MAC
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "itd.h"
#include "graphics.h"

void handleMenuActions(char* mapName){}

bool handleGameActions(){
	SDL_Event e;
	bool askedForQuit = false;
	while(SDL_PollEvent(&e)) {
		/* L'utilisateur ferme la fenêtre : */
		if(e.type == SDL_QUIT) {
			askedForQuit = true;
			break;
		}
		switch(e.type) {
		case SDL_KEYDOWN:
			switch(e.key.keysym.sym){
			case 'q' : 
			case SDLK_ESCAPE : 
			askedForQuit = true;
			break;
			default : break;
			}
		break;
		default : break;
		}
	}

	return askedForQuit;
}

void clicButton (SDL_Event e){
	if (e.button.button==SDL_BUTTON_LEFT){
		/*printf(" x : %f, y : %f\n",(8.0*(e.button.x/(float)WINDOW_WIDTH)-4.0),(8.0*(e.button.y/(float)WINDOW_WIDTH)-4.0));*/
			if ((8.0*(e.button.x/(float)WINDOW_WIDTH)-4.0) >= -0.6 && 
			(8.0*(e.button.x/(float)WINDOW_WIDTH)-4.0) <= 0.6 && 
			(8.0*(e.button.y/(float)WINDOW_WIDTH)-4.0) >= -0.6 && 
			(8.0*(e.button.y/(float)WINDOW_WIDTH)-4.0) <= 0.6){
				printf("coucou\n");
			}

				  
	}

}
