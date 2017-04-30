#ifndef MY_JPG_INTERFACE_H
#define MY_JPG_INTERFACE_H

#include "texgen.h"

struct Pixel
{
	byte r, g, b;
};


class Image
{
public:

	void Load(char *pFileName);
	void Save(char *pFileName);

	void GetPixel(dword u, dword v, Pixel *pixel);
	void PutPixel(dword u, dword v, Pixel *pixel);

	float GetHeight(sdword u, sdword v);

	dword	 xRes, yRes;
	byte	*pData;
};


#endif // MY_JPG_INTERFACE_H