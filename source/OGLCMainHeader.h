//-------------------------------------------------------------------
//  File:   OGLCMainHeader.h
//  Time:   July 15, 2000
//  Author:   Aaron Hilton (OTri Studios)
//  Comments: OpenGL Challenge application functions and variables
//            used to bind the platform specific parts to the visual
//            code.
//-------------------------------------------------------------------
#ifndef __OGLCMainHeader_h_
#define __OGLCMainHeader_h_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// These values are maintained by the platform
// side of the OpenGL Challenge framework. Do Not Modify.
extern UInt32 sys_glWidth, sys_glHeight;
extern UInt8  sys_glBPP;
extern UInt32 sys_Time; // Time counted in milliseconds (1/1000 sec).

// Win32Framework.cpp, opengl-extensions
extern PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB;
extern PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB;


extern PFNGLLOCKARRAYSEXTPROC			glLockArraysEXT;
extern PFNGLUNLOCKARRAYSEXTPROC			glUnlockArraysEXT;


#endif // __OGLCMainHeader_h_
