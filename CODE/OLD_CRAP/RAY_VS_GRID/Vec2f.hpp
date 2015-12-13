#ifndef _Vec2f_hpp_
#define _Vec2f_hpp_

#include <cmath>
#include "Mat9f.hpp"

#ifndef SQUARED
#define SQUARED(x) ((x) * (x))
#endif

#ifndef M_PI
#define M_PI 3.14159265359
#endif 

struct Vec2f {
    float x, y;

    void init(float px, float py)
    {
        x = px, y = py;
    }
    
    Vec2f(void)
    {
        init(0.0f, 0.0f);
    }

    Vec2f(float px, float py)
    {
        init(px, py);
    }

    // *************
    // * Operators *
    // *************

    Vec2f& operator=(Vec2f const& rhs)
    {
        x = rhs.x, y = rhs.y;
        return *this;
    }

    Vec2f& operator+=(Vec2f const& rhs)
    {
        x += rhs.x, y += rhs.y;
        return *this;
    }

    Vec2f& operator-=(Vec2f const& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    Vec2f& operator*=(float k)
    {
        x *= k;
        y *= k;
        return *this;
    }

    Vec2f operator+(Vec2f const& rhs) const
    {
        Vec2f res = *this;
        res += rhs;
        return res;
    }

    Vec2f operator-(Vec2f const& rhs) const
    {
        Vec2f res = *this;
        res -= rhs;
        return res;
    }

    Vec2f operator-(void) const
    {
        Vec2f res = *this;
        res.x = -x;
        res.y = -y;
        return res;
    }

    Vec2f operator*(float k) const
    {
        Vec2f res = *this;
        res *= k;
        return res;
    }
    
    // ********************
    // * Matrix Operators *
    // ********************
    
    Vec2f operator*(Mat9f const& mat) const
    {
        Vec2f res = *this;
        
        float w = 1.0 / (x * mat(0, 2) + y * mat(1, 2) + 1 * mat(2, 2));
        res.x = (x * mat(0, 0) + y * mat(1, 0) + 1 * mat(2, 0));// * w;
        res.y = (x * mat(0, 1) + y * mat(1, 1) + 1 * mat(2, 1)) * w;
        
        return res;
    }
    
    Vec2f& operator*=(Mat9f const& mat)
    {
        *this = *this * mat;
        return *this;
    }
    
    // *********
    // * Other *
    // *********

    float dot(Vec2f const& rhs) const
    {
        return x * rhs.x + y * rhs.y;
    }

    float cross(Vec2f const& param) const
    {
        return x * param.y - y * param.x;
    }

    float length(void) const
    {
        return sqrt(x * x + y * y);
    }
    
    float squaredLength() const
    {
        return x * x + y * y;
    }

    float approxLength(void) const
    {
        float dx = fabs(x), dy = fabs(y);
        if(dy > dx) return 0.41f * dx + 0.941246f * dy;
        return 0.41f * dy + 0.941246f * dx;
    }

    Vec2f normal(void) const
    {
        Vec2f n(-y, x);
        return n;
    }

    void normalize(void)
    {
        float k = 1.0f / length();
        x *= k;
        y *= k;
    }

    Vec2f unit(void) const
    {
        Vec2f u(x, y);
        u.normalize();
        return u;
    }

    Vec2f unitNormal(void) const
    {
        Vec2f n = normal();
        return n.unit();
    }

    Vec2f rotated(float alpha) const
    {
        Vec2f r;
        float c = cos(alpha), s = sin(alpha);

        r.x = x * c - y * s;
        r.y = x * s + y * c;

        return r;
    }

    // Transform the vector from the base defined by a, o, b to the base in which they are defined.

    Vec2f fromBase(Vec2f const& a, Vec2f const& o, Vec2f const& b) const
    {
        return o + a * x + b * y;
    }

    static float cosAng(Vec2f const& a, Vec2f const& o, Vec2f const& b)
    {
        return(a - o).unit().dot((b - o).unit());
    }

    static float ang(Vec2f const& a, Vec2f const& b, float l1, float l2)
    {
        float dot = a.normal().dot(b);
        float l1l2 = l1 * l2;
        float cosa;

        if(l1l2 < 0.00001 || (cosa = a.dot(b) / l1l2) > 1.0) return 0.0;

        bool side = dot > 0;
        float ang = acos(cosa);
        return side ? ang : 2.0 * M_PI - ang;
    }

    static Vec2f blend(Vec2f const& a, Vec2f const& b, float k)
    {
        return Vec2f(a.x * k + b.x * (1.0f - k), a.y * k + b.y * (1.0f - k));
    }

    static float squaredDist(Vec2f const& a, Vec2f const& b)
    {
        Vec2f d = a - b;
        return d.dot(d);
    }
};

#endif
