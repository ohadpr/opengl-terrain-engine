//
// YourCode.cpp
//
//
//

// includes
#include "YourCode.h"

#include "DynamicCamera.h"
#include "Terrain.h"

#include "Ending.h"

// vars
static RTexture			 l_texTerrain, l_texDetail;
static RTexture			 l_texSkyBox[6];
static RTexture			 l_texCaustics[N_CAUSTICS_TEX];
static DynamicCamera	*l_pDynCam;
static Terrain			*l_pTerrain;
static Ending			*l_pEnding;
float					 g_frameTime = 0;


// functions

static char	titleBar[1024];
char *appGetTitleBar()      {return titleBar;}





// May be useful to some people.
bool appPreInit() // Called before OpenGL has been initialized.
{
  return true;
}

void LoadTexture(RTexture *pTex, char *pName, char *pName2=0)
{
	RImage imgTmp;
	imgTmp.Create(pName, imgLoadBMP);

	if (pName2)
	{
		RImage imgTmp2;
		imgTmp2.Create(pName2, imgLoadBMP);
		pTex->Create( &imgTmp, &imgTmp2 ); 
		imgTmp.Toast();
	}
	else
		pTex->Create( &imgTmp ); 

	imgTmp.Toast();
}

// Startup Stuff.
bool appInit()	// Called right after the window is created, and OpenGL is initialized.
{
	// OpenGL inits


	// zbuffering
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// We want to use textures, so enable them.
	glEnable( GL_TEXTURE_2D );
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	// load some textures
	LoadTexture(&l_texTerrain, "../data/terrain-texture3.bmp");
	LoadTexture(&l_texDetail, "../data/detail.bmp");

	dword i;

	// load skybox textures
	for (i=0; i<6; i++) {
		char tempStr[128];
		sprintf(tempStr, "../data/SkyBox/skybox%d.bmp", i);

		LoadTexture(l_texSkyBox+i, tempStr);
	}

	// load caustics texture
	// load our caustics textures
	for (i=0; i<N_CAUSTICS_TEX; i++) {
		char tempStr[128];
		if (i < 10) {
//			sprintf(tempStr, "../data/Caustics/caust0%d.bmp", i);
		} else {
//			sprintf(tempStr, "../data/Caustics/caust%d.bmp", i);
		}

		LoadTexture(l_texCaustics+i, tempStr);
	}


	// initialize sub-systems

	// initialize our dynamic camera
	l_pDynCam = new DynamicCamera( Vector(0,100,0) );
	
	// initialize our terrain object
	l_pTerrain = new Terrain("../data/heightmap.bmp");
//	l_pTerrain = new Terrain("../data/crytek-heightmap.bmp");
	
	// set terrain parameters
	l_pTerrain->SetParams(	Vector(0,0,0),			// origin
							Vector(1000,160,1000),	// dimensions
							0.4f,					// patch density (0..1)
							0.3f);					// water height

	// set the textures the terrain will use
	l_pTerrain->SetTextures(&l_texTerrain, &l_texDetail,		// ground 1/2
							l_texSkyBox,						// skybox
							l_texCaustics);						// caustics


	l_pEnding = new Ending;

	// If everything went well, then return true.
	// Returning false will cause the program to quit right away.
	return true;
}


// Draw all the scene related stuff.
bool appRender()
{
	// We like to use floating point seconds, just for kicks.
	static float	lastTime = 0.0f;
	float			fTime = sys_Time/1000.f;
	static float	lastSecTime = fTime;
	static dword	frameCounter = 0;

	frameCounter++;
	g_frameTime = fTime-lastTime;
	lastTime = fTime;

	if (fTime-lastSecTime > 1.f)
	{
		// a second has passed
		sprintf(titleBar, "its called experimenting..., at %.1ffps", (float)frameCounter/(fTime-lastSecTime));
		frameCounter = 0;
		lastSecTime = fTime;
	}
	

	// dynamic camera
	l_pDynCam->Update();

//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClear( GL_DEPTH_BUFFER_BIT );

	// render terrain object
	l_pTerrain->Render();


	return true;
}

void appShutdown() // Called right after the window is destroyed.
{
	// delete our dynamic camera
	delete l_pDynCam;

	// delete our terrain object
	delete l_pTerrain;

	delete l_pEnding;

	l_texTerrain.Toast();
	l_texDetail.Toast();

	dword i;

	for (i=0; i<6; i++) {
		l_texSkyBox[i].Toast();
	}

	for (i=0; i<N_CAUSTICS_TEX; i++) {
//		l_texCaustics[i].Toast();
	}
}
