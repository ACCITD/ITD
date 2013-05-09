#include "actionManager.h"
#include <SDL/SDL.h>
#ifdef MAC
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <math.h>
#include "graphics.h"

//TODO
bool handleMenuActions(char* mapName){ 
	SDL_Event e;
	bool askedForQuit = false;
	while(SDL_PollEvent(&e) && !askedForQuit) {
		/* L'utilisateur ferme la fenêtre : */
		if(e.type == SDL_QUIT) {
			askedForQuit = true;
		}
		else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
			switch(e.key.keysym.sym){
			case 'q' : 
			case SDLK_ESCAPE : 
			askedForQuit = true;
			break;
			default : break;
			}
		}
		
	}
		
	return askedForQuit;
}

//Variables globales pour la gestion de la caméra
bool isMouseExtremRight = false;
bool isMouseExtremLeft = false;
bool isMouseExtremTop = false;
bool isMouseExtremBottom = false;

bool handleGameActions(World* world, Interface* interface){
	//Gestion de l'évolution de l'argent
	static int lastMoney = 0;
	if(lastMoney != world->money){
		updateMoneyTexture(interface, world->money);
		lastMoney = world->money;
	}
	
	SDL_Event e;
	bool askedForQuit = false;
	while(SDL_PollEvent(&e) && !askedForQuit) {
		/* L'utilisateur ferme la fenêtre : */
		if(e.type == SDL_QUIT) {
			askedForQuit = true;
		}
		
		else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP){
			askedForQuit = handleGameKeyboard(&(e.key), world, interface);
		}	
		
		else askedForQuit = handleGameMouse(&e, world, interface);
	}
	//Gestion caméra
	
	if(isMouseExtremBottom) world->cameraPosition.y += 2;
	else if(isMouseExtremTop) world->cameraPosition.y -= 2;
	if(isMouseExtremLeft) world->cameraPosition.x += 2;
	else if(isMouseExtremRight) world->cameraPosition.x -= 2;

	return askedForQuit;
}

bool handleGameKeyboard(const SDL_KeyboardEvent* e, World* world, Interface* interface){
	bool askedForQuit = false;
	
	switch((*e).keysym.sym){
		case 'q' : 
		case SDLK_ESCAPE : 
		askedForQuit = true;
		break;
		case ' ' : 
			if((*e).type != SDL_KEYDOWN) break;//Sinon met pause et l'enlève dès qu'on retire espace
			if(world->paused){
				world->paused = false;
				interface->currentAction = NO_ACTION;
			}
			else{
				world->paused = true;
				interface->currentAction = PAUSE_GAME;
			}
		break;
		default : break;
	}
	
	return askedForQuit;
}

//TODO
bool handleGameMouse(const SDL_Event* e, World* world, Interface* interface){
	if(e->type != SDL_MOUSEMOTION && e->type != SDL_MOUSEBUTTONDOWN && e->type != SDL_MOUSEBUTTONUP)
		return false;
	if(world == NULL || interface == NULL) return false;
	//Déplacement de la caméra si on est à une extrémité de la fenêtre
	if(e->type == SDL_MOUSEMOTION && SDL_GetTicks() > 2000){
		if(fabs((int)e->motion.x - (int)WINDOW_WIDTH) < 2.* WINDOW_WIDTH / 100.0){
			isMouseExtremRight = true;
			isMouseExtremLeft = false;
		}
		else if(fabs(e->motion.x - 0.0) < 2.*WINDOW_WIDTH / 100.0){
			isMouseExtremLeft = true;
			isMouseExtremRight = false;
		}
		else {
			isMouseExtremLeft = false;
			isMouseExtremRight = false;
		}
		if(fabs((int)e->motion.y - (int)WINDOW_HEIGHT) < 2.*WINDOW_HEIGHT / 100.0){
			isMouseExtremBottom = true;
			isMouseExtremTop = false;
		}
		else if(fabs(e->motion.y - 0.) < 2.* WINDOW_HEIGHT / 100.0){
			isMouseExtremTop = true;
			isMouseExtremBottom = false;
		}	
		else{
			isMouseExtremBottom = false;
			isMouseExtremTop = false;
		}
	}
	else if(e->type == SDL_MOUSEBUTTONDOWN){
		Tower* pointedTower = NULL;
		Action action = detectAction(e->button.x, e->button.y, world, interface, &pointedTower);
		if(action == QUIT_GAME) return true;
		if(action == CLICK_ON_TOWER) {
			if(pointedTower == NULL) fprintf(stderr, "Impossible d'obtenir les infos sur la tour sélectionnée.\n");
			else {
				updateInfoTexture(interface, pointedTower->power, pointedTower->rate, pointedTower->range);
				printf("Tower power : %d\n", pointedTower->power);
			}
		}
		else if(action == CLICK_ON_MAP){
		
			switch(interface->currentAction){
			case PUT_GUN : 
				addTowerOnMap(world, e->button.x, e->button.y, GUN);
				interface->currentAction = NO_ACTION;
			
			break;
			case PUT_HYBRID :
				addTowerOnMap(world, e->button.x, e->button.y, HYBRID);
				interface->currentAction = NO_ACTION;
			break;
			case PUT_LASER :
				addTowerOnMap(world, e->button.x, e->button.y, LASER);
				interface->currentAction = NO_ACTION;
			break;
			case PUT_ROCKET :
				addTowerOnMap(world, e->button.x, e->button.y, ROCKET);
				interface->currentAction = NO_ACTION;
			break;
			case NO_ACTION : 
			break;
			case QUIT_GAME :
			break;
			case CLICK_ON_MAP:
			break;
			case PAUSE_GAME : 
			default :
			break;
			}
		}
		else{
			//Pour l'instant quand le jeu est en pause on ne peut que quitter (à modifier)
			if(interface->currentAction != PAUSE_GAME){
				interface->currentAction = action;
			}
		}
		
	}
	
	return false;
}

Action detectAction(Uint16 x, Uint16 y, World* world, Interface* interface, Tower** pointedTower){
	bool actionDetected = false;
	
	//Inutile de tester tous les boutons si on n'est pas sur l'interface
	if(isMouseOnInterface(x,y, interface)) {
		goToHeadList(interface->lstButtons);
		Button* cur = NULL;
		while( !actionDetected && (cur = (Button*) nextData(interface->lstButtons)) != NULL){
			actionDetected = isMouseOnButton(cur, x, y);	
		}
		if(cur == NULL) return NO_ACTION;
		return cur->action;
	}
	else{
	//On check le click sur une tour
		Tower* cur = NULL;
		goToHeadList(world->towersList);
		while(!actionDetected && (cur = (Tower*) nextData(world->towersList)) != NULL ){
			actionDetected = isMouseOnTower(cur, world->cameraPosition, x, y);
		}
		if(actionDetected){
			*pointedTower = cur;
			return CLICK_ON_TOWER;
		}
		else return CLICK_ON_MAP;
	}
}

bool isMouseOnInterface(Uint16 x, Uint16 y, Interface* interface){
	if(interface == NULL) return false;
	bool inside = false;
	Point3D oglMouse = sdlToOpenGL(PointXYZ(x,y,0.0));
	inside = oglMouse.x >= (interface->position.x - interface->width / 2.0) && oglMouse.x <= interface->position.x + (interface->width / 2.0);
	if(!inside) return inside;
	inside = oglMouse.y >= interface->position.y - (interface->height / 2.0)  && oglMouse.y <= interface->position.y + interface->height / 2.0;
	return inside;
}

bool isMouseOnButton(Button* button, Uint16 x, Uint16 y){
	if(button == NULL) return false;
	bool inside = false;
	Point3D oglMouse = sdlToOpenGL(PointXYZ(x,y,0.0));
	inside = oglMouse.x >= button->position.x - button->width / 2.0 && oglMouse.x <= button->position.x + button->width / 2.0;
	if(!inside) return inside;
	inside = oglMouse.y >= button->position.y - button->height / 2.0 && oglMouse.y <= button->position.y + button->height;
	return inside;
}

//ATTENTION : la caméra est à prendre en compte dans la détection d'un clic sur une tour
bool isMouseOnTower(Tower* tower, Point3D cameraPosition, Uint16 x, Uint16 y){
	if(tower == NULL) return false;
	bool inside = false;
	Point3D oglMouse = sdlToOpenGL(PointXYZ(x,y,0.0));
	Point3D oglTower = sdlToOpenGL(tower->position);
	//Il faut appliquer la translation de la caméra à la position
	//"sdl to opengl" de la tour pour avoir sa position dans le monde
	oglTower.x += cameraPosition.x;
	oglTower.y += cameraPosition.y;
	oglTower.z += cameraPosition.z;
	
	inside = oglMouse.x >= oglTower.x - TOWER_WIDTH_PX / 2.0 && oglMouse.x <= oglTower.x + TOWER_HEIGHT_PX / 2.0;
	if(!inside) return inside;
	inside = oglMouse.y >= oglTower.y - TOWER_HEIGHT_PX / 2.0 && oglMouse.y <= oglTower.y + TOWER_HEIGHT_PX;
	return inside;
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
