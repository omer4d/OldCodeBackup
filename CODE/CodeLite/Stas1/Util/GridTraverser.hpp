/* 
 * File:   GridTraverser.hpp
 * Author: Stas
 *
 * Created on February 15, 2013, 5:29 PM
 */

#ifndef GRIDTRAVERSER_HPP
#define	GRIDTRAVERSER_HPP

#include "Vec2f.hpp"
#include "Ray.hpp"

class GridTraverser {
    static float sgn(float f)
    {
        return f < 0 ? -1 : 1;
    }
    
    int cols, rows;
    int x, y;
    int stepX, stepY;
    float tMaxX, tMaxY;
    float tDeltaX, tDeltaY;
    float t;
    
public:
    GridTraverser(int cols, int rows, Ray const& ray)
    {
        this->cols = cols;
        this->rows = rows;
        
        this->x = floor(ray.orig.x);
        this->y = floor(ray.orig.y);
        this->stepX = sgn(ray.dir.x);
        this->stepY = sgn(ray.dir.y);
        
        this->tMaxX = (floor(ray.orig.x + (this->stepX > 0)) - ray.orig.x) / ray.dir.x;
        this->tMaxY = (floor(ray.orig.y + (this->stepY > 0)) - ray.orig.y) / ray.dir.y;

        this->tDeltaX = this->stepX / ray.dir.x;
        this->tDeltaY = this->stepY / ray.dir.y;
        this->t = 0.f;
    }
    
    int getX() const
    {
        return x;
    }
    
    int getY() const
    {
        return y;
    }
    
    float getDist() const
    {
        return t;
    }
    
    bool next()
    {
        if(tMaxX < tMaxY)
        {
            t = tMaxX;
            tMaxX += tDeltaX;
            x += stepX;
        }
        
        else
        {
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
        }
        
        return x >= 0 && y >= 0 && x < cols && y < rows;
    }
};

#endif	/* GRIDTRAVERSER_HPP */

