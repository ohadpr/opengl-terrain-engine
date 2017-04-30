//
// DynamicCamera.cpp
//
//
//

#include "DynamicCamera.h"

#include "YourCode.h"


// vars

// functions

DynamicCamera::DynamicCamera()
{
	DynamicCamera(Vector(0,0,0));
}

DynamicCamera::DynamicCamera(Vector &vecSrc)
{
	camSrc = vecSrc;
	camAng.Set(0, 0, 0);
	viewMat.Identity();

	camFov = 60.f;
	camRoll = 0;

	angSpeed = 0.1f;
	moveSpeed = 4.f;

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(camFov, (GLdouble)sys_glWidth/(GLdouble)sys_glHeight, 1.0, 1500000.0);
	
	glMatrixMode (GL_MODELVIEW);
}

DynamicCamera::~DynamicCamera()
{
}

void DynamicCamera::Update()
{
	float newAngSpeed = angSpeed * g_frameTime;
	if (GetAsyncKeyState(VK_RIGHT))
		camAng.x -= newAngSpeed;
	if (GetAsyncKeyState(VK_LEFT))
		camAng.x += newAngSpeed;

	if (GetAsyncKeyState(VK_UP))
		camAng.y -= newAngSpeed;
	if (GetAsyncKeyState(VK_DOWN))
		camAng.y += newAngSpeed;

	if (GetAsyncKeyState(VK_PRIOR))
		camAng.z -= newAngSpeed;
	if (GetAsyncKeyState(VK_HOME))
		camAng.z += newAngSpeed;

	Matrix	angRotMat(camAng.x, camAng.y, camAng.z);

	viewMat = angRotMat*viewMat;

	camAng *= expf(-2.0f*g_frameTime);

	camRoll += camAng.z*70;

	Vector	sideVec(viewMat.p[0], viewMat.p[1], viewMat.p[2]);
	Vector	upVec(viewMat.p[3], viewMat.p[4], viewMat.p[5]);
	Vector	dirVec(viewMat.p[6], viewMat.p[7], viewMat.p[8]);

	// forward-backward
	if (GetAsyncKeyState('A'))
		camVel += dirVec*moveSpeed;
	if (GetAsyncKeyState('Z'))
		camVel -= dirVec*moveSpeed;

	// up-down
	if (GetAsyncKeyState(VK_SUBTRACT))
		camVel += upVec*moveSpeed;
	if (GetAsyncKeyState(VK_ADD))
		camVel -= upVec*moveSpeed;

	// left-right
	if (GetAsyncKeyState(VK_NEXT))
		camVel += sideVec*moveSpeed;
	if (GetAsyncKeyState(VK_END))
		camVel -= sideVec*moveSpeed;

	camSrc += camVel * g_frameTime;
	camVel *= expf(-2.0f*g_frameTime);
	// setup opengl camera

	glLoadIdentity ();
	//glRotatef(camRoll, 0, 0, 1);
	/*gluLookAt(	camSrc.x,			camSrc.y,			camSrc.z,			// src
				camSrc.x+dirVec.x,	camSrc.y+dirVec.y,	camSrc.z+dirVec.z,	// dst
				0,					1,					0);// up*/


	float m[16] = {
		viewMat.p[0],viewMat.p[3],-viewMat.p[6],0,//-(viewMat.p[0]*camSrc.x + viewMat.p[3]*camSrc.y + viewMat.p[6]*camSrc.z),
		viewMat.p[1],viewMat.p[4],-viewMat.p[7],0,//-(viewMat.p[1]*camSrc.x + viewMat.p[4]*camSrc.y + viewMat.p[7]*camSrc.z),
		viewMat.p[2],viewMat.p[5],-viewMat.p[8],0,//-(viewMat.p[2]*camSrc.x + viewMat.p[5]*camSrc.y + viewMat.p[8]*camSrc.z),
		//0,0,0,1
		-(viewMat.p[0]*camSrc.x + viewMat.p[1]*camSrc.y + viewMat.p[2]*camSrc.z),
		-(viewMat.p[3]*camSrc.x + viewMat.p[4]*camSrc.y + viewMat.p[5]*camSrc.z),
		(viewMat.p[6]*camSrc.x + viewMat.p[7]*camSrc.y + viewMat.p[8]*camSrc.z),1
	};
	glLoadMatrixf(m);
}
