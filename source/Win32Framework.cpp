// Spinner.cpp : Defines the entry point for the application.
// Thanks goes out to Max Code for making this skeleton
// able to run with Voodoo2 OpenGL.

#include "Std.h"
#include "OGLCMainHeader.h"
#include "..\VC6Project\YourCode.h"

// These values are maintained by the OpenGL Challenge framework.
UInt32 sys_glWidth, sys_glHeight;
UInt8  sys_glBPP=0;
UInt32 sys_Time=0;

// Internal variables....
static bool   oglcFullscreen;
static GLsizei oglcWidth, oglcHeight;
static bool bReady;
static LARGE_INTEGER TimeFreq, TimeStart;

// Special identifier for doing title update.
#define TIMER_CODE 123

// Good 'ol generic drawing stuff.
LRESULT CALLBACK OGLCProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

// Usefull debugger thingy.
  const int kMaxDebugMessageLength = 1024;
  void DebugOut(const char *szFormat, ...)
  {
    va_list argList;
    char szBuffer[kMaxDebugMessageLength];

    va_start(argList, szFormat);
    _vsnprintf(szBuffer, kMaxDebugMessageLength, szFormat, argList);
    va_end(argList);

    OutputDebugString(szBuffer);
  }

// Mess with the display system.
bool ChangeDisplayMode()
{

  DEVMODE device_mode;

  memset(&device_mode, 0, sizeof(DEVMODE));

  device_mode.dmSize = sizeof(DEVMODE);

  device_mode.dmPelsWidth  = sys_glWidth;
  device_mode.dmPelsHeight = sys_glHeight;
  if( sys_glBPP )
  {
    device_mode.dmBitsPerPel = sys_glBPP;
    device_mode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
  } else
  {
    device_mode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT;
  }

  if( ChangeDisplaySettings(&device_mode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL )
    return true;  // Yay!
  else
    return false; // Booo...
}

void RestoreDisplayMode()
{
  ChangeDisplaySettings(0, 0);
}

// Select the pixel format for a given device context
void SetDCPixelFormat(HDC hDC)
{

  int nPixelFormat;

  static PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR),  // Size of this structure
    1,                              // Version of this structure
    PFD_DRAW_TO_WINDOW |    // Draw to Window (not to bitmap)
    PFD_SUPPORT_OPENGL |    // Support OpenGL calls in window
    PFD_DOUBLEBUFFER,       // Double buffered mode
    PFD_TYPE_RGBA,        // RGBA Color mode
    sys_glBPP,  // Want the display bit depth
    0,0,0,0,0,0,                    // Not used to select mode
    0,0,                            // Not used to select mode
    0,0,0,0,0,                      // Not used to select mode
    sys_glBPP,  // Size of depth buffer should match BPP.
    0,                    // Not used to select mode
    0,                    // Not used to select mode
    PFD_MAIN_PLANE,       // Draw in main plane
    0,                    // Not used to select mode
    0,0,0
  };                      // Not used to select mode

  // Choose a pixel format that best matches that described in pfd
  nPixelFormat = ChoosePixelFormat(hDC, &pfd);
  DebugOut( "PixelFormat Selected: %d\nBPP: %d\n", nPixelFormat, GetDeviceCaps(hDC, BITSPIXEL) );

  // Set the pixel format for the device context
  SetPixelFormat(hDC, nPixelFormat, &pfd);
}

void ChangeSize( GLsizei w, GLsizei h )
{
  // Prevent a divide by Zero
  if( h == 0 )
    h = 1;

  // Set viewport to our dimensions.
  glViewport( 0
	  , 0, w, h);
}

//-------------------------
// Good ol' creation code.
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
  // Say we're not ready to render yet.
  bReady = false;

  // Load up our system settings from OpenGLChallenge.INI
	oglcFullscreen = false;
	//oglcFullscreen = true;
	oglcWidth = 800;
	oglcHeight = 600;

	HDC hDC = GetDC( 0 ); // Get the screen's DC.
	sys_glBPP = GetDeviceCaps(hDC, BITSPIXEL);
	ReleaseDC( 0, hDC);

  // Command line can override.
  if (strstr(lpCmdLine, "-fullscreen"))
    oglcFullscreen = true;
  if (strstr(lpCmdLine, "-fullscreen=0"))
    oglcFullscreen = false;

  //-------------------------------------------------------------------
  // Preinitialize the app.
  if( appPreInit() == false )
  {
    MessageBox( NULL, "App returned false on PreInit(), we shouldn't really continue.", "Error", MB_OK | MB_ICONERROR );
    return FALSE; // Quit right away.
  }

  // Specify exactly what the app dimensions should be..
  sys_glWidth = oglcWidth;
  sys_glHeight = oglcHeight;


  // Setup the window class.
  WNDCLASS wc;
  HWND hWnd;
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc  = (WNDPROC)OGLCProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = hInstance;
  wc.hIcon = NULL;
  wc.hCursor = LoadCursor( NULL, IDC_ARROW );

  wc.hbrBackground = NULL;
  wc.lpszMenuName = NULL;
  wc.lpszClassName = "OGLC_Class";

  // Announce it to the Microsloth OS.
  if( RegisterClass( &wc ) == 0)
  {
    MessageBox( NULL, "Error creating the window class, right before creating the window.", "Fatal Blow", MB_OK | MB_ICONERROR );
    return FALSE; // Failed to create window, so just quit right away.
  }

  // Make it go.......
  if( oglcFullscreen == false )
  { // Default startup mode.
    hWnd = CreateWindow("OGLC_Class",
                        appGetTitleBar(),
                        WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS,
                        CW_USEDEFAULT, CW_USEDEFAULT,
                        oglcWidth, oglcHeight,
                        NULL, NULL,
                        hInstance,
                        NULL );
  } else
  { // alternate startup mode, to bring up full screen.
    hWnd = CreateWindow("OGLC_Class",
                        appGetTitleBar(),
                        WS_POPUP,
                        0, 0,
                        oglcWidth, oglcHeight,
                        NULL, NULL,
                        hInstance,
                        NULL );
  }

  if( hWnd == NULL )
  {
    MessageBox( NULL, "App failed to create the rendering window.", "Fatal Blow", MB_OK | MB_ICONERROR );
    return FALSE; // Failed to create window, so just quit right away.
  }

  // Make the app window visible...
  ShowWindow( hWnd, SW_SHOW );
  UpdateWindow( hWnd );

  // Initialize our system time.
  QueryPerformanceFrequency(&TimeFreq);
  QueryPerformanceCounter(&TimeStart);

  // Prepare the scene for rendering.
  if( appInit() == false )
  {
    MessageBox( hWnd, "App failed to initialize.", "Fatal Blow", MB_OK | MB_ICONERROR );
    return FALSE; // Failed init, so just quit right away.
  }

  // Turn on the title update timer.
  // - Put in .5 second timer for title update code.
  SetTimer( hWnd, TIMER_CODE, 500, 0 );

  // We're now ready.
  bReady = true;

  // Usual running around in circles bit...
  BOOL bGotMsg;
  MSG  msg;
  PeekMessage( &msg, NULL, 0U, 0U, PM_NOREMOVE );
  while( WM_QUIT != msg.message  )
  {
    // Use PeekMessage() if the app is active, so we can use idle time to
    // render the scene. Else, use GetMessage() to avoid eating CPU time.
    bGotMsg = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );

    if( bGotMsg )
    {
      // Translate and dispatch the message
      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
    else
    {
      // Get the current time, and update the time controller.
      LARGE_INTEGER TimeNow;
      QueryPerformanceCounter(&TimeNow);
      sys_Time = (UInt32)((TimeNow.QuadPart-TimeStart.QuadPart)*1000/TimeFreq.QuadPart);

      HDC hDC = GetDC( hWnd );

      // Draw all the scene related stuff.
      if( appRender() == false )
        DestroyWindow(hWnd); // False means quit.

      // Make it all visible at once!
      SwapBuffers( hDC );

      ReleaseDC(hWnd, hDC );
    }
  }

  // Not ready to render anymore.
  bReady = false;

  return msg.wParam;
}


// multi-texture
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB			= 0;
PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB		= 0;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB	= 0;

// compiled array vertex
PFNGLLOCKARRAYSEXTPROC			glLockArraysEXT				= 0;
PFNGLUNLOCKARRAYSEXTPROC		glUnlockArraysEXT			= 0;


int isExtensionSupported(const char *extension)
{
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	/* Extension names should not have spaces. */
	where = (GLubyte *) strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;
	extensions = glGetString(GL_EXTENSIONS);
	/* It takes a bit of care to be fool-proof about parsing the
	 OpenGL extensions string. Don't be fooled by sub-strings,
	 etc. */
	start = extensions;
	for (;;) 
	{
		where = (GLubyte *) strstr((const char *) start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return 1;
		start = terminator;
	}
	return 0;
}


void InitExtensions()
{
	// multi-texture
	if (!isExtensionSupported("GL_ARB_multitexture"))
		MessageBox(0, "ARB_multitexture is not supported on this device", "error", 0);

	glActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glActiveTextureARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
	glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");


	// compiled-vertex-arrays
	if (!isExtensionSupported("GL_EXT_compiled_vertex_array"))
		MessageBox(0, "EXT_compiled_vertex_array is not supported on this device", "error", 0);

	glLockArraysEXT = (PFNGLLOCKARRAYSEXTPROC) wglGetProcAddress("glLockArraysEXT");
	glUnlockArraysEXT = (PFNGLUNLOCKARRAYSEXTPROC) wglGetProcAddress("glUnlockArraysEXT");
}

//----------------------------------------------------
// Usual callback window proc message spud gun.
LRESULT CALLBACK OGLCProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  // Keep drawing stuff around.
  static HGLRC hRC;
  HDC hDC;

  switch( uMsg )
  {
  case WM_CREATE:
    // Remember our mSloth drawing context.
    hDC = GetDC( hWnd );

      if( oglcFullscreen )
      {
        ChangeDisplayMode();
      }

        // Select our precious pixel format.
      SetDCPixelFormat( hDC );

      // Yeppers, make something that OpenGL understands.
      hRC = wglCreateContext( hDC );
      wglMakeCurrent( hDC, hRC );

	  // opengl-extensions
	  InitExtensions();

    ReleaseDC( hWnd, hDC );
    break;

  case WM_DESTROY:
    // Nuke all applicable scene stuff.
    appShutdown();
    bReady = false;

    // Finally release all display context material from OpenGL.
    hDC = GetDC( hWnd );
      wglMakeCurrent( hDC, NULL );
      wglDeleteContext( hRC );
    ReleaseDC( hWnd, hDC );

    if( oglcFullscreen )
    {
      RestoreDisplayMode();
    }

    PostQuitMessage( 0 );
    break;

  case WM_SIZE:
    sys_glWidth = LOWORD(lParam);
    sys_glHeight = HIWORD(lParam);
    ChangeSize( sys_glWidth, sys_glHeight );
    break;

  case WM_KEYDOWN:
    if (wParam == VK_ESCAPE) {
      DestroyWindow(hWnd);
    }
    break;

  case WM_TIMER:
    if((wParam == TIMER_CODE ) && (bReady))
    {
      // Set our window title, from app callback.
      SetWindowText(hWnd, appGetTitleBar() );
    }
    break;


  default:
    return DefWindowProc( hWnd, uMsg, wParam, lParam );
  }

  return 0;
}
