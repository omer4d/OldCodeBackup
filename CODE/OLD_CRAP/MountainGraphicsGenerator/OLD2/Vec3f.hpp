#ifndef _Vec3f_hpp_
#define _Vec3f_hpp_

#include "math.h"

#ifndef SQUARED
#define SQUARED(x) ((x) * (x))
#endif

struct Vec3f {
    float x, y, z;

    void init(float px, float py, float pz)
    {
        x = px, y = py, z = pz;
    }

    Vec3f(void)
    {
        init(0.0f, 0.0f, 0.0f);
    }

    Vec3f(float px, float py, float pz)
    {
        init(px, py, pz);
    }

    Vec3f& operator=(Vec3f const& rhs)
    {
        x = rhs.x, y = rhs.y;
        z = rhs.z;
        return *this;
    }

    Vec3f& operator+=(Vec3f const& rhs)
    {
        x += rhs.x, y += rhs.y;
        z += rhs.z;
        return *this;
    }

    Vec3f& operator-=(Vec3f const& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    Vec3f& operator*=(float k)
    {
        x *= k;
        y *= k;
        z *= k;
        return *this;
    }

    Vec3f operator+(Vec3f const& rhs) const
    {
        Vec3f res = *this;
        res += rhs;
        return res;
    }

    Vec3f operator-(Vec3f const& rhs) const
    {
        Vec3f res = *this;
        res -= rhs;
        return res;
    }

    Vec3f operator-(void) const
    {
        Vec3f res = *this;
        res.x = -x;
        res.y = -y;
        return res;
    }

    Vec3f operator*(float k) const
    {
        Vec3f res = *this;
        res *= k;
        return res;
    }

    float dot(Vec3f const& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    Vec3f cross(Vec3f const& param) const
    {
        Vec3f r;

        r.x = y * param.z - param.y * z;
        r.y = z * param.x - param.z * x;
        r.z = x * param.y - param.x * y;

        return r;
    }

    float squaredLength() const
    {
        return x * x + y * y + z * z;
    }

    float length(void) const
    {
        return sqrt(x * x + y * y + z * z);
    }
    
    void normalize(void)
    {
        float k = 1.0f / length();
        x *= k;
        y *= k;
        z *= k;
    }

    Vec3f unit(void) const
    {
        Vec3f u(x, y, z);
        u.normalize();
        return u;
    }
    
    static Vec3f normal(Vec3f const& a, Vec3f const& b, Vec3f const& c)
    {
       return (a - b).cross(c - b);
    }
    
    static Vec3f unitNormal(Vec3f const& a, Vec3f const& b, Vec3f const& c)
    {
       return (a - b).cross(c - b).unit();
    }

    static Vec3f blend(Vec3f const& a, Vec3f const& b, float k)
    {
        return Vec3f(a.x * k + b.x * (1.0f - k),
                     a.y * k + b.y * (1.0f - k),
                     a.z * k + b.z * (1.0f - k));
    }

    static float squaredDist(Vec3f const& a, Vec3f const& b)
    {
        Vec3f d = a - b;
        return d.dot(d);
    }
};

#endif
