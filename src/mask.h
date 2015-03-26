#ifndef MASK_INCLUDED // -*- C++ -*-
#define MASK_INCLUDED

class ImportMask
{

public:
    int width, height;

    ImportMask() { width=0; height=0; }


    virtual double apply(int /*x*/, int /*y*/, double val) { return val; }
};


class realMask : public ImportMask
{
    double *data;

public:
    realMask(int width, int height);

    inline double& ref(int x, int y);
    double apply(int x, int y, double val) { return ref(x,y) * val; }
};


inline realMask::realMask(int w, int h)
{
    width = w;
    height = h;
    data = (double *)calloc(w*h, sizeof(double));
}

inline double& realMask::ref(int i, int j)
{
#ifdef SAFETY
	assert(i>=0); assert(j>=0); assert(i<width); assert(j<height);
#endif

	return data[j*width + i];
}

extern realMask *readMask(std::istream&);

#endif
