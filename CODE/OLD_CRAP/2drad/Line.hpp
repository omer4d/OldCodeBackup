#ifndef _Line_hpp_
#define _Line_hpp_

#include "Vec2f.hpp"

struct Line {
       Vec2f a, b;
       
       Line()
       {
       }
       
       Line(Vec2f const& a, Vec2f const& b)
       {
        this->a = a;
        this->b = b;
       }
       
       Line(float x1, float y1, float x2, float y2)
       {
        a.init(x1, y1);
        b.init(x2, y2);
       }
       
       Vec2f point(float k) const
       {
        return a + (b - a) * k;
       }
};

#endif
