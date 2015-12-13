/* 
 * File:   Ray.hpp
 * Author: Stas
 *
 * Created on June 30, 2012, 10:26 AM
 */

#ifndef RAY_HPP
#define	RAY_HPP

#include "Vec2f.hpp"

struct Ray {
    Vec2f orig, dir;
    
    Ray()
    {
        
    }

    Ray(Vec2f const& orig, Vec2f const& dir)
    {
        this->orig = orig;
        this->dir = dir;
    }
    
    void initFromSegment(Vec2f const& src, Vec2f const& dest)
    {
        orig = src;
        dir = (dest - src).unit();
    }

    float dist(Vec2f const& p) const
    {
        if(dir.x > 0.01 || dir.x < -0.01)
            return(p.x - orig.x) / dir.x;
        else
            return(p.y - orig.y) / dir.y;
    }

    Vec2f point(float k) const
    {
        return orig + dir * k;
    }

    bool intersectLine(float& res, Vec2f const& v1, Vec2f const& v2) const
    {
        Vec2f n = dir.normal();
        float d1 = (v1 - orig).dot(n);
        float d2 = (v2 - orig).dot(n);

        if((d1 < 0 && d2 >= 0) || (d2 < 0 && d1 >= 0))
        {
            float k = fabs(d1) / (fabs(d1) + fabs(d2)), q = dist(v1 + (v2 - v1) * k);

            if(q >= 0)
            {
                res = q;
                return true;
            }
        }

        return false;
    }
    
    bool intersectSegment(float& res, Vec2f const& v1, Vec2f const& v2) const
    {
        Vec2f n = dir.normal();
        float d1 = (v1 - orig).dot(n);
        float d2 = (v2 - orig).dot(n);

        if((d1 < 0 && d2 >= 0) || (d2 < 0 && d1 >= 0))
        {
            float k = fabs(d1) / (fabs(d1) + fabs(d2)), q = dist(v1 + (v2 - v1) * k);

            if(q >= 0 && k >= 0 && k <= 1)
            {
                res = q;
                return true;
            }
        }

        return false;
    }
};

#endif	/* RAY_HPP */

