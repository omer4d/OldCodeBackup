#ifndef _DrawVec2f_hpp_
#define _DrawVec2f_hpp_

#include <allegro.h>

#include "Poly.hpp"
#include "Vec2f.hpp"

struct DrawVec2f {
      static float tx, ty, sx, sy, su;
      
      static void init(float tx=0, float ty=0, float sx=1, float sy=1, float su=1)
      {
           DrawVec2f::tx = tx;
           DrawVec2f::ty = ty;
           DrawVec2f::sx = sx;
           DrawVec2f::sy = sy;
           DrawVec2f::su = su;
      }
      
      static int scrx(float x)
      {
          return int(x * sx + tx);
      }
      
      static int scry(float y)
      {
          return int(y * sy + ty);
      }
      
      static void line(BITMAP* buffer, Vec2f const& a, Vec2f const& b, int col)
      {
           ::line(buffer, scrx(a.x), scry(a.y),
                          scrx(b.x), scry(b.y), col);
      }
      
      static void circle(BITMAP* buffer, Vec2f const& p, int r, int col)
      {
           ::circle(buffer, scrx(p.x), scry(p.y), int(r * su), col);
      }
      
      static void circlefill(BITMAP* buffer, Vec2f const& p, int r, int col)
      {
           ::circlefill(buffer, scrx(p.x), scry(p.y), int(r * su), col);
      }
      
      static void rect(BITMAP* buffer, Vec2f const& p1, Vec2f const& p2, int col)
      {
           ::rect(buffer, scrx(p1.x), scry(p1.y), scrx(p2.x), scry(p2.y), col);
      }
      
      static void rectfill(BITMAP* buffer, Vec2f const& p1, Vec2f const& p2, int col)
      {
           ::rectfill(buffer, scrx(p1.x), scry(p1.y), scrx(p2.x), scry(p2.y), col);
      }
      
      static void poly(BITMAP* buffer, Poly const& poly, int fillCol, int lineCol=-1)
      {
           if(fillCol != -1)
           {
               int* points = new int[poly.vertexVec.size() * 2];
     
               for(int i = 0; i < poly.vertexVec.size(); i++)
               {
                       points[i * 2]     = int(scrx(poly.vertexVec[i].x));
                       points[i * 2 + 1] = int(scry(poly.vertexVec[i].y));
               }
     
               ::polygon(buffer, poly.vertexVec.size(), points, fillCol);
               delete[] points;
           }
     
           if(lineCol != -1)
           {
              int i;
              for(i = 0; i < poly.vertexVec.size() - 1; i++)
                  line(buffer, poly.vertexVec[i], poly.vertexVec[i + 1], lineCol);
              line(buffer, poly.vertexVec[i], poly.vertexVec[0], lineCol);
           }
     
     }
};

float DrawVec2f::tx = 0;
float DrawVec2f::ty = 0;
float DrawVec2f::sx = 1;
float DrawVec2f::sy = 1;
float DrawVec2f::su = 1;


#endif
