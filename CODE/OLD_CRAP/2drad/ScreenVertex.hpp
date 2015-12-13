#ifndef _ScreenVertex_hpp_
#define _ScreenVertex_hpp_

#include "Vertex.hpp"
#include "Col3f.hpp"

struct ScreenVertex {
       float y;
       Col3f col;
       float invDepth;

       ScreenVertex()
       {
        y = 0;
       }
       
       ScreenVertex(Vertex const& vertex, int h)
       {
        this->y = (1 - vertex.pos.y) * (h - 1);
        
        if(vertex.pos.x > 0.000001)
        this->invDepth = 1 / vertex.pos.x;
        else
        this->invDepth = 1 / 0.000001;
        
        this->col = vertex.col;
       }
       
       ScreenVertex operator+=(ScreenVertex const& other)
       {
        this->col += other.col;
        this->invDepth += other.invDepth;
       }
       
       static ScreenVertex calcDelta(ScreenVertex const& min, ScreenVertex const& max, int steps)
       {
        ScreenVertex d;
        float invSteps = 1.0 / steps;
        
        d.col = (max.col - min.col) * invSteps;
        d.invDepth = (max.invDepth - min.invDepth) * invSteps;
        
        return d;
       }
};

#endif
