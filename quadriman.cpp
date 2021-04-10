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
#define WINDOW_TITLE "QuadriMan - Le QuadriMonsieur"
#define LANGUAGE "fr"
#else
#define WINDOW_TITLE "QuadriMan - The Quadri-Guy"
#define LANGUAGE "en"
#endif

#define ANGLE_X_INIT 90
#define ANGLE_Z_INIT 0

#define ANGLE_X_LIMIT_UP 90
#define ANGLE_X_LIMIT_DOWN 0

#define ANGLE_X_DAMPING 5
#define ANGLE_Z_DAMPING 5

#define ANGLE_X_STEP 30
#define ANGLE_Z_STEP 30

#define W_WIDTH 480
#define W_HEIGHT 480

#define MAP_X 4
#define MAP_Y 4
#define MAP_Z 4
#define MAP_W 4

#define CUBE_LENGTH 30
#define CUBE_BORDER 3
#define FRONT_BORDER 2
#define BACK_BORDER 1

#define ALPHA_FRONT 0.5
#define ALPHA_BACK 0.4

#define CHARACTER_LENGTH 10.0
#define CHARACTER_HEIGHT 15.0

#define POS_INITX 2
#define POS_INITY 0
#define POS_INITZ 0
#define POS_INITW 2

#define POS_HEARTX 0
#define POS_HEARTY 1
#define POS_HEARTZ 0
#define POS_HEARTW 1

#define CUT_XYW 0
#define CUT_YZW 1
#define CUT_ZXW 2

#define SWAP_STEPS 20
#define TIME_STEP 30

#define WALK_STEP 0.1

#define G_GRAVITY 0.098
#define TRIO_MASS 0.1
#define JUMP_INIT_SPEED 0.15

#define CAMERA_FRONT 0
#define CAMERA_RIGHT 1
#define CAMERA_LEFT 2
#define CAMERA_BACK 3

#define SCREEN_BEGIN_PATH "./img/quadriman-debut-" LANGUAGE ".png"
#define SCREEN_END_PATH "./img/quadriman-fin-" LANGUAGE ".png"

#define FREEBLOCK(inposx, inposy, inposz, inposw) ((inposx < 0) || (inposy < 0) || (inposz < 0) || (inposw < 0) \
	|| (inposx >= MAP_X) || (inposy >= MAP_Y) || (inposz >= MAP_Z) || (inposw >= MAP_W) \
	|| (map[inposw][inposz][inposy][inposx] == 0))

#define GO_XM(posx, posy, posz, posw) if (iposxp >= 0 && FREEBLOCK(iposxp,iposy,iposz,iposw)) posx -= WALK_STEP;
#define GO_XP(posx, posy, posz, posw) if (iposxn <= MAP_X - 1 && FREEBLOCK(iposxn,iposy,iposz,iposw)) posx += WALK_STEP;
#define GO_YM(posx, posy, posz, posw) if (iposyp >= 0 && FREEBLOCK(iposx,iposyp,iposz,iposw)) posy -= WALK_STEP;
#define GO_YP(posx, posy, posz, posw) if (iposyn <= MAP_Y - 1 && FREEBLOCK(iposx,iposyn,iposz,iposw)) posy += WALK_STEP;
#define GO_ZM(posx, posy, posz, posw) if (iposzp >= 0 && FREEBLOCK(iposx,iposy,iposzp,iposw)) posz -= WALK_STEP;
#define GO_ZP(posx, posy, posz, posw) if (iposzn <= MAP_Y - 1 && FREEBLOCK(iposx,iposy,iposzn,iposw)) posz += WALK_STEP;

static GLuint texName;

static GLubyte texstripe[64][64];
static GLubyte texcross[64][64];

const int map[4][4][4][4] = {
	{
		{
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 }
		},
		{
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 }
		},
		{
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 }
		},
		{
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 }
		}
	},
	{
		{
			{ 0, 1, 0, 1 },
			{ 0, 1, 1, 1 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 0, 0 }
		},
		{
			{ 0, 0, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 0, 0 }
		},
		{
			{ 1, 0, 1, 1 },
			{ 1, 0, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 0, 0, 1, 0 }
		}
	},
	{
		{
			{ 1, 1, 0, 1 },
			{ 1, 1, 1, 1 },
			{ 0, 1, 1, 1 },
			{ 1, 1, 0, 0 }
		},
		{
			{ 0, 1, 0, 1 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 1 },
			{ 1, 1, 0, 0 }
		},
		{
			{ 1, 1, 1, 0 },
			{ 1, 1, 1, 0 },
			{ 1, 0, 1, 1 },
			{ 1, 1, 0, 0 }
		},
		{
			{ 1, 1, 1, 1 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 1 },
			{ 1, 1, 0, 0 }
		}
	},
	{
		{
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 1, 1, 1 },
			{ 1, 1, 0, 1 },
			{ 1, 1, 0, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 0, 0, 1, 1 },
			{ 1, 1, 1, 1 },
			{ 0, 1, 1, 1 },
			{ 0, 0, 0, 0 }
		}
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


void prepareGL(double angle_x, double angle_z) {
	static const GLfloat mat_shininess[] = { 50 };
	static const GLfloat light_color[] = { 1.0, 1.0, 1.0, 1.0 };
	static const GLfloat mat_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
	static const GLfloat light_position[] = { CUBE_LENGTH * MAP_X / 2, CUBE_LENGTH * MAP_Y / 2, CUBE_LENGTH * MAP_Z, 0.0 };

	glPushMatrix();

	glTranslatef(0.0, 0.0, -170.0);
	glRotatef(-angle_x, 1.0, 0.0, 0.0);
	glRotatef(-angle_z, 0.0, 0.0, 1.0);
	glTranslatef(-CUBE_LENGTH * MAP_X / 2, -CUBE_LENGTH * MAP_Y / 2, -CUBE_LENGTH * MAP_Z / 2);

	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_SMOOTH);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_FRONT, GL_AMBIENT, mat_ambient);
}

void endGL() {

	glPopMatrix();

	glFlush();
	SDL_GL_SwapWindow(gWindow);
}

void drawCube(double x1, double y1, double z1, double x2, double y2, double z2, double border) {
	x1 -= border;
	y1 -= border;
	z1 -= border;

	x2 += border;
	y2 += border;
	z2 += border;

	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, -1.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(x2, y1, z1);
	glTexCoord2f(1.0, 1.0); glVertex3f(x2, y2, z1);
	glTexCoord2f(0.0, 1.0); glVertex3f(x1, y2, z1);
	glTexCoord2f(0.0, 0.0); glVertex3f(x1, y1, z1);

	glNormal3f(0.0, 0.0, 1.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(x1, y1, z2);
	glTexCoord2f(1.0, 1.0); glVertex3f(x1, y2, z2);
	glTexCoord2f(0.0, 1.0); glVertex3f(x2, y2, z2);
	glTexCoord2f(0.0, 0.0); glVertex3f(x2, y1, z2);

	glNormal3f(0.0, -1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(x1, y1, z2);
	glTexCoord2f(1.0, 1.0); glVertex3f(x2, y1, z2);
	glTexCoord2f(0.0, 1.0); glVertex3f(x2, y1, z1);
	glTexCoord2f(0.0, 0.0); glVertex3f(x1, y1, z1);

	glNormal3f(0.0, 1.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(x1, y2, z1);
	glTexCoord2f(1.0, 1.0); glVertex3f(x2, y2, z1);
	glTexCoord2f(0.0, 1.0); glVertex3f(x2, y2, z2);
	glTexCoord2f(0.0, 0.0); glVertex3f(x1, y2, z2);

	glNormal3f(-1.0, 0.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(x1, y2, z1);
	glTexCoord2f(1.0, 1.0); glVertex3f(x1, y2, z2);
	glTexCoord2f(0.0, 1.0); glVertex3f(x1, y1, z2);
	glTexCoord2f(0.0, 0.0); glVertex3f(x1, y1, z1);

	glNormal3f(1.0, 0.0, 0.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(x2, y1, z1);
	glTexCoord2f(1.0, 1.0); glVertex3f(x2, y1, z2);
	glTexCoord2f(0.0, 1.0); glVertex3f(x2, y2, z2);
	glTexCoord2f(0.0, 0.0); glVertex3f(x2, y2, z1);
	glEnd();
}

void updatePlane(double posx, double posy, double posz, double posw, int mode, double transit, int go) {
	double destx, desty;
	double curposxd, curposyd, curposzd;
	int startz, endz, curposx, curposy, curposz;
	int iposx, iposy, iposz;
	int posheartx, poshearty, posheartz;
	int iz;
	int zarray[3];
	int x, y, z, w;
	double border = 0;

	iposx = floor(posx);
	iposy = floor(posy);
	iposz = floor(posz);

	if (mode == CUT_XYW) {
		if (go == 1) {
			destx = posy * CUBE_LENGTH * (1 - transit);
			desty = posz * CUBE_LENGTH * (1 - transit);
		}
		else {
			destx = posz * CUBE_LENGTH * (1 - transit);
			desty = posx * CUBE_LENGTH * (1 - transit);
		}
		startz = iposz - 1;
		endz = iposz + 1;
		curposx = iposx;
		curposy = iposy;
		curposz = iposz;
		curposxd = posx;
		curposyd = posy;
		curposzd = posz;
		posheartx = POS_HEARTX;
		poshearty = POS_HEARTY;
		posheartz = POS_HEARTZ;
	}
	else if (mode == CUT_YZW) {
		if (go == 1) {
			destx = posz * CUBE_LENGTH * (1 - transit);
			desty = posx * CUBE_LENGTH * (1 - transit);
		}
		else {
			destx = posx * CUBE_LENGTH * (1 - transit);
			desty = posy * CUBE_LENGTH * (1 - transit);
		}
		startz = iposx - 1;
		endz = iposx + 1;
		curposx = iposy;
		curposy = iposz;
		curposz = iposx;
		curposxd = posy;
		curposyd = posz;
		curposzd = posx;
		posheartx = POS_HEARTY;
		poshearty = POS_HEARTZ;
		posheartz = POS_HEARTX;
	}
	else {
		if (go == 1) {
			destx = posx * CUBE_LENGTH * (1 - transit);
			desty = posy * CUBE_LENGTH * (1 - transit);
		}
		else {
			destx = posy * CUBE_LENGTH * (1 - transit);
			desty = posz * CUBE_LENGTH * (1 - transit);
		}
		startz = iposy - 1;
		endz = iposy + 1;
		curposx = iposz;
		curposy = iposx;
		curposz = iposy;
		curposxd = posz;
		curposyd = posx;
		curposzd = posy;
		posheartx = POS_HEARTZ;
		poshearty = POS_HEARTX;
		posheartz = POS_HEARTY;
	}

	zarray[0] = curposz;
	zarray[1] = startz;
	zarray[2] = endz;

	for (iz = 0; iz < 3; iz++) {
		z = zarray[iz];

		if (z < 0 || z >= MAP_Y)
			continue;

		if (z == startz) {
			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glColor4f(0.5, 0.5, 1.0, ALPHA_BACK);

			border = BACK_BORDER;
		}
		else if (z == curposz) {
			glColor4f(0.0, 0.0, 1.0, 0.8);
			border = 0.0;
		}
		else if (z == endz) {
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texName);
			glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

			glEnable(GL_BLEND);
			glDepthMask(GL_FALSE);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glColor4f(0.0, 0.0, 0.4, ALPHA_FRONT);

			border = FRONT_BORDER;
		}

		for (w = 0; w < MAP_W; w++) {
			for (y = 0; y < MAP_Y; y++) {
				for (x = 0; x < MAP_X; x++) {
					if ((mode == CUT_XYW && map[w][z][y][x] == 1)
						|| (mode == CUT_YZW && map[w][y][x][z] == 1)
						|| (mode == CUT_ZXW && map[w][x][z][y] == 1))
						drawCube(destx + (x * CUBE_LENGTH + CUBE_BORDER) * transit,
							desty + (y * CUBE_LENGTH + CUBE_BORDER) * transit,
							w * CUBE_LENGTH + CUBE_BORDER,
							destx + ((x + 1) * CUBE_LENGTH - CUBE_BORDER) * transit,
							desty + ((y + 1) * CUBE_LENGTH - CUBE_BORDER) * transit,
							(w + 1) * CUBE_LENGTH - CUBE_BORDER, border);
				}
			}
		}

		if (z == posheartz) {
			if (z == startz) {
				glColor4f(1.0, 0.3, 0.5, ALPHA_BACK);
			}
			else if (z == curposz) {
				glColor4f(1.0, 0.3, 0.5, 1.0);
			}
			else {
				glColor4f(1.0, 0.3, 0.5, ALPHA_FRONT);
			}
			drawCube(destx + (posheartx * CUBE_LENGTH + CUBE_BORDER) * transit,
				desty + (poshearty * CUBE_LENGTH + CUBE_BORDER) * transit,
				POS_HEARTW * CUBE_LENGTH + CUBE_BORDER,
				destx + ((posheartx + 1) * CUBE_LENGTH - CUBE_BORDER) * transit,
				desty + ((poshearty + 1) * CUBE_LENGTH - CUBE_BORDER) * transit,
				(POS_HEARTW + 1) * CUBE_LENGTH - CUBE_BORDER, border);
		}

		if (z == curposz) {
			glColor4f(0.8, 0.0, 0.0, 1.0);
			drawCube(destx + (curposxd * CUBE_LENGTH) * transit,
				desty + (curposyd * CUBE_LENGTH) * transit,
				posw * CUBE_LENGTH,
				destx + (curposxd * CUBE_LENGTH + CHARACTER_LENGTH) * transit,
				desty + (curposyd * CUBE_LENGTH + CHARACTER_LENGTH) * transit,
				posw * CUBE_LENGTH + CHARACTER_HEIGHT, border);
		}
		else {
			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
		}
	}
}

bool splashscreen(const char* path) {
	bool quit = false;
	bool success = true;
	SDL_Event event;

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_2D, texName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);

	// get data from image
	int width, height, nrChannels;
	unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
	if (!data) {
		printf("Failed to load texture \"%s\"", path);
	}
	else {
		/*
		printf("Texture width: %d\n"
			"Texture height: %d\n"
			"Number of channels: %d\n",
			width, height, nrChannels
		);
		*/
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	stbi_image_free(data);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glOrtho(0., 1., 0., 1., -1., 1.);

	glColor3f(1.f, 1.f, 1.f);
	glBindTexture(GL_TEXTURE_2D, texName);
	glBegin(GL_QUADS);
	glTexCoord2f(0.f, 1.f); glVertex3f(0.f, 0.f, 0.f);
	glTexCoord2f(0.f, 0.f); glVertex3f(0.f, 1.f, 0.f);
	glTexCoord2f(1.f, 0.f); glVertex3f(1.f, 1.f, 0.f);
	glTexCoord2f(1.f, 1.f); glVertex3f(1.f, 0.f, 0.f);
	glEnd();

	glFlush();
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

void generateTextures() {
	int x, y;

	for (x = 0; x < 64; x++) {
		for (y = 0; y < 64; y++) {
			if ((x + y) % 8 < 4)
				texstripe[y][x] = 0xff;
			else
				texstripe[y][x] = 0x00;

			if ((x % 16 < 8) && (y % 16 < 8))
				texcross[y][x] = 0xff;
			else
				texcross[y][x] = 0x00;
		}
	}
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

bool initScene()
{
	bool success = true;

	// generate current Textures
	generateTextures();
	glGenTextures(1, &texName);

	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
		GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 64,
		64, 0, GL_ALPHA, GL_UNSIGNED_BYTE,
		texcross);

	glClearColor(1.f, 1.f, 1.f, 1.f);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-1.f, 1.f, -1.f, 1.f, 1.5f, 250.f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return success;
}

bool endScene()
{
	// delete texture
	glDeleteTextures(1, &texName);

	glClearColor(1.f, 1.f, 1.f, 0.f);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	return true;
}

bool mainScene()
{
	bool success = true;
	bool quit = false;

	double angle_x = ANGLE_X_INIT;
	double angle_z = ANGLE_Z_INIT;
	double angle_xcons = angle_x;
	double angle_zcons = angle_z;
	int is_down = 0;

	int time = 0;
	int time_prec = 0;

	double posx = POS_INITX + CHARACTER_LENGTH / CUBE_LENGTH;
	double posy = POS_INITY + CHARACTER_LENGTH / CUBE_LENGTH;
	double posz = POS_INITZ;
	double posw = POS_INITW;

	int iposx = POS_INITX;
	int iposy = POS_INITY;
	int iposz = POS_INITZ;
	int iposw = POS_INITW;

	int iposxn, iposyn, iposzn;
	int iposxp, iposyp, iposzp;

	int wunder;
	int wupper;

	int camera_pos = CAMERA_FRONT;

	double speed = 0;

	double trans;

	int curplane = CUT_XYW;

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
			iposw = floor(posw);

			iposxp = floor(posx - WALK_STEP);
			iposxn = floor(posx + WALK_STEP);
			iposyp = floor(posy - WALK_STEP);
			iposyn = floor(posy + WALK_STEP);
			iposzp = floor(posz - WALK_STEP);
			iposzn = floor(posz + WALK_STEP);

			// apply gravity
			if (speed == 0)
				wunder = floor(posw - 0.1);
			else
				wunder = floor(posw + speed);

			if (wunder <= iposw - 1)
				wunder = iposw - 1;

			wupper = floor(posw + speed + CHARACTER_HEIGHT / CUBE_LENGTH);

			if ((speed > 0) && (wupper >= iposw + 1) && (iposw + 1 < MAP_W) && !FREEBLOCK(iposx, iposy, iposz, iposw + 1)) {
				speed = 0;
			}
			else if ((speed > 0) || (wunder >= MAP_Z) || FREEBLOCK(iposx, iposy, iposz, wunder)) {
				posw += speed;
				speed -= G_GRAVITY * TRIO_MASS;
			}
			else {
				posw = iposw;
				speed = 0;
			}

			angle_z += (angle_zcons - angle_z) / ANGLE_X_DAMPING;

			angle_x += (angle_xcons - angle_x) / ANGLE_X_DAMPING;
			if (angle_x > ANGLE_X_LIMIT_UP)
				angle_x = ANGLE_X_LIMIT_UP;
			if (angle_x < ANGLE_X_LIMIT_DOWN)
				angle_x = ANGLE_X_LIMIT_DOWN;


			prepareGL(angle_x, angle_z);

			updatePlane(posx, posy, posz, posw, curplane, 1.0, 1);

			endGL();

			iposx = floor(posx);
			iposy = floor(posy);
			iposz = floor(posz);
			iposw = floor(posw);

			iposxp = floor(posx - WALK_STEP);
			iposxn = floor(posx + WALK_STEP + CHARACTER_LENGTH / (CUBE_LENGTH));
			iposyp = floor(posy - WALK_STEP);
			iposyn = floor(posy + WALK_STEP + CHARACTER_LENGTH / (CUBE_LENGTH));
			iposzp = floor(posz - WALK_STEP);
			iposzn = floor(posz + WALK_STEP + CHARACTER_LENGTH / (CUBE_LENGTH));
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
					case SDLK_t:
						for (trans = 0.0; trans < SWAP_STEPS; trans += 1.0) {
							time = SDL_GetTicks();
							time_prec = SDL_GetTicks();
							if (time_prec - time < TIME_STEP)
								SDL_Delay(TIME_STEP - time_prec + time);

							prepareGL(angle_x, angle_z);
							if (curplane == CUT_XYW) {
								updatePlane(posx, posy, posz, posw, CUT_YZW, trans / SWAP_STEPS, 0);
								updatePlane(posx, posy, posz, posw, CUT_XYW, 1.0 - trans / SWAP_STEPS, 1);
							}
							else if (curplane == CUT_YZW) {
								updatePlane(posx, posy, posz, posw, CUT_ZXW, trans / SWAP_STEPS, 0);
								updatePlane(posx, posy, posz, posw, CUT_YZW, 1.0 - trans / SWAP_STEPS, 1);
							}
							else {
								updatePlane(posx, posy, posz, posw, CUT_XYW, trans / SWAP_STEPS, 0);
								updatePlane(posx, posy, posz, posw, CUT_ZXW, 1.0 - trans / SWAP_STEPS, 1);
							}

							endGL();
						}
						curplane = (curplane + 1) % 3;
						break;

					case SDLK_r:
						for (trans = 0.0; trans < SWAP_STEPS; trans += 1.0) {
							time = SDL_GetTicks();
							time_prec = SDL_GetTicks();
							if (time_prec - time < TIME_STEP)
								SDL_Delay(TIME_STEP - time_prec + time);

							prepareGL(angle_x, angle_z);
							if (curplane == CUT_XYW) {
								updatePlane(posx, posy, posz, posw, CUT_ZXW, trans / SWAP_STEPS, 1);
								updatePlane(posx, posy, posz, posw, CUT_XYW, 1.0 - trans / SWAP_STEPS, 0);
							}
							else if (curplane == CUT_YZW) {
								updatePlane(posx, posy, posz, posw, CUT_XYW, trans / SWAP_STEPS, 1);
								updatePlane(posx, posy, posz, posw, CUT_YZW, 1.0 - trans / SWAP_STEPS, 0);
							}
							else {
								updatePlane(posx, posy, posz, posw, CUT_YZW, trans / SWAP_STEPS, 1);
								updatePlane(posx, posy, posz, posw, CUT_ZXW, 1.0 - trans / SWAP_STEPS, 0);
							}

							endGL();
						}
						curplane = (curplane + 2) % 3;
						break;

					case SDLK_g:
						if (speed == 0)
							speed = JUMP_INIT_SPEED;
						break;

					case SDLK_s:
						angle_zcons += ANGLE_Z_STEP;
						if (angle_zcons >= 180) {
							angle_zcons -= 360;
							angle_z -= 360;
						}
						break;

					case SDLK_f:
						angle_zcons -= ANGLE_Z_STEP;
						if (angle_zcons <= -180) {
							angle_zcons += 360;
							angle_z += 360;
						}
						break;

					case SDLK_d:
						angle_xcons -= ANGLE_X_STEP;
						if (angle_xcons < ANGLE_X_LIMIT_DOWN)
							angle_xcons = ANGLE_X_LIMIT_DOWN;
						break;

					case SDLK_e:
						angle_xcons += ANGLE_X_STEP;
						if (angle_xcons > ANGLE_X_LIMIT_UP)
							angle_xcons = ANGLE_X_LIMIT_UP;
						break;

					case SDLK_ESCAPE:
						success = false;
						quit = true;
						break;

					default:
						break;
					}
					break;
				default:
					break;
				}
			} // if (SDL_PollEvent(&event))

			if (angle_zcons < -180 + 45)
				camera_pos = CAMERA_BACK;
			else if (angle_zcons < -90 + 45)
				camera_pos = CAMERA_LEFT;
			else if (angle_zcons < 45)
				camera_pos = CAMERA_FRONT;
			else if (angle_zcons < 90 + 45)
				camera_pos = CAMERA_RIGHT;
			else
				camera_pos = CAMERA_BACK;

			const Uint8* keystate = SDL_GetKeyboardState(NULL);

			if (curplane == CUT_XYW) {
				switch (camera_pos) {
				case CAMERA_FRONT:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_XM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_XP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_YP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_YM(posx, posy, posz, posw);
					}
					break;
				case CAMERA_BACK:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_XP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_XM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_YM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_YP(posx, posy, posz, posw);
					}
					break;
				case CAMERA_LEFT:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_YP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_YM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_XP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_XM(posx, posy, posz, posw);
					}
					break;
				case CAMERA_RIGHT:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_YM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_YP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_XM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_XP(posx, posy, posz, posw);
					}
					break;
				default:
					break;
				}
			}
			else if (curplane == CUT_YZW) {
				switch (camera_pos) {
				case CAMERA_FRONT:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_YM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_YP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_ZP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_ZM(posx, posy, posz, posw);
					}
					break;
				case CAMERA_BACK:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_YP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_YM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_ZM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_ZP(posx, posy, posz, posw);
					}
					break;
				case CAMERA_LEFT:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_ZP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_ZM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_YP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_YM(posx, posy, posz, posw);
					}
					break;
				case CAMERA_RIGHT:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_ZM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_ZP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_YM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_YP(posx, posy, posz, posw);
					}
					break;
				default:
					break;
				}
			}
			else {
				switch (camera_pos) {
				case CAMERA_FRONT:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_ZM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_ZP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_XP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_XM(posx, posy, posz, posw);
					}
					break;
				case CAMERA_BACK:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_ZP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_ZM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_XM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_XP(posx, posy, posz, posw);
					}
					break;
				case CAMERA_LEFT:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_XP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_XM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_ZP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_ZM(posx, posy, posz, posw);
					}
					break;
				case CAMERA_RIGHT:
					if (keystate[SDL_SCANCODE_LEFT]) {
						GO_XM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_RIGHT]) {
						GO_XP(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_UP]) {
						GO_ZM(posx, posy, posz, posw);
					}
					if (keystate[SDL_SCANCODE_DOWN]) {
						GO_ZP(posx, posy, posz, posw);
					}
					break;
				default:
					break;
				}
			}

			if ((iposx == POS_HEARTX) && (iposy == POS_HEARTY) && (iposz == POS_HEARTZ) && (iposw == POS_HEARTW))
				quit = true;
		} // if (!quit)

		endScene();
	}

	return success;
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
