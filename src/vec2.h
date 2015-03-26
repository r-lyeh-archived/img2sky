#pragma once

class Vec2 {
protected:
    double elt[2];

    inline void copy(const Vec2& v);

public:
    // Standard constructors
    Vec2(double x=0, double y=0) { elt[0]=x; elt[1]=y; }
    Vec2(const Vec2& v) { copy(v); }
    Vec2(const double *v) { elt[0]=v[0]; elt[1]=v[1]; }
    Vec2& clone() const { return *(new Vec2(elt[0], elt[1])); }

    // Access methods
    double& operator()(int i)             { return elt[i]; }
    const double& operator()(int i) const { return elt[i]; }
    double& operator[](int i)             { return elt[i]; }
    const double& operator[](int i) const { return elt[i]; }

    // Assignment methods
    inline Vec2& operator=(const Vec2& v);
    inline Vec2& operator+=(const Vec2& v);
    inline Vec2& operator-=(const Vec2& v);
    inline Vec2& operator*=(double s);
    inline Vec2& operator/=(double s);

    // Arithmetic methods
    inline Vec2 operator+(const Vec2& v) const;
    inline Vec2 operator-(const Vec2& v) const;
    inline Vec2 operator-() const;

    inline Vec2 operator*(double s) const;
    inline Vec2 operator/(double s) const;
    inline double operator*(const Vec2& v) const;

    // Input/Output methods
    template<typename ostream>
    inline friend ostream& operator<<(ostream& out, const Vec2& v) {
        return out << "[" << v[0] << " " << v[1] << "]";
    }

    template<typename istream>
    inline friend istream& operator>>(istream& in, Vec2& v) {
        return in >> "[" >> v[0] >> v[1] >> "]";
    }

    // Additional vector methods
    inline double length();
    inline double norm();
    inline double norm2();

    inline double unitize();

    inline int operator==(const Vec2& v) const
    {
	return (*this - v).norm2() < EPS2;
    }
};

inline void Vec2::copy(const Vec2& v)
{
    elt[0]=v.elt[0]; elt[1]=v.elt[1];
}

inline Vec2& Vec2::operator=(const Vec2& v)
{
    copy(v);
    return *this;
}

inline Vec2& Vec2::operator+=(const Vec2& v)
{
    elt[0] += v[0];
    elt[1] += v[1];
    return *this;
}

inline Vec2& Vec2::operator-=(const Vec2& v)
{
    elt[0] -= v[0];
    elt[1] -= v[1];
    return *this;
}

inline Vec2& Vec2::operator*=(double s)
{
    elt[0] *= s;
    elt[1] *= s;
    return *this;
}

inline Vec2& Vec2::operator/=(double s)
{
    elt[0] /= s;
    elt[1] /= s;
    return *this;
}

///////////////////////

inline Vec2 Vec2::operator+(const Vec2& v) const
{
    Vec2 w(elt[0]+v[0], elt[1]+v[1]);
    return w;
}

inline Vec2 Vec2::operator-(const Vec2& v) const
{
    Vec2 w(elt[0]-v[0], elt[1]-v[1]);
    return w;
}

inline Vec2 Vec2::operator-() const
{
    return Vec2(-elt[0], -elt[1]);
}

inline Vec2 Vec2::operator*(double s) const
{
    Vec2 w(elt[0]*s, elt[1]*s);
    return w;
}

inline Vec2 Vec2::operator/(double s) const
{
    Vec2 w(elt[0]/s, elt[1]/s);
    return w;
}

inline double Vec2::operator*(const Vec2& v) const
{
    return elt[0]*v[0] + elt[1]*v[1];
}

inline double Vec2::length()
{
    return norm();
}

inline double Vec2::norm()
{
    return sqrt(elt[0]*elt[0] + elt[1]*elt[1]);
}

inline double Vec2::norm2()
{
    return elt[0]*elt[0] + elt[1]*elt[1];
}

inline double Vec2::unitize()
{
    double l=norm();
    if( l!=1.0 )
	(*this)/=l;
    return l;
}
