#pragma once

class Vec3 {
protected:
    double elt[3];

    inline void copy(const Vec3& v);

public:
    // Standard constructors
    Vec3(double x=0, double y=0, double z=0) { elt[0]=x; elt[1]=y; elt[2]=z; }
    Vec3(const Vec2& v, double z) { elt[0]=v[0]; elt[1]=v[1]; elt[2]=z; }
    Vec3(const Vec3& v) { copy(v); }
    Vec3(const double *v) { elt[0]=v[0]; elt[1]=v[1]; elt[2]=v[2]; }

    // Access methods
    double& operator()(int i)             { return elt[i]; }
    const double& operator()(int i) const { return elt[i]; }
    double& operator[](int i)             { return elt[i]; }
    const double& operator[](int i) const { return elt[i]; }

    // Assignment methods
    inline Vec3& operator=(const Vec3& v);
    inline Vec3& operator+=(const Vec3& v);
    inline Vec3& operator-=(const Vec3& v);
    inline Vec3& operator*=(double s);
    inline Vec3& operator/=(double s);

    // Arithmetic methods
    inline Vec3 operator+(const Vec3& v) const;
    inline Vec3 operator-(const Vec3& v) const;
    inline Vec3 operator-() const;

    inline Vec3 operator*(double s) const;
    inline Vec3 operator/(double s) const;
    inline double operator*(const Vec3& v) const;
    inline Vec3 operator^(const Vec3& v) const;


    // Input/Output methods
    template<typename ostream>
    inline friend ostream& operator<<(ostream& out, const Vec3& v) {
        return out << "[" << v[0] << " " << v[1] << " " << v[2] << "]";
    }

    template<typename istream>
    inline friend istream& operator>>(istream& in, Vec3& v) {
        return in >> "[" >> v[0] >> v[1] >> v[2] >> "]";
    }

    // Additional vector methods
    inline double length();
    inline double norm();
    inline double norm2();

    inline double unitize();
};


inline void Vec3::copy(const Vec3& v)
{
    elt[0]=v.elt[0]; elt[1]=v.elt[1]; elt[2]=v.elt[2];
}

inline Vec3& Vec3::operator=(const Vec3& v)
{
    copy(v);
    return *this;
}

inline Vec3& Vec3::operator+=(const Vec3& v)
{
    elt[0] += v[0];
    elt[1] += v[1];
    elt[2] += v[2];
    return *this;
}

inline Vec3& Vec3::operator-=(const Vec3& v)
{
    elt[0] -= v[0];
    elt[1] -= v[1];
    elt[2] -= v[2];
    return *this;
}

inline Vec3& Vec3::operator*=(double s)
{
    elt[0] *= s;
    elt[1] *= s;
    elt[2] *= s;
    return *this;
}

inline Vec3& Vec3::operator/=(double s)
{
    elt[0] /= s;
    elt[1] /= s;
    elt[2] /= s;
    return *this;
}

///////////////////////

inline Vec3 Vec3::operator+(const Vec3& v) const
{
    Vec3 w(elt[0]+v[0], elt[1]+v[1], elt[2]+v[2]);
    return w;
}

inline Vec3 Vec3::operator-(const Vec3& v) const
{
    Vec3 w(elt[0]-v[0], elt[1]-v[1], elt[2]-v[2]);
    return w;
}

inline Vec3 Vec3::operator-() const
{
    return Vec3(-elt[0], -elt[1], -elt[2]);
}

inline Vec3 Vec3::operator*(double s) const
{
    Vec3 w(elt[0]*s, elt[1]*s, elt[2]*s);
    return w;
}

inline Vec3 Vec3::operator/(double s) const
{
    Vec3 w(elt[0]/s, elt[1]/s, elt[2]/s);
    return w;
}

inline double Vec3::operator*(const Vec3& v) const
{
    return elt[0]*v[0] + elt[1]*v[1] + elt[2]*v[2];
}

inline Vec3 Vec3::operator^(const Vec3& v) const
{
    Vec3 w( elt[1]*v[2] - v[1]*elt[2],
	   -elt[0]*v[2] + v[0]*elt[2],
	    elt[0]*v[1] - v[0]*elt[1] );
    return w;
}

inline double Vec3::length()
{
    return norm();
}

inline double Vec3::norm()
{
    return sqrt(elt[0]*elt[0] + elt[1]*elt[1] + elt[2]*elt[2]);
}

inline double Vec3::norm2()
{
    return elt[0]*elt[0] + elt[1]*elt[1] + elt[2]*elt[2];
}

inline double Vec3::unitize()
{
    double l=norm();
    if( l!=1.0 )
	(*this)/=l;
    return l;
}

