#ifndef _Vertex_hpp_
#define _Vertex_hpp_

#include "Vec2f.hpp"
#include "Col3f.hpp"
#include "Line.hpp"

struct Vertex {
       Vec2f pos;
       Col3f col;
       
       Vertex()
       {
       }
       
       static Vertex blend(Vertex const& a, Vertex const& b, float k)
       {
        Vertex r;
        
        r.pos = a.pos * (1 - k) + b.pos * k;
        r.col = a.col * (1 - k) + b.col * k; 
        
        return r;
       }
       
       static bool clip(Vertex& v1, Vertex& v2, Line const& clipper)
       {
        Vec2f n = (clipper.b - clipper.a).normal();
        float d1 = (v1.pos - clipper.a).dot(n);
        float d2 = (v2.pos - clipper.a).dot(n);
        
        if((d1 < 0 && d2 > 0) || (d2 < 0 && d1 > 0))
        {
         float k = fabs(d1) / (fabs(d1) + fabs(d2));
         
         if(d1 < 0) v1 = Vertex::blend(v1, v2, k);
         else       v2 = Vertex::blend(v1, v2, k);
         
         return true;
        }
        
        else if(d1 <= 0 && d2 <= 0) return false;
        return true;
       }
};

#endif
