#include "texgen.h"


void Image::GetPixel(dword u, dword v, Pixel *pixel)
{
	dword ofs = ((u%xRes) + (v%yRes)*xRes)*3;

	pixel->r = pData[ofs+0];
	pixel->g = pData[ofs+1];
	pixel->b = pData[ofs+2];
}

void Image::PutPixel(dword u, dword v, Pixel *pixel)
{
	dword ofs = (u + v*xRes)*3;

	pData[ofs+0] = pixel->r;
	pData[ofs+1] = pixel->g;
	pData[ofs+2] = pixel->b;
}

float Image::GetHeight(sdword u, sdword v)
{
	dword ofs = ((u < 0 ? 0 : (u >= xRes-1 ? xRes-1 : u)) + (v < 0 ? 0 : (v >= yRes-1 ? yRes-1 : v))*xRes)*3;

	return pData[ofs] / 256.f;
}


void Image::Load(char *pFileName)
{
    JPEG_CORE_PROPERTIES	image;
    	
	ZeroMemory( &image, sizeof( JPEG_CORE_PROPERTIES ) );

	ijlInit( &image );
	image.JPGFile = pFileName;
	ijlRead( &image, IJL_JFILE_READPARAMS );

	// Set the JPG color space ... this will always be
	// somewhat of an educated guess at best because JPEG
	// is "color blind" (i.e., nothing in the bit stream
	// tells you what color space the data was encoded from).
	// However, in this example we assume that we are
	// reading JFIF files which means that 3 channel images
	// are in the YCbCr color space and 1 channel images are
	// in the Y color space.
	switch(image.JPGChannels)
	{
		case 1:
			image.JPGColor    = IJL_G;
			image.DIBChannels = 3;
			image.DIBColor    = IJL_BGR;
			break;

		case 3:
			image.JPGColor    = IJL_YCBCR;
			image.DIBChannels = 3;
			image.DIBColor    = IJL_BGR;
			break;

		default:
			// This catches everything else, but no
			// color twist will be performed by the IJL.
			image.DIBColor = (IJL_COLOR)IJL_OTHER;
			image.JPGColor = (IJL_COLOR)IJL_OTHER;
			image.DIBChannels = image.JPGChannels;
			break;
	}

	image.DIBWidth    = image.JPGWidth;
	image.DIBHeight   = image.JPGHeight;
	image.DIBPadBytes = IJL_DIB_PAD_BYTES(image.DIBWidth,image.DIBChannels);

	int imageSize = (image.DIBWidth * image.DIBChannels + image.DIBPadBytes) * image.DIBHeight;

	BYTE *imageData = new BYTE[ imageSize ];

	image.DIBBytes = imageData;

	ijlRead( &image, IJL_JFILE_READWHOLEIMAGE );

	ijlFree( &image );

	if(image.DIBColor == IJL_BGR) {
		for (int i=0; i<image.DIBHeight*image.DIBWidth; i++) {
			byte t;

			t = imageData[i*3+0];
			imageData[i*3+0] = imageData[i*3+2];
			imageData[i*3+2] = t;
		}
	}

	// return data
	pData = imageData;
	xRes = image.DIBWidth;
	yRes = image.DIBHeight;
}



void Image::Save(char *pFileName)
{
    JPEG_CORE_PROPERTIES image;
    ZeroMemory( &image, sizeof( JPEG_CORE_PROPERTIES ) );

	ijlInit( &image );

	// Setup DIB
	image.DIBWidth         = xRes;
	image.DIBHeight        = yRes;
	image.DIBBytes         = pData;

	// Setup JPEG
	image.JPGFile          = pFileName;
	image.JPGWidth         = xRes;
	image.JPGHeight        = yRes;

	image.DIBColor       = IJL_RGB;
	image.DIBChannels    = 3;
	image.DIBPadBytes    = IJL_DIB_PAD_BYTES(image.DIBWidth,3);
	image.JPGColor       = IJL_YCBCR;
	image.JPGChannels    = 3;
	image.JPGSubsampling = IJL_411;

	ijlWrite( &image, IJL_JFILE_WRITEWHOLEIMAGE );

    ijlFree( &image );
}
