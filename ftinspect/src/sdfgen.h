
#ifndef __SDF_GEN_H__
#define __SDF_GEN_H__

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include FT_BBOX_H

// bitmap implementation taken from http://www.codersnotes.com/notes/signed-distance-fields/
// for outline msdfgen is used https://github.com/Chlumsky/msdfgen

#include <vector>

struct SDFData {
	std::vector<float> Buffer;
	unsigned int Width	= 0u;
	unsigned int Height	= 0u;
};

class SDFGen {
public:
	static SDFData GenerateSDFFromBitmap(const unsigned char * buff, unsigned int sizeX, unsigned int sizeY);
	static SDFData GenerateSDFFromOutline(FT_Outline * outline, unsigned int sizeX, unsigned int sizeY);
};

#endif //__SDF_GEN_H__
