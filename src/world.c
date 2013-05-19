#include "world.h"
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <math.h>
#include "geometry.h"
#include "point3D.h"

//-------------- FONCTIONS PUBLIQUES --------------------------
World initWorld(const char* pathToItdFile){
	World newWorld;
	
	newWorld.worldTime = SDL_GetTicks();
	newWorld.currentMonstersWave = 0;
	newWorld.isBetweenWaves = true;
	newWorld.nbTurnsWaiting = 0;
	newWorld.nbMonstersAlive = 0;//Pas de monstres au départ
	newWorld.money = 500;
	newWorld.paused = false;
	newWorld.gameLoosed = false;
	newWorld.gameWinned = false;
	
	newWorld.cameraPosition = PointXYZ(0.,0.,0.);
	newWorld.towersList = createEmptyList();
	if(newWorld.towersList == NULL){
		perror("Erreur fatale, impossible d'allouer l'espace mémoire nécessaire.\n");
		exit(-1);
	}
	
	newWorld.map = initMap();
	bool loadSucceed = loadMap(&(newWorld.map), pathToItdFile);
	if(!loadSucceed){
		fprintf(stderr, "Erreur fatale : impossible de charger la carte %s.\n", pathToItdFile);
		cleanWorld(&newWorld);
		exit(EXIT_FAILURE);
	}
	
	return newWorld;
}

void startWorld(World* world){
	if(world == NULL) return;
	world->worldTime = SDL_GetTicks();
}

void startNewMonsterWave(World* world){
	if(world == NULL) return;
	world->currentMonstersWave++;
	printf("Starts monster wave n°%d\n", world->currentMonstersWave);
	
	int i = 0;
	Point3D startPoint = getStartPoint(&(world->map));
	Vector3D direction = Vector(startPoint, PointXYZ(startPoint.x, startPoint.y, -1.0));
	//On fait démarrer les monstres "à la queuleuleu" en dehors de la map (en z)
	//Avec comme destination le point de départ de leur chemin.
	for(i = 0; i < MONSTERS_PER_WAVE; ++i){
		world->monsters[i] = createMonster(world->currentMonstersWave, i);
		world->monsters[i].destination = startPoint;
		//On ménage un espace entre les monstres, le *2 a été rajouté après qu'on les fasse arrivé par l'axe z, 
		//en effet au moment où le monstre arrive sur la map il lui faut à nouveau s'éloigner en x et en y du monstre qui le suit !
		world->monsters[i].position = PointPlusVector(startPoint, MultVector(direction, (MONSTER_WIDTH_PX+SPACE_BETWEEN_MONSTERS_PX*world->monsters[i].speed*3)*(i+1)*2));
		//--- VERY IMPORTANT SINON BUG DE DEPLACEMENT ==> On veut des valeurs entières qui correspondent à un pixel
		world->monsters[i].position.x = floor(world->monsters[i].position.x);
		world->monsters[i].position.y = floor(world->monsters[i].position.y);
		world->monsters[i].position.z = floor(world->monsters[i].position.z);
		world->monsters[i].direction = Vector(world->monsters[i].position, startPoint);
		world->monsters[i].realPosition = world->monsters[i].position;
	}
	
	world->nbMonstersAlive = MONSTERS_PER_WAVE;
}

bool worldNewStep(World* world){
	if(world == NULL) return false;
	int i = 0;
	bool isGameFinished = false;
	Uint32 now = SDL_GetTicks();
	//Tps écoulé depuis le dernier tour de jeu permet de savoir combien de tour jouer cette fois
	int turnsRemaining = (now - world->worldTime) / ((int)TIMESTEP_MILLISECONDS);
	if(turnsRemaining > 0) world->worldTime = SDL_GetTicks();
	//Si le jeu est en pause on ne fait rien !
	while(i < turnsRemaining && !isGameFinished && !world->paused){
		isGameFinished = doTurn(world);
		i++;
	}
	
	return isGameFinished;
}

//Les coordonnées doivent être en répère itd (map) !!
bool canIPutATowerHere(World* world, Point3D oglPosition, Point3D itdPosition){
	if(world == NULL) return false;
	int posX = itdPosition.x, posY = itdPosition.y;

	//Si le centre est en dehors des limites de la carte, c'est mort
	if(posX < 0.0 || posX >= world->map.width) return false;
	if(posY < 0.0 || posY >= world->map.height) return false;
	
	//Si le centre n'est pas sur une zone constructible, c'est mort
	if(! world->map.tabXYConstruct[posX][posY]) return false;
	
	//On teste maintenant les quatres coins en coordonnées ITD
	Point3D tower_top_right = PointXYZ(posX + TOWER_WIDTH_PX/2, posY + TOWER_HEIGHT_PX/2, 0.0);;
	Point3D tower_top_left = PointXYZ(posX + TOWER_WIDTH_PX/2, posY - TOWER_HEIGHT_PX/2, 0.0);
	Point3D tower_bot_right = PointXYZ(posX - TOWER_WIDTH_PX/2, posY + TOWER_HEIGHT_PX/2, 0.0);
	Point3D tower_bot_left = PointXYZ(posX - TOWER_WIDTH_PX/2,posY - TOWER_HEIGHT_PX/2, 0.0);
	
	//Si un des deux points de la diagonale est en dehors de la map c'est mort
	if(tower_top_right.x >= world->map.width || tower_top_right.y < 0.0) return false;
	if(tower_bot_left.x < 0.0 || tower_bot_left.y >= world->map.height) return false;
	
	//On test maintenant si les quatre points sont sur une zone constructible
	if( ! world->map.tabXYConstruct[(int)floor(tower_top_right.x)][(int)floor(tower_top_right.y)]) return false;
	if( ! world->map.tabXYConstruct[(int)floor(tower_top_left.x)][(int)floor(tower_top_left.y)]) return false;
	if( ! world->map.tabXYConstruct[(int)floor(tower_bot_right.x)][(int)floor(tower_bot_right.y)]) return false;
	if( ! world->map.tabXYConstruct[(int)floor(tower_bot_left.x)][(int)floor(tower_bot_left.y)]) return false;
					
	/* Tester s'il n'y a pas déjà une tour */
	Tower* cur = NULL;
	goToHeadList(world->towersList);
	while( (cur = (Tower*) nextData(world->towersList)) != NULL ){
		bool xOverlap = false, yOverlap = false;
		if( oglPosition.x > cur->position.x - TOWER_WIDTH_PX &&  oglPosition.x < cur->position.x+ TOWER_WIDTH_PX)
			xOverlap = true;
		if( oglPosition.y < cur->position.y + TOWER_HEIGHT_PX && oglPosition.y > cur->position.y - TOWER_WIDTH_PX)
			yOverlap = true;
		if(xOverlap && yOverlap)return false;
		
	}
	
	/* Tester si le chemin ne passe pas par là */
	tower_top_right = PointXYZ(oglPosition.x + TOWER_WIDTH_PX/2, oglPosition.y + TOWER_HEIGHT_PX/2, 0.0);;
	tower_top_left = PointXYZ(oglPosition.x + TOWER_WIDTH_PX/2, oglPosition.y - TOWER_HEIGHT_PX/2, 0.0);
	tower_bot_right = PointXYZ(oglPosition.x - TOWER_WIDTH_PX/2, oglPosition.y + TOWER_HEIGHT_PX/2, 0.0);
	tower_bot_left = PointXYZ(oglPosition.x - TOWER_WIDTH_PX/2,oglPosition.y - TOWER_HEIGHT_PX/2, 0.0);
	goToHeadList(world->map.pathNodeList);
	Point3D* curNode1 = nextData(world->map.pathNodeList);
	Point3D* curNode2 = NULL;
	while( (curNode2 = nextData(world->map.pathNodeList) ) != NULL){
		if(segment2segment(*curNode1, *curNode2, tower_top_left, tower_bot_right)) return false;
		if(segment2segment(*curNode1, *curNode2, tower_top_right, tower_bot_left)) return false;
		curNode1 = curNode2;
	}

	return true;
}

bool addTowerOnMap(World* world, int posX, int posY, TowerType type){
	if(world == NULL || world->towersList == NULL) return false;
//----Si la zone où l'on veut construire la tour est non constructible, on annule l'action.---
	
	//On annule la translation de la caméra pour obtenir la position OpenGL "réelle"
	Point3D oglRealPosition = sdlToOpenGL(PointXYZ(posX, posY, 0.0));
	oglRealPosition.x -= world->cameraPosition.x;
	oglRealPosition.y -= world->cameraPosition.y;
	Point3D itdPosition = openGLToItd(world->map.width, world->map.height, oglRealPosition);
	if(canIPutATowerHere(world, oglRealPosition, itdPosition) == false) return false;
	
	//L'erreur d'allocation est gérée plus bas
	//Le programme s'arrête si le malloc a échoué 
	Tower* newTower = createTower(type, world->map);
	
	//Si pas assez d'argent pour construire la tour, on annule !
	if(world->money < newTower->cost){
		free(newTower);
		return false;
	}
	
	//La "caméra" fausse les coordonnées de la tour il faut donc l'annuler
	//Point3D towerPosition = PointXYZ(posX - world->cameraPosition.x,posY + world->cameraPosition.y,0);
	//newTower->position = towerPosition;
	newTower->position = oglRealPosition;
	insertBottomCell(world->towersList, newTower);
	
	//La tour a un coût !
	world->money -= newTower->cost;
	return true;
}


void cleanWorld(World* world){
	if(world == NULL) return;
	cleanMap(&(world->map));
	if(world->towersList != NULL)
		freeListComplete(world->towersList);
	world->towersList = NULL;
}

//---------------------- FONCTIONS PRIVEES ---------------------
bool doTurn(World* world){
	if(world == NULL) return false;
	
	bool isGameFinished = false;
	
	if(world->gameLoosed || world->gameWinned) return true;
	
	//Phase d'attente entre deux vague de monstres ? On ne fait rien
	if(world->isBetweenWaves){
		world->nbTurnsWaiting++;
		if(world->nbTurnsWaiting >= NB_TURNS_BETWEEN_WAVES){
			startNewMonsterWave(world);
			world->isBetweenWaves = false;
			world->nbTurnsWaiting = 0;
		}
		return isGameFinished;
	}
	
	moveMonsters(world->monsters, world->map.pathNodeList);
	//Tuer des monstres ça fait gagner des sousous !
	world->money += towersShoot(world->towersList, world->monsters);
	
	//Les monstres sont-ils tous morts ? 
	//Un monstre a-t-il atteint l'arrivée ?
	//Tous les monstres sont morts et c'était la dernier vague => Le jeu est fini ?
	int i = 0, cptMonstersAlive = 0;
	Point3D endPoint = getEndPoint(&(world->map));
	while(i < MONSTERS_PER_WAVE && !isGameFinished){
		isGameFinished = arePointsEquals(world->monsters[i].position, endPoint);
		if(world->monsters[i].life > 0) cptMonstersAlive++;
		if(isGameFinished){
			printf("Un monstre a atteint la sortie, vous avez perdu !\n");
			world->gameLoosed = true;
		}
		++i;
	}
	world->nbMonstersAlive = cptMonstersAlive;
	if(world->nbMonstersAlive <= 0){
		if(world->currentMonstersWave >= NB_TOTAL_WAVES){
			isGameFinished = true;
			printf("Vous avez tué tous les monstres, vous avez gagné !\n");
			world->gameWinned = true;
		}
		else world->isBetweenWaves = true;
		goToHeadList(world->towersList);

	}
	return isGameFinished;
}

//Note : la variable monster->speed est égale au nombre de tours nécessaires au monstre pour
//avancer d'un pixel.
void moveMonsters(Monster* monsters, List* pathNodeList){
	if(monsters == NULL) return;
		
	int i = 0;
	
	for(i = 0; i < MONSTERS_PER_WAVE; ++i){
		//Inutile de faire bouger un monstre mort !
		if(monsters[i].life <= 0) continue;
		monsters[i].nbTurnsSinceLastMove++;
		
		moveMonster(&(monsters[i]));
		
		//Si on est sur un pathnode, on change de pathnode de destination

		if(arePointsEquals(monsters[i].position, monsters[i].destination)){
			monsters[i].destination = nextNode(pathNodeList, monsters[i].destination);
			monsters[i].direction = Vector(monsters[i].position, monsters[i].destination);
		}	
	}
	
}

int towersShoot(List* towersList, Monster* monsters){
	if(towersList == NULL || monsters == NULL) return 0;
	int moneyGained = 0;
	Tower* curTower = NULL;	
	goToHeadList(towersList);
	
	//Pour chaque tour on regarde si elle peut tirer sur un monstre
	while( (curTower = (Tower*) nextData(towersList)) != NULL){
		moneyGained += towerShoots(curTower, monsters);
	}
	return moneyGained;
}

int towerShoots(Tower* tower, Monster* monsters){
	if(tower == NULL || monsters == NULL) return 0;
	tower->nbTurnsSinceLastShoot++;
	int moneyGained = 0;
		//On supprime à chaque fois la liste des cibles de la tour
	freeListComplete(tower->whereIShoot);
	tower->whereIShoot = createEmptyList();	
	if(tower->rate > tower->nbTurnsSinceLastShoot){
		return 0;
	} 
	bool towerCanShoot = true;
	int i = 0; int lifeLosed = 0;
	
	while(towerCanShoot && i < MONSTERS_PER_WAVE){
		//Si le monstre a un z différent de 0 c'est qu'il n'est pas encore apparu sur la carte
		if(monsters[i].life > 0 && monsters[i].position.z >= 0.0
		&& Norm(Vector(monsters[i].position, tower->position)) <= tower->range){
			switch(tower->type){
				case ROCKET : lifeLosed = tower->power - monsters[i].rocketResistance;
				break;
				case GUN : lifeLosed = tower->power - monsters[i].gunResistance;
				break;
				case LASER : lifeLosed = tower->power - monsters[i].laserResistance;
				break;
				case HYBRID : lifeLosed = tower->power - monsters[i].hybridResistance;
				break;
				default : lifeLosed = tower->power;
				break;
			}
			monsters[i].life -= lifeLosed;
			//Argent gagné si monstre tué
			if(monsters[i].life <= 0){
				moneyGained += monsters[i].money;
			}
			towerCanShoot = tower->type == GUN;//Seul les GUN peuvent tirer sur tous les monstres en même temps
			tower->nbTurnsSinceLastShoot = 0;
			Point3D* target = (Point3D*) malloc(sizeof(Point3D));
			*target = monsters[i].position;
			if(target == NULL){
				fprintf(stderr, "Impossible d'allouer la mémoire nécessaire, le programme va quitter.\n");
				exit(EXIT_FAILURE);
			}
			insertBottomCell(tower->whereIShoot, target);
		}
		
		i++;
	}
	return moneyGained;
}


