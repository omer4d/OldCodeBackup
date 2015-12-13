#ifndef _DrawVec2f_hpp_
#define _DrawVec2f_hpp_

#include <allegro.h>
#include "Mat9f.hpp"
#include "Vec2f.hpp"
#include "Poly.hpp"

struct DrawVec2f {
      static float tx, ty, sx, sy, su;
      
      static void init(float tx=0, float ty=0, float sx=1, float sy=1, float su=1);
      static int scrx(float x);
      static int scry(float y);
      static void line(BITMAP* buffer, Vec2f const& a, Vec2f const& b, int col);
      static void pixel(BITMAP* buffer, Vec2f const& p, int col);
      static void circle(BITMAP* buffer, Vec2f const& p, float r, int col);
      
      static void circlefill(BITMAP* buffer, Vec2f const& p, int r, int col);
      
      static void rect(BITMAP* buffer, Vec2f const& p1, Vec2f const& p2, int col);
      
      static void rect(BITMAP* buffer, Vec2f const& p1, float w, float h, int col);
      static void rectfill(BITMAP* buffer, Vec2f const& p1, Vec2f const& p2, int col);
      
      static void rectfill(BITMAP* buffer, Vec2f const& p1, float w, float h, int col);

    static void poly(BITMAP* buffer, Poly const& poly, int fillCol, int lineCol = -1,
                     Mat9f const& mat = Mat9f::identity);
};

#endif
