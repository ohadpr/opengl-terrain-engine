//
// DynamicCamera.h
//
// DynamicCamera header file
//

#ifndef __DYNAMICCAMERA_H__
#define __DYNAMICCAMERA_H__

// includes
#include "base.h"

#include "vector.h"
#include "matrix.h"

// defs

// structs

// vars

// funcs/classes
class DynamicCamera
{
public:
	DynamicCamera(Vector &vecSrc);
	DynamicCamera();
	~DynamicCamera();

	void Update();

private:
	Vector	camSrc, camVel, camAng;
	float	angSpeed, moveSpeed;
	float	camFov, camRoll;
	Matrix	viewMat;
};


#endif //__DYNAMICCAMERA_H__