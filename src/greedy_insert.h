#ifndef GREEDYINSERT_INCLUDED // -*- C++ -*-
#define GREEDYINSERT_INCLUDED

#include "Heap.h"
#include "Subdivision.h"
#include "Map.h"

class TrackedTriangle : public Triangle
{
    //
    // candidate position
    int sx, sy;


public:
    TrackedTriangle(Edge *e, int t=NOT_IN_HEAP)
	: Triangle(e, t)
    {
    }

    void update(Subdivision&);


    void setCandidate(int x,int y, real) { sx=x; sy=y; }
    void getCandidate(int *x, int *y) { *x=sx; *y=sy; }
};


class Candidate
{
public:

    int x, y;
    real import;

    Candidate() { import = -HUGE; }

    void consider(int sx, int sy, real i)
    {
	if( i > import )
	{
	    x = sx;
	    y = sy;
	    import = i;
	}
    }
};


class GreedySubdivision : public Subdivision
{
    Heap *heap;
    unsigned int count;

protected:

    Map *H;

    Triangle *allocFace(Edge *e);

    void compute_plane(Plane&, Triangle&, Map&);

    void scan_triangle_line(Plane& plane,
			    int y, real x1, real x2,
			    Candidate& candidate);

public:
    GreedySubdivision(Map *map);

    array2<char> is_used;

    Edge *select(int sx, int sy, Triangle *t=NULL);

    Map& getData() { return *H; }

    void scanTriangle(TrackedTriangle& t);
    int greedyInsert();

    unsigned int pointCount() { return count; }
    real maxError();
    real rmsError();
    real eval(int x,int y);
};

//
// These are the possible values of is_used(x,y):
#define DATA_POINT_UNUSED  0
#define DATA_POINT_USED    1

#endif
