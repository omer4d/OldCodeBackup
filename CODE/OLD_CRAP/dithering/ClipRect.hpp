#ifndef _CLIPRECT_HPP_
#define _CLIPRECT_HPP_

struct ClipRect {
       int minx, miny, maxx, maxy;
       
       void initDims(int x, int y, int w, int h)
       {
           this->minx = x;
           this->miny = y;
           this->maxx = x + ABS(w);
           this->maxy = y + ABS(h);
       }
       
       void initCorners(int x1, int y1, int x2, int y2)
       {
           this->minx = MIN(x1, x2);
           this->miny = MIN(y1, y2);
           this->maxx = MAX(x1, x2);
           this->maxy = MAX(y1, y2);
       }
       
       ClipRect()
       {
           minx = 0;
           miny = 0;
           maxx = 0;
           maxy = 0;
       }
       
       ClipRect(int x1, int y1, int x2, int y2)
       {
           initCorners(x1, y1, x2, y2);
       }
       
       int getWidth() { return maxx - minx; }
       int getHeight() { return maxy - miny; }
       
       static ClipRect intersect(ClipRect const& a, ClipRect const& b)
       {
           return ClipRect(MAX(a.minx, b.minx), MAX(a.miny, b.miny),
                           MIN(a.maxx, b.maxx), MIN(a.maxy, b.maxy));
       }
};

#endif
