#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


#define LANGUAGE_FR 0 // set to 0 or 1
#if LANGUAGE_FR == 1
#define WINDOW_TITLE "TrioMan - Le Trio-Monsieur"
#define LANGUAGE "fr"
#else
#define WINDOW_TITLE "TrioMan - The Trio-Guy"
#define LANGUAGE "en"
#endif

#define ANGLE_X_STEP 3
#define ANGLE_Z_STEP 6

#define ANGLE_X_LIMIT_UP 180
#define ANGLE_X_LIMIT_DOWN 0

#define W_WIDTH 480
#define W_HEIGHT 480

#define MAP_X 4
#define MAP_Y 4
#define MAP_Z 4

#define CUBE_LENGTH 30
#define CUBE_BORDER 2

#define CHARACTER_LENGTH 10.0
#define CHARACTER_HEIGHT 10.0

#define POS_INITX 2
#define POS_INITY 2
#define POS_INITZ 2

#define POS_HEARTX 0
#define POS_HEARTY 1
#define POS_HEARTZ 4

#define CUT_XZ 0
#define CUT_YZ 1

#define SWAP_STEPS 20
#define TIME_STEP 30

#define WALK_STEP 0.1

#define G_GRAVITY 0.098
#define TRIO_MASS 0.1
#define JUMP_INIT_SPEED 0.15

#define SCREEN_BEGIN_PATH "./img/trioman-debut-" LANGUAGE ".png"
#define SCREEN_END_PATH "./img/trioman-fin-" LANGUAGE ".png"

#define FREEBLOCK(inposx, inposy, inposz) ((inposx < 0) || (inposy < 0) || (inposz < 0) \
	|| (inposx >= MAP_X) || (inposy >= MAP_Y) || (inposz >= MAP_Z) \
	|| (map[inposz][inposy][inposx] == 0))

#define GO_UP(posx, posy, posz) if (iposyp >= 0 && FREEBLOCK(iposx,iposyp,iposz)) posy -= WALK_STEP;
#define GO_DOWN(posx, posy, posz) if (iposyn <= MAP_Y - 1 && FREEBLOCK(iposx,iposyn,iposz)) posy += WALK_STEP;
#define GO_LEFT(posx, posy, posz) if (iposxp >= 0 && FREEBLOCK(iposxp,iposy,iposz)) posx -= WALK_STEP;
#define GO_RIGHT(posx, posy, posz) if (iposxn <= MAP_X - 1 && FREEBLOCK(iposxn,iposy,iposz)) posx += WALK_STEP;

GLubyte halftone[] = {
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
	0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55
};

const int map[4][4][4] = {
	{
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 }
	},
	{
		{ 0, 1, 1, 1 },
		{ 0, 0, 0, 0 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 0, 0 }
	},
	{
		{ 0, 0, 0, 0 },
		{ 0, 0, 0, 1 },
		{ 0, 1, 0, 1 },
		{ 0, 0, 0, 1 }
	},
	{
		{ 0, 0, 0, 0 },
		{ 1, 0, 0, 0 },
		{ 1, 1, 0, 0 },
		{ 0, 1, 1, 0 }
	}
};

SDL_Window* gWindow = NULL;
SDL_GLContext gContext;

bool init(); // init window creation and other elements
bool initGL(); // init OpenGL elements
void shutDown(); // close all SDL elements (called once)

// scene creation
bool initScene();
bool mainScene();
bool endScene();

bool splashscreen(const char* path) {
	bool quit = false;
	bool success = true;
	SDL_Event event;

	GLuint texName;

	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// get data from image
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (!data) {
		printf("Failed to load texture \"%s\"", path);
	}
	else {
		printf("Texture width: %d\n"
			"Texture height: %d\n"
			"Number of channels: %d\n",
			width, height, nrChannels
		);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	stbi_image_free(data);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0., 1., 0., 1., -1., 1.);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 1.f); glVertex2f(0.f, 0.f);
	glTexCoord2f(0.f, 0.f); glVertex2f(0.f, 1.f);
	glTexCoord2f(1.f, 0.f); glVertex2f(1.f, 1.f);
	glTexCoord2f(1.f, 1.f); glVertex2f(1.f, 0.f);
	glEnd();

	glFlush ();
	SDL_GL_SwapWindow(gWindow);

	while (!quit) {
		SDL_WaitEvent(&event);
		switch (event.type)
			{
			case SDL_QUIT:
				success = false;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_KEYDOWN:
				quit = true;
				break;

			default:
				break;
			}
	}

	glDeleteTextures(1, &texName);
	glDisable(GL_TEXTURE_2D);

	return success;
}

void prepareGL() {
		glClearColor (1.0, 1.0, 1.0, 0.0);
		glClearDepth(1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable (GL_BLEND);
}

void endGL() {
		glFlush ();
		SDL_GL_SwapWindow(gWindow);
}

void updatePlane(double posx, double posy, double posz, int mode, double transit) {
	double destx;
	double curposxd, curposyd;
	int starty, endy, curposx, curposy;
	int iposx, iposy;
	int posheartx, poshearty;
	int x, y, z;

	if (mode == CUT_XZ) {
		iposx = floor(posx);
		iposy = floor(posy);
		destx = posy * CUBE_LENGTH * (1 - transit);
		starty = iposy - 1;
		endy = iposy + 1;
		curposx = iposx;
		curposy = iposy;
		curposxd = posx;
		curposyd = posy;
		posheartx = POS_HEARTX;
		poshearty = POS_HEARTY;
	} else {
		iposx = floor(posx);
		iposy = floor(posy);
		destx = posx * CUBE_LENGTH * (1 - transit);
		starty = - iposx - 1;
		endy = - iposx + 1;
		curposx = iposy;
		curposy = - iposx;
		curposxd = posy;
		curposyd = posx;
		posheartx = POS_HEARTY;
		poshearty = - POS_HEARTX;
	}

	for (y = starty; y <= endy; y++) {
		if ((mode == CUT_XZ && (y < 0 || y >= MAP_Y))
			|| (mode == CUT_YZ && (- y < 0 || - y >= MAP_Y)))
			continue;

		if (y == starty) {
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.0, 0.0, 1.0, 0.3);
		} else if (y == curposy) {
			glBlendFunc (GL_ONE, GL_ZERO);
			glColor4f(0.0, 0.0, 1.0, 1.0);
		} else if (y == endy) {
			glEnable(GL_POLYGON_STIPPLE);
			glPolygonStipple(halftone);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.0, 0.0, 0.4, 1.0);
		}

		for (z = 0; z < MAP_Z; z++) {
			for (x = 0; x < MAP_X; x++) {
				if ((mode == CUT_XZ && map[z][y][x] == 1)
					|| (mode == CUT_YZ && map[z][x][-y] == 1))
					glRectd(destx + (x * CUBE_LENGTH + CUBE_BORDER) * transit,
							z * CUBE_LENGTH + CUBE_BORDER,
							destx + ((x + 1) * CUBE_LENGTH - CUBE_BORDER) * transit,
							(z + 1) * CUBE_LENGTH - CUBE_BORDER);
			}
		}

		if (y == poshearty) {
			if (y == starty) {
				glColor4f(1.0, 0.6, 0.8, 0.3);
			} else {
				glColor4f(1.0, 0.6, 0.8, 1.0);
			}
			glRectd(destx + (posheartx * CUBE_LENGTH + CUBE_BORDER) * transit,
							POS_HEARTZ * CUBE_LENGTH + CUBE_BORDER,
							destx + ((posheartx + 1) * CUBE_LENGTH - CUBE_BORDER) * transit,
							(POS_HEARTZ + 1) * CUBE_LENGTH - CUBE_BORDER);
		}

		if (y == endy)
			glDisable(GL_POLYGON_STIPPLE);

		if (y == curposy) {
			glColor4f(0.8, 0.0, 0.0, 1.0);
			glBlendFunc (GL_ONE, GL_ZERO);
			glRectd(destx + (curposxd * CUBE_LENGTH - CHARACTER_LENGTH / 2) * transit,
					posz * CUBE_LENGTH,
					destx + (curposxd * CUBE_LENGTH + CHARACTER_LENGTH / 2) * transit,
					posz * CUBE_LENGTH + CHARACTER_HEIGHT);
		}
	}
}

bool initScene()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0, (MAP_X + 1) * CUBE_LENGTH,
		0, (MAP_Z + 1) * CUBE_LENGTH,
		-100, 100);

	return true;
}

bool mainScene()
{
	bool success = true;
	bool quit = false;

	int time = 0;
	int time_prec = 0;

	double posx = POS_INITX + CHARACTER_LENGTH / CUBE_LENGTH;
	double posy = POS_INITY + CHARACTER_LENGTH / CUBE_LENGTH;
	double posz = POS_INITZ;

	int iposx = POS_INITX;
	int iposy = POS_INITY;
	int iposz = POS_INITZ;

	int iposxn, iposyn;
	int iposxp, iposyp;

	int zunder;
	int zupper;

	double speed = 0;

	double trans;

	int curplane = CUT_XZ;

	if (!initScene()) {
		printf("Failed to initialize OpenGL scene!\n");
		success = false;
	}
	else {
		//Main loop flag

		//Event handler
		SDL_Event event;

		//While application is running
		while (!quit)
		{
			iposx = floor(posx);
			iposy = floor(posy);
			iposz = floor(posz);

			iposxp = floor(posx - WALK_STEP);
			iposxn = floor(posx + WALK_STEP);
			iposyp = floor(posy - WALK_STEP);
			iposyn = floor(posy + WALK_STEP);

			// apply gravity
			if (speed == 0)
				zunder = floor(posz - 0.1);
			else
				zunder = floor(posz + speed);

			if (zunder <= iposz - 1)
				zunder = iposz - 1;

			zupper = floor(posz + speed + CHARACTER_HEIGHT / CUBE_LENGTH);

			if ((speed > 0) && (zupper >= iposz + 1) && (iposz + 1 < MAP_Z) && !FREEBLOCK(iposx, iposy, iposz + 1)) {
				speed = 0;
			}
			else if ((speed > 0) || (zunder >= MAP_Z) || FREEBLOCK(iposx, iposy, zunder)) {
				posz += speed;
				speed -= G_GRAVITY * TRIO_MASS;
			}
			else {
				posz = iposz;
				speed = 0;
			}

			prepareGL();
			updatePlane(posx, posy, posz, curplane, 1.0);
			endGL();

			iposx = floor(posx);
			iposy = floor(posy);
			iposz = floor(posz);

			iposxp = floor(posx - WALK_STEP - CHARACTER_LENGTH / (2 * CUBE_LENGTH));
			iposxn = floor(posx + WALK_STEP + CHARACTER_LENGTH / (2 * CUBE_LENGTH));
			iposyp = floor(posy - WALK_STEP - CHARACTER_LENGTH / (2 * CUBE_LENGTH));
			iposyn = floor(posy + WALK_STEP + CHARACTER_LENGTH / (2 * CUBE_LENGTH));
			//printf("%d %d %d %d", iposxp, iposxn, iposyp, iposyn);

			time = SDL_GetTicks();
			time_prec = SDL_GetTicks();
			if (time_prec - time < TIME_STEP)
				SDL_Delay(TIME_STEP - time_prec + time);

			if (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_QUIT:
					success = false;
					quit = true;
					break;

				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
					case SDLK_x:
						for (trans = 0.0; trans < SWAP_STEPS; trans += 1.0) {
							time = SDL_GetTicks();
							time_prec = SDL_GetTicks();
							if (time_prec - time < TIME_STEP)
								SDL_Delay(TIME_STEP - time_prec + time);

							prepareGL();
							if (curplane == CUT_XZ) {
								updatePlane(posx, posy, posz, CUT_YZ, trans / SWAP_STEPS);
								updatePlane(posx, posy, posz, CUT_XZ, 1.0 - trans / SWAP_STEPS);
							}
							else {
								updatePlane(posx, posy, posz, CUT_XZ, trans / SWAP_STEPS);
								updatePlane(posx, posy, posz, CUT_YZ, 1.0 - trans / SWAP_STEPS);
							}
							endGL();
						}
						curplane = (curplane + 1) % 2;
						break;

					case SDLK_c:
						if (speed == 0)
							speed = JUMP_INIT_SPEED;
						break;

					case SDLK_ESCAPE:
						success = false;
						quit = true;
					default:
						break;
					}
					break;
				default:
					break;
				}
			} // if (SDL_PollEvent(&event))

			const Uint8* keystate = SDL_GetKeyboardState(NULL);

			if (keystate[SDL_SCANCODE_LEFT]) {
				if (curplane == CUT_XZ) {
					GO_LEFT(posx, posy, posz);
				}
				else {
					GO_UP(posx, posy, posz);
				}
			}
			if (keystate[SDL_SCANCODE_RIGHT]) {
				if (curplane == CUT_XZ) {
					GO_RIGHT(posx, posy, posz);
				}
				else {
					GO_DOWN(posx, posy, posz);
				}
			}

			if ((iposx == POS_HEARTX) && (iposy == POS_HEARTY) && (iposz == POS_HEARTZ))
				quit = true;
		} // if (!quit)

		endScene();
	}

	return success;

}

bool endScene()
{
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_BLEND);

	return true;
}

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		//Create window
		gWindow = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, W_WIDTH, W_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Use Vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
				}

				//Initialize OpenGL
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}

	return success;
}

bool initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;

	//Initialize Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	//Initialize Modelview Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	//Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	return success;
}

void shutDown()
{
	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else if (splashscreen(SCREEN_BEGIN_PATH) && mainScene())
	{
		splashscreen(SCREEN_END_PATH);
	}

	//Free resources and close SDL
	shutDown();

	return 0;
}
