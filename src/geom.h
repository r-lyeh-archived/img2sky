#pragma once

#include <assert.h>
#include <math.h>

#define EPS 1e-6
#define EPS2 (EPS*EPS)
#include "Vec2.h"
#include "Vec3.h"

enum Axis {X, Y, Z, W};
enum Side {Left=-1, On=0, Right=1};

class Labelled {
public:
    int token;
};

// Here we define some useful geometric functions

//
// triArea returns TWICE the area of the oriented triangle ABC.
// The area is positive when ABC is oriented counterclockwise.
inline double triArea(const Vec2& a, const Vec2& b, const Vec2& c)
{
    return (b[X] - a[X])*(c[Y] - a[Y]) - (b[Y] - a[Y])*(c[X] - a[X]);
}

inline bool ccw(const Vec2& a, const Vec2& b, const Vec2& c)
{
    return triArea(a, b, c) > 0;
}

inline bool rightOf(const Vec2& x, const Vec2& org, const Vec2& dest)
{
    return ccw(x, dest, org);
}

inline bool leftOf(const Vec2& x, const Vec2& org, const Vec2& dest)
{
    return ccw(x, org, dest);
}

// Returns True if the point d is inside the circle defined by the
// points a, b, c. See Guibas and Stolfi (1985) p.107.
//
inline bool inCircle(const Vec2& a, const Vec2& b, const Vec2& c,
			const Vec2& d)
{
    return (a[0]*a[0] + a[1]*a[1]) * triArea(b, c, d) -
	   (b[0]*b[0] + b[1]*b[1]) * triArea(a, c, d) +
 	   (c[0]*c[0] + c[1]*c[1]) * triArea(a, b, d) -
	   (d[0]*d[0] + d[1]*d[1]) * triArea(a, b, c) > EPS;
}


class Plane {
public:

    double a, b, c;

    Plane() { }
    Plane(const Vec3& p, const Vec3& q, const Vec3& r) { init(p,q,r); }

    // find the plane z=ax+by+c passing through three points p,q,r
    void init(const Vec3& p, const Vec3& q, const Vec3& r) {
        // We explicitly declare these (rather than putting them in a
        // Vector) so that they can be allocated into registers.
        double ux = q[X]-p[X], uy = q[Y]-p[Y], uz = q[Z]-p[Z];
        double vx = r[X]-p[X], vy = r[Y]-p[Y], vz = r[Z]-p[Z];
        double den = ux*vy-uy*vx;

        a = (uz*vy - uy*vz)/den;
        b = (ux*vz - uz*vx)/den;
        c = p[Z] - a*p[X] - b*p[Y];
    }

    double operator()(double x,double y) { return a*x + b*y + c; }
    double operator()(int x, int y)  { return a*x + b*y + c; }
};

class Line {

private:
    double a, b, c;

public:
    Line(const Vec2& p, const Vec2& q) {
    	Vec2 t = q - p;
    	double l = t.length();
    	assert(l!=0);
    	a =   t[Y] / l;
    	b = - t[X] / l;
    	c = -(a*p[X] + b*p[Y]);
    }

    double eval(const Vec2& p) const {
    	return (a*p[X] + b*p[Y] + c);
    }

    Side classify(const Vec2& p) const {
    	double d = eval(p);

    	if( d < -EPS )
    	    return Left;
    	else if( d > EPS )
    	    return Right;
    	else
    	    return On;
    }

    Vec2 intersect(const Line& l) const {
    	Vec2 p;
    	intersect(l, p);
    	return p;
    }

    void intersect(const Line& l, Vec2& p) const {
    	double den = a*l.b - b*l.a;
    	assert(den!=0);
    	p[X] = (b*l.c - c*l.b)/den;
    	p[Y] = (c*l.a - a*l.c)/den;
    }

    template<typename ostream>
    inline friend ostream& operator<<( ostream &out, const Line &self ) {
        return out << "Line(a=" << self.a << " b=" << self.b << " c=" << self.c << ")";
    }
};
