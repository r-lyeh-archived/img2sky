#ifndef MASK_INCLUDED // -*- C++ -*-
#define MASK_INCLUDED

class ImportMask
{

public:
    int width, height;

    ImportMask() { width=0; height=0; }


    virtual real apply(int /*x*/, int /*y*/, real val) { return val; }
};


class RealMask : public ImportMask
{
    real *data;

public:
    RealMask(int width, int height);

    inline real& ref(int x, int y);
    real apply(int x, int y, real val) { return ref(x,y) * val; }
};


inline RealMask::RealMask(int w, int h)
{
    width = w;
    height = h;
    data = (real *)calloc(w*h, sizeof(real));
}

inline real& RealMask::ref(int i, int j)
{
#ifdef SAFETY
	assert(i>=0); assert(j>=0); assert(i<width); assert(j<height);
#endif

	return data[j*width + i];
}

extern RealMask *readMask(std::istream&);

#endif
