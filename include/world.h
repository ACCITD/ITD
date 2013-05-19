#ifndef ITD_WORLD_H
#define ITD_WORLD_H
#include "map.h"
#include "tower.h"
#include "monster.h"
#include "utils.h"
#include "list.h"

#define MONSTERS_PER_WAVE 10

/* Unité de temps d'un pas de jeu = 10ms */
static const int TIMESTEP_MILLISECONDS = 10;

/*Temps d'attente en nombre de tours entre deux vagues successives */
static const int NB_TURNS_BETWEEN_WAVES = 100;//=10seconde

/* Nombre de vagues de monstres que le jouer va devoir affronter */
static const int NB_TOTAL_WAVES = 20;

/**
 * Structure représentant l'état du "monde",
 * c'est-à-dire de la partie en cours.
 * Choix de conception non définitifs...
 */
typedef struct{
	Uint32 worldTime;///Temps actuel dans le tour de jeu
	Map map;///Map chargée
	unsigned char currentMonstersWave;///Vague de monstres en cours (sur 20)
	char nbMonstersAlive;///Nombre de monstres encore en vie
	bool isBetweenWaves;///Indique si l'on est entre deux vagues de monstres
	Uint32 nbTurnsWaiting;///Nombre de tours restant à attendre avant la prochaine vague de monstres
	bool paused; //Indique si le jeu est en pause ou non
	Monster monsters[MONSTERS_PER_WAVE];///Tableau des monstres actuellement sur la map.
	List* towersList;///Liste des tours actuellement sur la map.
	int money;
	Point3D cameraPosition;
	bool gameWinned;
	bool gameLoosed;
}World;


//------------ FONCTIONS FAITES POUR ETRE APPELEES DE L'EXTERIEUR ---------
/**
 * Initialise un monde à partir du fichier .itd
 * dont le chemin est passé en paramètre.
 */
World initWorld(const char* pathToItdFile);

/**
 * Initialise le timer du monde qui gère les tours de jeu.
 */
void startWorld(World* world);

/**
 * Fait avancer le monde d'un ou plusieurs pas de temps.
 * Appelée à chaque nouveau tour de boucle de 
 * jeu. Renvoie si le jeu est terminé ou non.
 */
bool worldNewStep(World* world);

/**
 * Indique s'il est possible de poser une tour à
 * la position indiquée en paramètre (en coordonnées openGL et ITD)
 **/
bool canIPutATowerHere(World* world, Point3D oglPosition, Point3D itdPosition);

/**
 * Ajoute une tour de type 'type' à la position posX posY
 * sur la carte du jeu.
 * Si la zone où la tour est posée n'est pas constructible,
 * ne pose pas la tour (fait appel à canIPutATowerHere).
 * Renvoie si l'ajout a réussi ou non
 **/
bool addTowerOnMap(World* world, int posX, int posY, TowerType type);

void cleanWorld(World* world);


// -------- FONCTIONS INTERNES, A NE PAS APPELER DE L'EXTERIEUR -----------
/*
 * Effectue un tour de jeux.
 * Appelée par worldNewStep.
 */
bool doTurn(World* world);

/*
 * Démarre une nouvelle vague de monstres.
 */
void startNewMonsterWave(World* world);

/**
 * Fait tirer toutes les tours capable
 * de toucher un monstre au moment où
 * cette fonction est appelée.
 * (appelée à chaque pas de temps).
 * Renvoie l'argent gagné par les monstres tués.
 */
int towersShoot(List* towerList, Monster* monsters);

/**
 * Fait tirer une tour (si elle le peut).
 * Appelée par towersShoot. Renvoie l'argent gagné par les monstres
 * tués.
 **/
int towerShoots(Tower* tower, Monster* monsters);

/**
 * Fait bouger tous les monstres à chaque pas de temps. 
 * Sachant qu'un monstre se déplace au maximum d'un pixel
 * par pas de temps, se contentera souvent de simplement
 * mettre à jour la durée depuis laquel le monstre a
 * bougé depuis la dernière fois.
 */
void moveMonsters(Monster* monsters, List* pathNodeList);

#endif
