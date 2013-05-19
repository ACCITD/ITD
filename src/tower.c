#include "tower.h"
#include <stdio.h>

/* les données sont pour les fichier ITD1, elles sont modifiées dans loadMap pour les ITD2 */
TowerStat* createEmptyTowerStat(){
	TowerStat* towerstat = (TowerStat*) malloc(sizeof(TowerStat));
	if(towerstat == NULL) return NULL;
	towerstat->powerR=100;
	towerstat->rateR=100;
	towerstat->rangeR=180;
	towerstat->costR=190;
	towerstat->powerL=60;
	towerstat->rateL=40;
	towerstat->rangeL=120;
	towerstat->costL=150;
	towerstat->powerM=40;
	towerstat->rateM=50;
	towerstat->rangeM=150;
	towerstat->costM=80;
	towerstat->powerH=40;
	towerstat->rateH=100;
	towerstat->rangeH=280;
	towerstat->costH=250;
	
	return towerstat;
}

Tower* createTower(TowerType type,Map map){
	switch(type){
	case ROCKET : return createRocketTower(map);
	break;
	case LASER : return createLaserTower(map);
	break;
	case GUN : return createGunTower(map);
	break;
	case HYBRID : return createHybridTower(map);
	break;
	default : return createRocketTower(map);
	}
}

//TODO
Tower* createRocketTower(Map map){
	Tower* newTower = (Tower*) malloc(sizeof(Tower));
	if(newTower == NULL){
		fprintf(stderr, "Erreur fatale : impossible d'allouer la mémoire nécessaire pour créer une tour.\n");
		exit(-1);
	}
	newTower->type = ROCKET;
	newTower->nbTurnsSinceLastShoot = 0;

	newTower->power = map.towerdatas->powerR;
	newTower->rate = map.towerdatas->rateR;
	newTower->range = map.towerdatas->rangeR;
	newTower->cost = map.towerdatas->costR;
	
	newTower->whereIShoot = createEmptyList();
	newTower->position = PointXYZ(-1,-1,0);
	return newTower;
}
//TODO
Tower* createLaserTower(Map map){
	Tower* newTower = (Tower*) malloc(sizeof(Tower));
	if(newTower == NULL){
		fprintf(stderr, "Erreur fatale : impossible d'allouer la mémoire nécessaire pour créer une tour.\n");
		exit(-1);
	}
	newTower->type = LASER;
	newTower->nbTurnsSinceLastShoot = 0;
	newTower->power = map.towerdatas->powerL;
	newTower->rate = map.towerdatas->rateL;
	newTower->range = map.towerdatas->rangeL;
	newTower->cost = map.towerdatas->costL;
	newTower->whereIShoot = createEmptyList();
	newTower->position = PointXYZ(-1,-1,0);
	return newTower;
}
//TODO
Tower* createGunTower(Map map){
	Tower* newTower = (Tower*) malloc(sizeof(Tower));
	if(newTower == NULL){
		fprintf(stderr, "Erreur fatale : impossible d'allouer la mémoire nécessaire pour créer une tour.\n");
		exit(-1);
	}
	newTower->type = GUN;
	newTower->nbTurnsSinceLastShoot = 0;
	newTower->power = map.towerdatas->powerM;
	newTower->rate = map.towerdatas->rateM;
	newTower->range = map.towerdatas->rangeM;
	newTower->cost = map.towerdatas->costM;
	newTower->whereIShoot = createEmptyList();
	newTower->position = PointXYZ(-1,-1,0);
	return newTower;
}
//TODO
Tower* createHybridTower(Map map){
	Tower* newTower = (Tower*) malloc(sizeof(Tower));
	if(newTower == NULL){
		fprintf(stderr, "Erreur fatale : impossible d'allouer la mémoire nécessaire pour créer une tour.\n");
		exit(-1);
	}
	newTower->type = HYBRID;
	newTower->nbTurnsSinceLastShoot = 0;
	newTower->power = map.towerdatas->powerH;
	newTower->rate = map.towerdatas->rateH;
	newTower->range = map.towerdatas->rangeH;
	newTower->cost = map.towerdatas->costH;
	newTower->whereIShoot = createEmptyList();
	newTower->position = PointXYZ(-1,-1,0);
	return newTower;
}
			
