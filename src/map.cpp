#include <math.h>

#include "main.h"
#include "geom.h"
#include "map.h"
#include "image.h"

void Map::findLimits()
{
    min = HUGE;
    max = -HUGE;

    for(int i=0;i<width;i++)
	for(int j=0;j<height;j++)
	{
	    double val = eval(i,j);

	    if( val<min ) min = val;
	    if( val>max ) max = val;

	}
}

Map *readTGA(const char *filename)
{
	TGA = new TGAFile;
	TGA->ReadTGA(filename);

	if (TGA->GetWidth() == 0) {
		std::cout << " (error: invalid image file)\r[FAIL]\n";
		return NULL;
	}

	Map *map = new ByteMap(TGA->GetWidth(), TGA->GetHeight());

	unsigned char *pMap = (unsigned char *)map->getBlock();

	for (unsigned long y = 0; y < TGA->GetHeight(); y++)
		for (unsigned long x = 0; x < TGA->GetWidth(); x++)
			*(pMap++) = TGA->GetPixelA(x, y).GrayValue();

	map->findLimits();

	return map;
}
