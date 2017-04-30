//-------------------------------------------------------------------
//	File:		YourCode.cpp
//	Comments:	Write your visual effect here.
//-------------------------------------------------------------------
#include "../source/std.h"      // Include the standard header.
#include "../source/RTexture.h" // We use textures, just for kicks.
#include "../source/imgLoadBMP.h" // Image loader handler for RTexture/RImage.

// This header binds this code with the system framework code.
#include "../source/OGLCMainHeader.h"

#include "DCam.h"


// Return the application info.
// Functions implemented by the author in YourCode.cpp
bool appPreInit();  // Initialize before OpenGL has been initialized.
bool appInit();     // Initialize after OpenGL has been initialized.
bool appRender();   // Update/Render a frame.
void appShutdown(); // Take down any textures, timers, sounds, etc...

// Return a null terminated string for each field.
char *appGetAuthor()        {return "Your Name Here";}
char *appGetName()          {return "Your App's Name Here";}
char *appGetDescription()   {return "This is an example description. "\
                                    "This entry makes a nice organic spiral "\
                                    "which paints colours against the background.";}
char *appGetTitleBar()      {return "Your App's TitleBar Here";}

// Title updated every half second on Win32 systems.
//char *appGetTitleBar()      {return "App Title Bar by Author [fps: ###]";}

RTexture texExample;

// May be useful to some people.
bool appPreInit() // Called before OpenGL has been initialized.
{
  return true;
}

// Startup Stuff.
bool appInit()	// Called right after the window is created, and OpenGL is initialized.
{
	RImage imgTmp;
	imgTmp.Create( "data/Fuzzy.bmp", imgLoadBMP );
	texExample.Create( &imgTmp); 
	imgTmp.Toast();

	// We want to use textures, so enable them.
	glEnable( GL_TEXTURE_2D );

	InitDCam();

	// If everything went well, then return true.
	// Returning false will cause the program to quit right away.
	return true;
}



// Draw all the scene related stuff.
bool appRender()
{
	// We like to use floating point seconds, just for kicks.
	double fTime = sys_Time/1000.f;

	// dynamic camera
	UpdateDCam();

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Use our wonderful fuzzy texture.
	texExample.Use();

//	glTranslatef(0, 0, fTime*10);

	glLineWidth(10);

	// Draw a spiraling ribbon.
	glBegin( GL_QUADS );

		//glColor3d(1, 0, 0);
		glTexCoord2d( 0, 0 );
		glVertex3d(-10, -10, 0);

		//glColor3d(0, 1, 0);
		glTexCoord2d( 1, 0 );
		glVertex3d(10, -10, 0);
		
		//glColor3d(0, 0, 1);
		glTexCoord2d( 1, 1 );
		glVertex3d(10, 10, 0);

		//glColor3d(0, 1, 0);
		glTexCoord2d( 0, 1 );
		glVertex3d(-10, 10, 0);


	glEnd();

	return true;
}

void appShutdown() // Called right after the window is destroyed.
{
	KillDCam();


	// We're finished with the texture.
	texExample.Toast();
}
