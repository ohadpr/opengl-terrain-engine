//
// Ending.cpp
//
//
//

#include "Ending.h"

#include "YourCode.h"

// vars

// functions
Ending::Ending()
{
}

Ending::~Ending()
{
}



void Ending::Render()
{


	// a square object (not total 3d square, depth is smaller)
	static float squareRad = 40;
	static float squareDepth = 10;

	static float squareVerts[8][3] = {
		{-squareRad, -squareRad, +squareDepth},	// 0
		{+squareRad, -squareRad, +squareDepth},	// 1
		{+squareRad, +squareRad, +squareDepth},	// 2
		{-squareRad, +squareRad, +squareDepth},	// 3
		{-squareRad, -squareRad, -squareDepth},	// 4
		{+squareRad, -squareRad, -squareDepth},	// 5
		{+squareRad, +squareRad, -squareDepth},	// 6
		{-squareRad, +squareRad, -squareDepth}	// 7
	};

	static dword squareQuads[6][4] = {
		{0,1,2,3},	// front
		{4,0,3,7},	// left
		{7,3,2,6},	// bottom
		{6,2,1,5},	// right
		{4,5,1,0},	// top
		{3,2,6,7}	// bottom
	};

	static dword nSquareQuads = 6;

	// draw a single square
	glBegin(GL_QUADS);

		for (dword i=0; i<nSquareQuads; i++) {
			for (dword j=0; j<4; j++) {
				glColor4f(1, 1, 1, 0.5);
				glVertex3fv(squareVerts[ squareQuads[i][j] ]);
			}
		}

	glEnd();
}
