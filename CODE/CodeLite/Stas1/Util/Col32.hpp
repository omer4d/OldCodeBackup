/* 
 * File:   Col32.hpp
 * Author: Stas
 *
 * Created on February 21, 2013, 9:57 AM
 */

#ifndef COL32_HPP
#define	COL32_HPP

#include <inttypes.h>

struct Col32 {
    int32_t value; // BGRA

    Col32(int v = 0)
    {
        value = v;
    }

    Col32(int r, int g, int b, int a = 0)
    {
        value = b | (g << 8) | (r << 16) | (a << 24);
    }

    static int clamp255(float n)
    {
        if(n > 255) return 255;
        else if(n < 0) return 0;
        else return int(n);
    }

    bool rgbCmp(Col32 const& other) const
    {
        return(value & 0x00FFFFFF) == (other.value & 0x00FFFFFF);
    }

    void setr(int r)
    {
        ((char*) &value)[2] = r;
    }

    void setg(int g)
    {
        ((char*) &value)[1] = g;
    }

    void setb(int b)
    {
        ((char*) &value)[0] = b;
    }

    void seta(int a)
    {
        ((char*) &value)[3] = a;
    }

    int getb() const
    {
        return value & 0x000000FF;
    }

    int getg() const
    {
        return(value >> 8) & 0x000000FF;
    }

    int getr() const
    {
        return(value >> 16) & 0x000000FF;
    }

    int geta() const
    {
        return(value >> 24);
    }

    // *************
    // * Operators *
    // *************
    
    Col32& operator+=(Col32 const& other)
    {
        setr(clamp255(getr() + other.getr()));
        setg(clamp255(getg() + other.getg()));
        setb(clamp255(getb() + other.getb()));

        return *this;
    }

    Col32& operator-=(Col32 const& other)
    {
        setr(clamp255(getr() - other.getr()));
        setg(clamp255(getg() - other.getg()));
        setb(clamp255(getb() - other.getb()));
        
        return *this;
    }

    Col32& operator*=(float k)
    {
        setr(clamp255(getr() * k));
        setg(clamp255(getg() * k));
        setb(clamp255(getb() * k));

        return *this;
    }

    Col32& operator*=(Col32 const& other)
    {
        setr(clamp255(getr() * other.getr() / 255.f));
        setg(clamp255(getg() * other.getg() / 255.f));
        setb(clamp255(getb() * other.getb() / 255.f));
    }

    Col32 operator+(Col32 const& other) const
    {
        Col32 temp = *this;
        temp += other;

        return temp;
    }

    Col32 operator-(Col32 const& other) const
    {
        Col32 temp = *this;
        temp -= other;

        return temp;
    }

    Col32 operator*(float k) const
    {
        Col32 temp = *this;
        temp *= k;
        return temp;
    }

    Col32 operator*(Col32 const& other) const
    {
        Col32 temp = *this;
        temp *= other;
        return temp;
    }

    static Col32 gradient(Col32 const& a, Col32 const& b, float k)
    {
        return a + (b - a) * k;
    }
};

#endif	/* COL32_HPP */

