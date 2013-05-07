#include "graphics.h"
#include <SDL/SDL_image.h>

//TODO
void initGameGraphics(const SDL_Surface* map){
	if(map == NULL){
		fprintf(stderr, "Erreur : les graphismes du jeu ne peuvent être initialisés avant le chargement de la map.\n");
		exit(-1);
	}

	GAME_TEXTURES_ID.BLUE_OCTOPUS_ID = makeTextureFromFile("images/monster1.png");
	GAME_TEXTURES_ID.ORANGE_OCTOPUS_ID = makeTextureFromFile("images/monster2.png");
	GAME_TEXTURES_ID.GREEN_OCTOPUS_ID = makeTextureFromFile("images/monster3.png");
	GAME_TEXTURES_ID.MAP_ID = makeTextureFromSurface(map);
}

GLuint makeTextureFromFile(const char* imagePath){
	if(imagePath == NULL) return -1;
	
	GLuint textureId;
	
	SDL_Surface* image = IMG_Load(imagePath);
	if (image ==NULL){
		fprintf(stderr,"Erreur fatale : impossible de charger l'image %s\n", imagePath);
		exit(1);
	}

	textureId = makeTextureFromSurface(image);
	
	SDL_FreeSurface(image);
	
	return textureId;
}

GLuint makeTextureFromSurface(const SDL_Surface* image){

	if(image == NULL) return -1;
	GLuint textureId;
	GLenum format;

	glGenTextures(1,&textureId);

	glBindTexture(GL_TEXTURE_2D,textureId);
  
	switch(image->format->BytesPerPixel) {
		case 1:
		format = GL_RED;
		break;

		case 3:
		format = GL_RGB;
		break;

		case 4:
		format = GL_RGBA;
		break;

		default:
		fprintf(stderr, "Format des pixels de l’image  non pris en charge\n");
		return EXIT_FAILURE;
	}

	/*envoi de la texture à openGL*/

	glTexImage2D(GL_TEXTURE_2D,0,4, image->w, image->h, 0, format,GL_UNSIGNED_BYTE, image->pixels);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
 
	glBindTexture(GL_TEXTURE_2D, 0);

	
	return textureId;
}

Point3D sdlToOpenGL(Point3D sdlPoint){
	Point3D oglPoint;
	oglPoint.z = 0.0;
	
	oglPoint.x = GL_X_AXIS_MIN + 2*GL_X_AXIS_MAX* sdlPoint.x / WINDOW_WIDTH;
	oglPoint.y = - (GL_Y_AXIS_MIN + 2*GL_Y_AXIS_MAX* sdlPoint.y / WINDOW_HEIGHT);
	
	return oglPoint;
}

void reshape() {
	glViewport(0, 0 , WINDOW_WIDTH , WINDOW_HEIGHT );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GL_X_AXIS_MAX = WINDOW_WIDTH / 2.0;
	GL_X_AXIS_MIN = - GL_X_AXIS_MAX;
	GL_Y_AXIS_MAX = WINDOW_HEIGHT / 2.0;
	GL_Y_AXIS_MIN = - GL_Y_AXIS_MAX;

	gluOrtho2D(GL_X_AXIS_MIN, GL_X_AXIS_MAX, GL_Y_AXIS_MIN, GL_Y_AXIS_MAX);
}

int setVideoMode() {
	WINDOW_WIDTH = 800;
	WINDOW_HEIGHT = 600;
	if(NULL == SDL_SetVideoMode(WINDOW_WIDTH, WINDOW_HEIGHT, BIT_PER_PIXEL, SDL_OPENGL | SDL_GL_DOUBLEBUFFER)) {
		return EXIT_FAILURE;
	}

	reshape();

	return EXIT_SUCCESS;
}

int initWindow(){
	/* Initialisation de la SDL */
	if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
	fprintf(stderr, "Impossible d'initialiser la SDL.\n");
	return EXIT_FAILURE;
	}

	/* Ouverture d'une fenêtre et création d'un contexte OpenGL */
	if(EXIT_FAILURE == setVideoMode()) {
		fprintf(stderr, "Impossible d'ouvrir la fenetre.\n");
		return EXIT_FAILURE;
	}

	/* Titre de la fenêtre */
	SDL_WM_SetCaption("ITD Avatanéo Camarasa Chiganne", NULL);
	
	return 0;
}
