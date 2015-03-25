#ifndef MAP_INCLUDED // -*- C++ -*-
#define MAP_INCLUDED

#include <stdlib.h>
#include <iostream>

#include "Geom.h"

class Map
{
public:

    int width;
    int height;
    int depth;  // in bits

    real min, max;

    real operator()(int i, int j) { return eval(i,j); }
    real operator()(real i, real j) { return eval((int)i,(int)j); }
    real eval(real i, real j) { return eval((int)i,(int)j); }
    virtual real eval(int i, int j) = 0;

    virtual void rawRead(std::istream&) = 0;
    virtual void textRead(std::istream&) = 0;
    virtual void *getBlock() { return NULL; }
    virtual void findLimits();
};

extern Map *readTGA(const char *filename);
extern Map *readPGM(std::istream&);



template<class T>
class DirectMap : public Map
{
    T *data;

protected:
    inline T& ref(int i,int j)
    {
#ifdef SAFETY
	assert(i>=0); assert(j>=0); assert(i<width); assert(j<height);
#endif

	return data[j*width + i];
    }

public:

    DirectMap(int width, int height);

    real eval(int i, int j) { return (real)ref(i,j); }
    void *getBlock() { return data; }

    void rawRead(std::istream&);
    void textRead(std::istream&);
};

typedef DirectMap<unsigned char>  ByteMap;
typedef DirectMap<unsigned short> ShortMap;
typedef DirectMap<unsigned int>   WordMap;
typedef DirectMap<real>           RealMap;



template<class T>
DirectMap<T>::DirectMap(int w, int h)
{
    width = w;
    height = h;
    depth = sizeof(T) << 3;

    data = (T *)calloc(w*h, sizeof(T));
}

template<class T>
void DirectMap<T>::rawRead(std::istream& in)
{
    char *loc = (char *)data;
    int target = width*height*sizeof(T);

    while( target>0 )
    {
	in.read(loc, target);
	target -= in.gcount();
	loc += in.gcount();
    }
}

template<class T>
void DirectMap<T>::textRead(std::istream& in)
{
    for(int j=0;j<height;j++)
	for(int i=0;i<width;i++)
	{
	    real val;
	    in >> val;
	    ref(i,j) = (T)val;
	}
}





#endif
