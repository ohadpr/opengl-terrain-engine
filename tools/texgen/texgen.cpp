#include "texgen.h"

#define PI	3.1415f

// input
char *heightMap = "textures\\heightmap.jpg";
//char *heightMap = "textures\\crytek-heightmap.jpg";


#define N_TEXTURES	5
char *textureNames[N_TEXTURES] = {	
									// Height Textures
									"textures\\Texture1.jpg",
									"textures\\LAND-Sandly copy.jpg",
									"textures\\GRASS-Mossy copy.jpg",
									"textures\\SNOW-Soft copy.jpg",

									// Slope Textures
									"textures\\Texture3 copy.jpg"};

// height dispersing
#define N_HEIGHT_TEXTURES	4
float	textureLimits[N_HEIGHT_TEXTURES+1] = {0, 0.25f, 0.4f, 0.8f, 1.f};

#define HEIGHT_TRANSITION_RANGE	0.05f


// slope differentiation (gotta' love this english shit)
#define SLOPE_LIM	0.65f
#define SLOPE_TRANSITION_RANGE	0.1f


#define SIZE_FAC	1.f
char *outTexName = "final.jpg";



// forward
float GetPixel(Image *pImg, float s, float t);

inline float sigmoid(float x, float X0, float A)
{
	return 1.f / (1+expf(A*(x-X0)));
}


// real stuff
int main(int argc, char* argv[])
{
	Image	imgHeightmap;
	Image	imgTextures[N_TEXTURES];

	printf("\nTexGen, ohad 2001\n\n");

	// load heightmap
	printf("Loading Heightmap : %s\n", heightMap);
	imgHeightmap.Load(heightMap);

	// load textures
	for (int i=0; i<N_TEXTURES; i++) {
		printf("Loading Texture #%d : %s\n", i+1, textureNames[i]);
		imgTextures[i].Load(textureNames[i]);
	}
	printf("\n");


	// generate texture
	Image	imgFinal;

	imgFinal.xRes = imgHeightmap.xRes*SIZE_FAC;
	imgFinal.yRes = imgHeightmap.yRes*SIZE_FAC;
	imgFinal.pData = new byte[imgFinal.xRes*imgFinal.yRes*3];


	// do the job
	for (dword y=0; y<imgFinal.yRes; y++) {
		for (dword x=0; x<imgFinal.xRes; x++) {
			// get height at this point
			float	height;

			//height = imgHeightmap.GetHeight(x, y);
			height = GetPixel(&imgHeightmap, x/SIZE_FAC, y/SIZE_FAC);

			// find where we are
			dword i;
			for (i=0; i<N_HEIGHT_TEXTURES; i++) {
				if (height >= textureLimits[i] && height <= textureLimits[i+1]) {
					// we found the limit
					break;
				}
			}

			// fetch color from texture
			Pixel	finalPixel;

			// is this point only 1 texture ?
			if ( (i == 0 && height <= textureLimits[i+1]-HEIGHT_TRANSITION_RANGE) ||
				 (i == N_HEIGHT_TEXTURES-1 && height >= textureLimits[i]+HEIGHT_TRANSITION_RANGE) ||
				 (height >= textureLimits[i]+HEIGHT_TRANSITION_RANGE && height <= textureLimits[i+1]-HEIGHT_TRANSITION_RANGE)) {

				// yes, cool
				imgTextures[i].GetPixel(x, y, &finalPixel);
			} else {
				// trnasition

				// low-limit
				if (height <= textureLimits[i]+HEIGHT_TRANSITION_RANGE) {
					
					Pixel	a, b;
					float	fA, fB;

					imgTextures[i-1].GetPixel(x, y, &b);
					imgTextures[i].GetPixel(x, y, &a);

//					fA = 0.5f+sinf((height-textureLimits[i]) * 0.5f*PI / TRANSITION_RANGE)*0.5f;
//					fB = 1.f-fA;
					fB = sigmoid(height, textureLimits[i], 0.5*6.5f/HEIGHT_TRANSITION_RANGE);
					fA = 1.f - fB;

					finalPixel.r = (byte)((a.r * fA) + (b.r * fB));
					finalPixel.g = (byte)((a.g * fA) + (b.g * fB));
					finalPixel.b = (byte)((a.b * fA) + (b.b * fB));
				} else {
					// top-limit

					Pixel	a, b;
					float	fA, fB;

					imgTextures[i].GetPixel(x, y, &a);
					imgTextures[i+1].GetPixel(x, y, &b);

//					fA = 0.5f+sinf((textureLimits[i+1]-height) * 0.5f*PI / TRANSITION_RANGE) * 0.5f;
//					fB = 1.f-fA;
					fA = sigmoid(height, textureLimits[i+1], 0.5*6.5f/HEIGHT_TRANSITION_RANGE);
					fB = 1.f - fA;


					finalPixel.r = (byte)((a.r * fA) + (b.r * fB));
					finalPixel.g = (byte)((a.g * fA) + (b.g * fB));
					finalPixel.b = (byte)((a.b * fA) + (b.b * fB));
				}
			}

			// we've dispersed textures along the height-path
			// now check if this is a unique slope

			// calculate slope for this point

			float heights[4];

			heights[0] = GetPixel(&imgHeightmap, (x+1)/SIZE_FAC, (y)/SIZE_FAC);
			heights[1] = GetPixel(&imgHeightmap, (x-1)/SIZE_FAC, (y)/SIZE_FAC);
			heights[2] = GetPixel(&imgHeightmap, (x)/SIZE_FAC, (y+1)/SIZE_FAC);
			heights[3] = GetPixel(&imgHeightmap, (x)/SIZE_FAC, (y-1)/SIZE_FAC);

			float slopeX = 160.f * (heights[0] - heights[1]) / (2.f*1000.f/(256.f*SIZE_FAC));
			float slopeY = 160.f * (heights[2] - heights[3]) / (2.f*1000.f/(256.f*SIZE_FAC));
			float slopeZ = 1.f;

			float normalZ = 1.f/(float)sqrt(slopeX * slopeX + slopeY*slopeY + slopeZ*slopeZ);

			// are entirely inside high-slope ?
			if (normalZ < SLOPE_LIM) {
				imgTextures[N_TEXTURES-1].GetPixel(x, y, &finalPixel);
			} else if (normalZ < SLOPE_LIM+SLOPE_TRANSITION_RANGE) {
				Pixel	b;
				float	fA, fB;

//				fA = 1.f-sinf((SLOPE_LIM+SLOPE_TRANSITION_RANGE-normalZ) * 0.5f*PI / SLOPE_TRANSITION_RANGE);
//				fB = 1.f-fA;
				fB = sigmoid(normalZ, SLOPE_LIM, .25*6.5f/SLOPE_TRANSITION_RANGE);
				fA = 1.f - fB;


				imgTextures[N_TEXTURES-1].GetPixel(x, y, &b);

				finalPixel.r = (byte)((finalPixel.r * fA) + (b.r * fB));
				finalPixel.g = (byte)((finalPixel.g * fA) + (b.g * fB));
				finalPixel.b = (byte)((finalPixel.b * fA) + (b.b * fB));
			}

			imgFinal.PutPixel(x, y, &finalPixel);
		}

		printf( "Progess : %3.2f%c\r", y*100.f/(imgFinal.yRes-1), '%');
	}


	printf("\n\nWriting : %s", outTexName);
	imgFinal.Save(outTexName);

	printf("  -  Done!\n\n");

	return 0;
}


float GetPixel(Image *pImg, float s, float t)
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

	uInt = ( uInt + 1 ) % pImg->xRes;
	vInt = ( vInt + 1 ) % pImg->yRes;

	// bilinear filtering, for red only, cause all channels are the same
	colors[0] = pImg->pData[3*(vInt*pImg->xRes + uInt)];
	colors[1] = pImg->pData[3*(vInt*pImg->xRes + ((uInt+1)%pImg->xRes))];
	colors[2] = pImg->pData[3*(((vInt+1)%pImg->yRes)*pImg->xRes + ((uInt+1)%pImg->xRes))];
	colors[3] = pImg->pData[3*(((vInt+1)%pImg->yRes)*pImg->xRes + uInt)];

	retCol =	colors[0] * ((1-uFrac)*(1-vFrac)) +
				colors[1] * (uFrac*(1-vFrac)) +
				colors[2] * (uFrac*vFrac) +
				colors[3] * ((1-uFrac)*vFrac);

	return retCol / 256.f;
}
