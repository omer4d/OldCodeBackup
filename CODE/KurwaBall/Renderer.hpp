#ifndef _RENDERER_HPP_
#define _RENDERER_HPP_

#include <allegro.h>
#include "Vec2f.hpp"
#include "Vec3f.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

struct Renderer {
       BITMAP *mBuffer, *mLeft, *mRight;
       float mFocal, mStereo;
       
       Renderer(int w, int h, float focal=256, float stereo=0): mFocal(focal), mStereo(stereo)
       {
           mBuffer = create_bitmap(w, h);
           mLeft = create_bitmap(w, h);
           mRight = create_bitmap(w, h);
       }
       
       ~Renderer()
       {
           destroy_bitmap(mBuffer);
           destroy_bitmap(mLeft);
           destroy_bitmap(mRight);
       }
       
       void setStereo(float stereo)
       {
            mStereo = stereo;
       }
       
       void clear()
       {
            clear_to_color(mLeft, 0);
            clear_to_color(mRight, 0);
       }
       
       float project(float x, float z)
       {
             return x * mFocal / (mFocal + z);
       }

       Vec2f project(Vec3f in)
       {
             return Vec2f(SCREEN_W / 2 + project(in.x, in.z),
                          SCREEN_H / 2 + project(in.y, in.z));
       }
       
       // ********************
       // * 2D drawing funcs *
       // ********************

       void drawRect(BITMAP* dest, Vec2f c, float w2, float h2, int col)
       {
            rect(dest, int(c.x - w2), int(c.y - h2), int(c.x + w2), int(c.y + h2), col);
       }

       void fillRect(BITMAP* dest, Vec2f c, float w2, float h2, int col)
       {
            rectfill(dest, int(c.x - w2), int(c.y - h2), int(c.x + w2), int(c.y + h2), col);
       }

       void drawLine(BITMAP* dest, Vec2f a, Vec2f b, int col)
       {
            line(dest, int(a.x), int(a.y), int(b.x), int(b.y), col);
       }
       
       void fillCircle(BITMAP* dest, Vec2f c, float r, int col)
       {
            circlefill(dest, int(c.x), int(c.y), int(r), col);
       }
       
       // ********************
       // * 3D drawing funcs *
       // ********************
       
       void drawRect3D(Vec3f c, float w2, float h2, int col)
       {
            c.x -= mStereo;
            drawRect(mLeft, project(c), project(w2, c.z), project(h2, c.z), col);
            c.x += mStereo * 2;
            drawRect(mRight, project(c), project(w2, c.z), project(h2, c.z), col);
       }
       
       void fillRect3D(Vec3f c, float w2, float h2, int col)
       {
            c.x -= mStereo;
            fillRect(mLeft, project(c), project(w2, c.z), project(h2, c.z), col);
            c.x += mStereo * 2;
            fillRect(mRight, project(c), project(w2, c.z), project(h2, c.z), col);
       }
       
       void drawLine3D(Vec3f a, Vec3f b, int col)
       {
            a.x -= mStereo;
            b.x -= mStereo;
            drawLine(mLeft, project(a), project(b), col);
            
            a.x += mStereo * 2;
            b.x += mStereo * 2;
            drawLine(mRight, project(a), project(b), col);
       }
       
       void fillCircle3D(Vec3f c, float r, int col)
       {
            c.x -= mStereo;
            fillCircle(mLeft, project(c), project(r, c.z), col);
            c.x += mStereo * 2;
            fillCircle(mRight, project(c), project(r, c.z), col);
       }
       
       BITMAP* getBuffer()
       {
            for(int y = 0; y < mBuffer->h; ++y)
              for(int x = 0; x < mBuffer->w; ++x)
              {
                  int c1 = PIXEL(mLeft, x, y), c2 = PIXEL(mRight, x, y);
                  PIXEL(mBuffer, x, y) = makeacol32(getr32(c1), 0, getb32(c2), 255);
                  //PIXEL(mBuffer, x, y) = makeacol32(getr32(c1), getg32(c2), getb32(c2), 255);
              }
              
            return mBuffer;
       }
};

#endif
