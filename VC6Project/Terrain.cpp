//
// Terrain.cpp
//
//
//

#include "Terrain.h"

#include "YourCode.h"

// vars

// functions
Terrain::Terrain(char *pHeightMapName)
{
	// load the terrain image
	imgHeightMap.Create(pHeightMapName, imgLoadBMP);

	// store the image localy
	mapXRes = imgHeightMap.GetWidth();
	mapYRes = imgHeightMap.GetHeight();

	// check image format
	if (imgHeightMap.GetBPP() != 8)
		MessageBox(0, "Height Map is not 8bpp", "error", 0);
}

Terrain::~Terrain()
{
	delete pVertexData;
}

void Terrain::SetParams(Vector &origin, Vector &dimen, float density, float waterHeight)
{
	this->origin = origin;
	this->dimen = dimen;
	this->waterHeight = waterHeight;

	// calculate the resolution of our vertex-map
	vertXRes = (dword)(mapXRes * density);
	vertYRes = (dword)(mapYRes * density);

	if (vertXRes < 3)
		vertXRes = 3;

	if (vertYRes < 3)
		vertYRes = 3;

	vertXRes = 1 + ((vertXRes-1)>>1)*2;
	vertYRes = 1 + ((vertYRes-1)>>1)*2;

	pVertexData = new VertexData[vertXRes*vertYRes];


	BuildTerrain();

	
	// construct sky-box vertices

	// vertices
	Vector	center, radius;
	const float rad = dimen.x*12;

	center = origin + (dimen * 0.5f);
	center.y = origin.y+dimen.y*waterHeight;
	radius.Set(rad, rad, rad);

	// top
	skyBoxVerts[0].Set(center.x-radius.x, center.y+radius.y, center.z+radius.z);
	skyBoxVerts[1].Set(center.x+radius.x, center.y+radius.y, center.z+radius.z);
	skyBoxVerts[2].Set(center.x+radius.x, center.y+radius.y, center.z-radius.z);
	skyBoxVerts[3].Set(center.x-radius.x, center.y+radius.y, center.z-radius.z);

	// bottom
	skyBoxVerts[4].Set(center.x-radius.x, center.y, center.z+radius.z);
	skyBoxVerts[5].Set(center.x+radius.x, center.y, center.z+radius.z);
	skyBoxVerts[6].Set(center.x+radius.x, center.y, center.z-radius.z);
	skyBoxVerts[7].Set(center.x-radius.x, center.y, center.z-radius.z);
}

void Terrain::SetTextures(	RTexture *pTexTerrain, RTexture *pTexDetail,
							RTexture *pTexSkyBox,
							RTexture *pTexCaustics)
{
	this->pTexTerrain = pTexTerrain;
	this->pTexDetail = pTexDetail;
	this->pTexSkyBox = pTexSkyBox;
	this->pTexCaustics = pTexCaustics;
}


float GetPixel(byte *pData, dword xRes, dword yRes, float s, float t)
{
	dword	uInt, vInt;
	float	uFrac, vFrac;

	byte	colors[4];
	float	retCol;

	// find integer coordinates
	uInt = (dword)s;
	vInt = (dword)t;

	uFrac = s - uInt;
	vFrac = t - vInt;

	// bilinear filtering
	colors[0] = pData[vInt*xRes + uInt];
	colors[1] = pData[vInt*xRes + ((uInt+1)%xRes)];
	colors[2] = pData[((vInt+1)%yRes)*xRes + ((uInt+1)%xRes)];
	colors[3] = pData[((vInt+1)%yRes)*xRes + uInt];

	retCol =	colors[0] * ((1-uFrac)*(1-vFrac)) +
				colors[1] * (uFrac*(1-vFrac)) +
				colors[2] * (uFrac*vFrac) +
				colors[3] * ((1-uFrac)*vFrac);

	return retCol;
}

void Terrain::BuildTerrain()
{
	float		 minY, deltaY, yScale;
	dword		 x, y;
	float		 stepX, stepY;
	float		 sCoord, tCoord;
	float		 rDensX, rDensY;
	VertexData	*pVertexDataPtr;
	byte		*pHeightMap = (byte*)imgHeightMap.GetData();

	minY = origin.y;
	deltaY = dimen.y - minY;
	yScale = deltaY / 256.f;

	rDensX = 1.f / vertXRes;
	rDensY = 1.f / vertYRes;

	pVertexDataPtr = pVertexData;

	// run over our heightmap
	for (y=0; y<vertYRes; y++)
	{
		stepY = (float)y*rDensX;

		// texture coordinates for this row
		tCoord = stepY*mapYRes;

		for (x=0; x<vertXRes; x++)
		{
			stepX = (float)x*rDensY;

			// texture coordinates for this quad
			sCoord = stepX*mapXRes;

			// get pixel from height map
			float	pixelHeight;

			// implement bilinear filtering, to eliminate jiggels			
			pixelHeight = GetPixel(pHeightMap, mapXRes, mapYRes, sCoord, tCoord);

//			if (pixelHeight/256.f < waterHeight) {
//				pixelHeight = waterHeight*256 - 2;
//			}

			// fetch a value from the height map
			pVertexDataPtr->pos.Set(	origin.x+stepX*dimen.x,
										minY + pixelHeight*yScale,
										origin.z+stepY*dimen.z );

			pVertexDataPtr->s = stepX;
			pVertexDataPtr->t = stepY;
			pVertexDataPtr->u = stepX*20.f;
			pVertexDataPtr->v = stepY*20.f;

/*			pVertexDataPtr->col[0] = 1.f;
			pVertexDataPtr->col[1] = 1.f;
			pVertexDataPtr->col[2] = 1.f;
			pVertexDataPtr->col[3] = 1.f;*/

			pVertexDataPtr++;
		}
	}

	// no need for the terrain image anymore
	imgHeightMap.Toast();

	// opengl arrays

	// vertices
	glVertexPointer(3, GL_FLOAT, sizeof(VertexData), pVertexData);
	glEnableClientState(GL_VERTEX_ARRAY);

	// color
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(VertexData), (float*)pVertexData+3);
	glEnableClientState(GL_COLOR_ARRAY);

	// 1'st texture coordinates
	glClientActiveTextureARB(GL_TEXTURE0_ARB); 
	glTexCoordPointer(2, GL_FLOAT, sizeof(VertexData), (float*)pVertexData+4);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	// 2'nd texture coordinates
	glClientActiveTextureARB(GL_TEXTURE1_ARB); 
	glTexCoordPointer(2, GL_FLOAT, sizeof(VertexData), (float*)pVertexData+6);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glClientActiveTextureARB(GL_TEXTURE0_ARB); 
	

	glLockArraysEXT(0, vertXRes*vertYRes);
	GLenum blet;
	blet = glGetError();



	// generate an index-array
	pIndexArray = new word[vertYRes*vertXRes*2];
	word *pTempPtr = pIndexArray;


	for (y=0; y<vertYRes-1; y++)
	{
		for (dword x=0; x<vertXRes; x++)
		{
			*pTempPtr++ = y*vertXRes + x;
			*pTempPtr++ = (y+1)*vertXRes + x;
		}
	}


  
//	glLockArraysEXT(0, count);                // extension GL_EXT_compiled_vertex_array
//  glUnlockArraysEXT;                       // extension GL_EXT_compiled_vertex_array
//  glDisableClientState(GL_COLOR_ARRAY);
//  glDisableClientState(GL_VERTEX_ARRAY);
}


void Terrain::Render()
{

	// enable a clipping plane (so that the terrain that extrudes out of the water
	// doesn't get drawn
	double plane_eqn[4] = { 0, -1, 0, origin.y + dimen.y*waterHeight}; 
	glClipPlane( GL_CLIP_PLANE0, plane_eqn ); 
	glEnable( GL_CLIP_PLANE0 ); 


	// vertical-flip everything
	glTranslatef(0, 2*waterHeight*dimen.y, 0);
	glScalef(1, -1, 1);

		// reverse culling order
		glCullFace(GL_BACK);
		
		RenderTerrain();
		RenderSkyBox();

		// restore correct culling
		glCullFace(GL_FRONT);

	// re-flip vertically
	glScalef(1, -1, 1);
	glTranslatef(0, -2*waterHeight*dimen.y, 0);

	// disable extra clipping-plane
	glDisable( GL_CLIP_PLANE0 ); 



	// render water
	RenderWater();

	// render the terrain
	RenderTerrain();

	// render the skybox (will set its own textures)
	RenderSkyBox();
}


void Terrain::RenderTerrain()
{
	dword	y;

	dword	drawDetail = !GetAsyncKeyState('D');
	dword	wireFrame = GetAsyncKeyState('W');

	// first texture
	glActiveTextureARB(GL_TEXTURE0_ARB); 
	pTexTerrain->Use();
	glEnable(GL_TEXTURE_2D);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT); 

	// second texture
	if (drawDetail) {
		glActiveTextureARB(GL_TEXTURE1_ARB); 
		pTexDetail->Use();
		glEnable(GL_TEXTURE_2D);
		//glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD_SIGNED_EXT); 
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT); 
		glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_ADD_SIGNED_EXT); 
	}


	for (y=0; y<vertYRes-1; y++) {
		glDrawElements(wireFrame ? GL_LINE_STRIP : GL_TRIANGLE_STRIP, vertXRes*2, GL_UNSIGNED_SHORT, pIndexArray+y*vertXRes*2);
	}

	// disable second texture
	if (drawDetail) {
		glDisable(GL_TEXTURE_2D);
		
		// set first texture to be 'simple'
		glActiveTextureARB(GL_TEXTURE0_ARB); 
	}
}


void Terrain::RenderSkyBox()
{
	// skybox

	const float u0 = 0.5f / 256.f,
				u1 = (256.f-0.5f) / 256.f,
				v0 = 0.5f / 256.f,
				v1 = (256.f-0.5f) / 256.f;

	const dword dataOfs[5][4] = {
		{7,6,2,3},
		{1,2,6,5},
		{0,1,5,4},
		{4,7,3,0},
		{3,2,1,0}
	};

	const float mapping[4][2] = { {u0, v1}, {u1,v1}, {u1,v0}, {u0, v0} };

	dword i;


	for (i=0; i<5; i++) {
		pTexSkyBox[i].Use();
		glBegin( GL_QUADS );

		sdword j;

		for (j=0; j<4; j++) {
			glTexCoord2d(mapping[j][0], mapping[j][1]);
			glVertex3fv(skyBoxVerts[dataOfs[i][j]].GetPtr());
		}

		glEnd();
	}


	// front
	// right
	// back
	// left
	// top
}


void Terrain::RenderWater()
{
	float waterTile = 2*60.f;

#define	WATER_STRIP_LEN	15
#define WATER_STRIP_LEN_1	(WATER_STRIP_LEN-1)

	Vector	temp;
	Vector	dU, dV;

	dU = skyBoxVerts[5] - skyBoxVerts[4];
	dV = skyBoxVerts[7] - skyBoxVerts[4];


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(1,1,1,0.45f);

	pTexSkyBox[5].Use();


	for (dword j=0; j<WATER_STRIP_LEN; j++) {
		glBegin( GL_QUAD_STRIP );
		for (dword i=0; i<WATER_STRIP_LEN+1; i++) {
			for (sdword dy=1; dy>=0; dy--) {
				temp = skyBoxVerts[4];
				temp += dU * ((float)i / WATER_STRIP_LEN);
				temp += dV * ((float)(j+dy) / WATER_STRIP_LEN);
				glTexCoord2d((j+dy)*waterTile/WATER_STRIP_LEN, i*waterTile/WATER_STRIP_LEN);
				glVertex3fv(temp.GetPtr());
			}
		}
		glEnd();
	}

//	glBlendFunc(GL_SRC_ALPHA, GL_DST_COLOR);//GL_SRC_ALPHA);
	glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);//GL_SRC_ALPHA);


	float anim = sys_Time*0.00003f;

	for (j=0; j<WATER_STRIP_LEN; j++) {
		glBegin( GL_QUAD_STRIP );
		for (dword i=0; i<WATER_STRIP_LEN+1; i++) {
			for (sdword dy=1; dy>=0; dy--) {
				temp = skyBoxVerts[4];
				temp += dU * ((float)i / WATER_STRIP_LEN);
				temp += dV * ((float)(j+dy) / WATER_STRIP_LEN);
				glTexCoord2d(anim+((j+dy)*waterTile/WATER_STRIP_LEN), anim+(i*waterTile/WATER_STRIP_LEN));
				glVertex3fv(temp.GetPtr());
			}
		}
		glEnd();
	}

	glDisable(GL_BLEND);


/*	pTexSkyBox[5].Use();
	glBegin( GL_QUADS );
		glTexCoord2d(0, waterTile);
		glVertex3fv(skyBoxVerts[4].GetPtr());
		glTexCoord2d(waterTile, waterTile);
		glVertex3fv(skyBoxVerts[5].GetPtr());
		glTexCoord2d(waterTile, 0);
		glVertex3fv(skyBoxVerts[6].GetPtr());
		glTexCoord2d(0, 0);
		glVertex3fv(skyBoxVerts[7].GetPtr());
	glEnd();*/
}