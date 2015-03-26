#pragma once

#include <cassert>
#include <algorithm>

//
// Array classes
//
// Taken from gfxTools.h 1.2

template<class T>
class array {
protected:
    T *data;
    int len;
public:
    array() { data=NULL; len=0; }
    array(int l) { init(l); }
    ~array() { free(); }


    inline void init(int l);
    inline void free();
    inline void resize(int l);

    inline T& ref(int i);
    inline T& operator[](int i) { return data[i]; }
    inline T& operator()(int i) { return ref(i); }
    inline int length() { return len; }
    inline int maxLength() { return len; }
};

template<class T>
inline void array<T>::init(int l)
{
    data = new T[l];
    len = l;
}

template<class T>
inline void array<T>::free()
{
    if( data )
    {
        delete[] data;
        data = NULL;
    }
}

template<class T>
inline T& array<T>::ref(int i)
{
    assert( data );
    assert( i>=0 && i<len );
    return data[i];
}

template<class T>
inline void array<T>::resize(int l)
{
    T *old = data;
    data = new T[l];
    data = (T *)memcpy(data,old,std::min(len,l)*sizeof(T));
    len = l;
    delete[] old;
}

template<class T>
class array2 {
protected:
    T *data;
    int w, h;
public:
    array2() { data=NULL; w=h=0; }
    array2(int w, int h) { init(w,h); }
    ~array2() { free(); }

    inline void init(int w, int h);
    inline void free();

    inline T& ref(int i, int j);
    inline T& operator()(int i,int j) { return ref(i,j); }
    inline int width() { return w; }
    inline int height() { return h; }
};

template<class T>
inline void array2<T>::init(int width,int height)
{
    w = width;
    h = height;
    data = new T[w*h];
}

template<class T>
inline void array2<T>::free()
{
    if( data )
    {
        delete[] data;
        data = NULL;
    }
}

template<class T>
inline T& array2<T>::ref(int i, int j)
{
    assert( data );
    assert( i>=0 && i<w );
    assert( j>=0 && j<h );
    return data[j*w + i];
}

