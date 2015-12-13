/* 
 * File:   Frame.hpp
 * Author: Stas
 *
 * Created on June 19, 2012, 5:36 AM
 */

#ifndef FRAME_HPP
#define	FRAME_HPP

#include <map>

#include "Vec2f.hpp"

class Pivot {
    Vec2f pos, dir;
    
public:
    Pivot()
    {
    }
    
    Pivot(Vec2f const& pPos, Vec2f const& pDir = Vec2f(1, 0)): pos(pPos), dir(pDir)
    {
    }
    
    Vec2f const& getPos() const
    {
        return pos;
    }
};

class Frame {
    typedef std::map<std::string, Pivot> PivotMap;
    
    Vec2f min, max, texMin, texMax;
    PivotMap pivotMap;
    
public:
    Frame(Vec2f const& texMin, Vec2f const& texMax, Vec2f const& min, Vec2f const& max)
    {
        this->min = min;
        this->max = max;
        this->texMin = texMin;
        this->texMax = texMax;
    }

    void addPivot(std::string name, Pivot const& p)
    {
        pivotMap[name] = p;
    }
    
    Pivot const* getPivot(std::string name) const
    {
        PivotMap::const_iterator i = pivotMap.find(name);
        
        if(i != pivotMap.end())
            return &i->second;
        
        else
            return NULL;
    }
    
    Vec2f const& getTexMax() const
    {
        return texMax;
    }

    Vec2f const& getTexMin() const
    {
        return texMin;
    }
    
    Vec2f const& getMin() const
    {
        return min;
    }

    int getHeight() const
    {
        return max.y - min.y;
    }

    int getWidth() const
    {
        return max.x - min.x;
    }
    
    bool contains(Vec2f const& p) const
    {
        return p.x >= min.x && p.y >= min.y && p.x <= max.x && p.y <= max.y;
    }
};

#endif	/* FRAME_HPP */

