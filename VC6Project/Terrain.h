//
// Terrain.h
//
// Terrain header file
//

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

// includes
#include "base.h"

#include "vector.h"

// defs

#define N_CAUSTICS_TEX	32

// structs

struct VertexData	// (3+2+2+4)*4 = 44 bytes
{
	Vector	pos;	// position
	byte	col[4];	// color
	float	s, t;	// texture coordinates
	float	u, v;	// detail-texture coordinates
};

struct Vector2D
{
	dword	x, y;
};

// vars

// funcs/classes
class Terrain
{
public:
	Terrain(char *pHeightMapName);
	~Terrain();
	
	void SetParams(	Vector &origin, 
					Vector &dimen, 
					float density, 
					float waterHeight);
	void SetTextures(	RTexture *pTexTerrain, RTexture *pTexDetail,
						RTexture *pTexSkyBox,
						RTexture *pTexCaustics);
	void BuildTerrain();

	void Render();


private:

	void PutTerrainVertex(dword i, dword j);
	void RenderTerrain();
	void RenderSkyBox();
	void RenderWater();


	RImage		 imgHeightMap;			// our height map
	dword		 mapXRes, mapYRes;		// height map dimensions

	Vector		 origin, dimen;			// terrain origin in space, and its dimensions
	dword		 vertXRes, vertYRes;	// vertex buffer size
	float		 waterHeight;			// height of water (0..1) 

	VertexData	*pVertexData;			// the vertex data for the terrain

	word		*pIndexArray;

	Vector		 skyBoxVerts[12];		// vertices for skybox

	// textures
	RTexture	*pTexTerrain, *pTexDetail;	// ground textures
	RTexture	*pTexSkyBox;				// sky-box textures
	RTexture	*pTexCaustics;				// caustics texture
};


#endif //__TERRAIN_H__