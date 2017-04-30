//
// YourCode.h
//
// YourCode header file
//

#ifndef __YourCode_H__
#define __YourCode_H__

// includes
#include "base.h"

// defs

// structs

// vars

// funcs/classes

// Functions implemented by the author in YourCode.cpp
bool appPreInit();  // Initialize before OpenGL has been initialized.
bool appInit();     // Initialize after OpenGL has been initialized.
bool appRender();   // Update/Render a frame. Return false to exit.
void appShutdown(); // Take down any textures, timers, sounds, etc...

// Return a null terminated string.
char *appGetTitleBar();      // The title bar refreshes every half sec.


extern float g_frameTime;


#endif //__YourCode_H__