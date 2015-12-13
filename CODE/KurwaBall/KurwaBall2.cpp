#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

#include "Vec2f.hpp"
#include "Vec3f.hpp"
#include "Renderer.hpp"

#define FPS 60

double runtime = 0;
bool hasTicked = false;

void tick()
{
   runtime += 0.001;
   hasTicked = true;
}

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     install_timer();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 800, 600, 0, 0);
     
     set_alpha_blender();
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     
     srand(time(NULL));
     install_int(tick, 17);
}

Vec2f constrain2D(Vec2f& p, Vec2f min, Vec2f max)
{
     Vec2f res(1, 1);
     
     if(p.x < min.x) { p.x = min.x; res.x = -1; }
     if(p.y < min.y) { p.y = min.y; res.y = -1; }
     if(p.x > max.x) { p.x = max.x; res.x = -1; }
     if(p.y > max.y) { p.y = max.y; res.y = -1; }
     
     return res;
}

Vec3f constrain3D(Vec3f& p, Vec3f min, Vec3f max)
{
     Vec3f res(1, 1, 1);
     
     if(p.x < min.x) { p.x = min.x; res.x = -1; }
     if(p.y < min.y) { p.y = min.y; res.y = -1; }
     if(p.x > max.x) { p.x = max.x; res.x = -1; }
     if(p.y > max.y) { p.y = max.y; res.y = -1; }
     if(p.z < min.z) { p.z = min.z; res.z = -1; }
     if(p.z > max.z) { p.z = max.z; res.z = -1; }
     
     return res;
}

// With padding:

Vec2f constrain2D(Vec2f& p, Vec2f min, Vec2f max, float px, float py)
{
      return constrain2D(p, min + Vec2f(px, py), max - Vec2f(px, py));
}

Vec3f constrain3D(Vec3f& p, Vec3f min, Vec3f max, float px, float py, float pz)
{
      return constrain3D(p, min + Vec3f(px, py, pz), max - Vec3f(px, py, pz));
}

struct Box {
       Vec3f min, max;
       
       static bool intersection(Box const& a, Box const& b)
       {
            return !(a.max.x < b.min.x || b.max.x < a.min.x ||
                     a.max.y < b.min.y || b.max.y < a.min.y ||
                     a.max.z < b.min.z || b.max.z < a.min.z);
       }
};

struct Paddle {
       Vec3f mPos, mVel, mAccel;
       float mWidth, mHeight;
       
       Paddle(Vec3f pos, float width, float height): mPos(pos), mWidth(width), mHeight(height)
       {
       }
       
       void constrain(Vec3f min, Vec3f max)
       {
            constrain3D(mPos, min, max, mWidth / 2, mHeight / 2, 0);
       }
       
       void tick(float dt)
       {
            mVel += mAccel * dt;
            mPos += mVel * dt;
       }
       
       void draw(Renderer& rend, int col)
       {
            float w2 = mWidth / 2, h2 = mHeight / 2;
            
            rend.fillRect3D(mPos, w2, h2, makeacol(255, 255, 255, 128));
            rend.drawRect3D(mPos, w2, h2, col);
            rend.drawRect3D(mPos, w2 * 0.3, h2 * 0.3, col);
       }
       
       Box getBox()
       {
            Box box;
            
            box.min.init(mPos.x - mWidth / 2, mPos.y - mHeight / 2, mPos.z - 1);
            box.max.init(mPos.x + mWidth / 2, mPos.y + mHeight / 2, mPos.z + 1);
            
            return box;
       }
};

struct Ball {
       Vec3f mPos, mVel, mAccel;
       float mRad;
       
       Ball(): mPos(0, 0, 0), mRad(20)
       {
       }
       
       void reset()
       {
            mPos.init(0, 0, 0);
            mVel.init(0, 0, 0);
            mAccel.init(0, 0, 0);
       }
       
       void constrain(Vec3f min, Vec3f max)
       {
            Vec3f res = constrain3D(mPos, min, max, mRad, mRad, mRad);
            mVel.x *= res.x;
            mVel.y *= res.y;
            mVel.z *= res.z;
       }
       
       void tick(float dt)
       {
            float l = mAccel.length();
            
            if(l > 200.f)
                 mAccel *= 200.f / l;
        
            mVel += mAccel * dt;
            mPos += mVel * dt;
       }
       
       void draw(Renderer& rend, int col)
       {
            rend.fillCircle3D(mPos, mRad, col);
       }
       
       Box getBox()
       {
            Box box;
            
            box.min.init(mPos.x - mRad, mPos.y - mRad, mPos.z - mRad);
            box.max.init(mPos.x + mRad, mPos.y + mRad, mPos.z + mRad);
            
            return box;
       }
};

struct Game {
       float mWidth, mHeight, mNear, mFar;
       Vec3f mMin, mMax;
       
       Paddle *p1, *p2;
       Ball* ball;
       Vec3f dest;
       bool serving;
       int lastMb;
       
       Game(float width, float height, float near, float far):
                  mWidth(width), mHeight(height), mNear(near), mFar(far)
       {
             p1 = new Paddle(Vec3f(0, 0, near), width * 0.2, height * 0.2);
             p2 = new Paddle(Vec3f(0, 0, far), width * 0.2, height * 0.2);
             ball = new Ball();
             
             mMin.init(-mWidth / 2, -mHeight / 2, mNear);
             mMax.init(mWidth / 2, mHeight / 2, mFar);
             
             serving = true;
             lastMb = 0;
       }
       
       ~Game()
       {
              delete ball;
              delete p1;
              delete p2;
       }
      
      void movePaddle(float sens, float k, float damp)
      {
           int mx, my;
           get_mouse_mickeys(&mx, &my);
     
           dest.x += mx * sens;
           dest.y += my * sens;
           
           constrain3D(dest, mMin, mMax, p1->mWidth / 2, p1->mHeight / 2, 0);
           
           p1->mAccel = (dest - p1->mPos) * k;
           p1->mVel *= damp;
      }
      
      void serve(float speed, float curve)
      {
           Vec3f v = p1->mVel * curve;
           ball->mPos.z = mNear + ball->mRad;
           ball->mVel.init(0, 0, speed);
           ball->mAccel.init(-v.x, -v.y, 0);    
      }
      
      void handleBallPaddleCollision(Ball* ball, Paddle* paddle, float boost, float curve)
      {
           Vec3f v = p1->mVel * curve;
           ball->mPos.z = paddle->mPos.z + ball->mRad + 2;
           ball->mVel.z *= -boost;
           ball->mAccel += Vec3f(-v.x, -v.y, 0);
      }
      
      void tick(float dt)
      {
           if(serving && !lastMb && mouse_b == 1 && Box::intersection(ball->getBox(), p1->getBox()))
           {
               serve(300, 100);
               serving = false;
           }
     
           lastMb = mouse_b;
       
           movePaddle(0.3, 700, 0.7);
       
           p1->tick(dt);
           p2->tick(dt);
           ball->tick(dt);
           
           ball->constrain(Vec3f(mMin.x, mMin.y, -1000), mMax);
           p1->constrain(mMin, mMax);
           p2->constrain(mMin, mMax);
           
           if(!serving && Box::intersection(ball->getBox(), p1->getBox()))
           {
              handleBallPaddleCollision(ball, p1, 1.05, 150);
           }
     
           if(ball->mPos.z < 0)
           {
               ball->reset();
               serving = true;
           }
      }
      
      void drawCorridor(Renderer& rend, float step, int col)
       {
            float w2 = mWidth * 0.5, h2 = mHeight * 0.5;
     
            for(float z = mNear; z < mFar; z += step)
            {
                rend.drawRect3D(Vec3f(0, 0, z), w2, h2, col);
            }
     
            rend.drawRect3D(Vec3f(0, 0, mFar), w2, h2, col);
     
            rend.drawLine3D(Vec3f(-w2, -h2, mNear), Vec3f(-w2, -h2, mFar), col);
            rend.drawLine3D(Vec3f(w2, -h2, mNear), Vec3f(w2, -h2, mFar), col);
            rend.drawLine3D(Vec3f(w2, h2, mNear), Vec3f(w2, h2, mFar), col);
            rend.drawLine3D(Vec3f(-w2, h2, mNear), Vec3f(-w2, h2, mFar), col);
      }
      
      void draw(Renderer& rend)
      {
           float w2 = mWidth * 0.5, h2 = mHeight * 0.5;
       
           p2->draw(rend, makeacol(255, 64, 64, 255));
           drawCorridor(rend, 100, makeacol(32, 64, 32, 255));
           
           rend.drawRect3D(Vec3f(0, 0, ball->mPos.z), w2, h2, makeacol(128, 255, 128, 255));
           
           ball->draw(rend, makeacol(255, 255, 255, 255));
           p1->draw(rend, makeacol(64, 64, 255, 255));
      }
};

int main()
{   
    bool exit = false;
    double lastLogicTime = -1000;
    double lastFpsTime = -1000;
    int frames = 0, currFps = 0;
    
    init();
    
    Renderer rend(SCREEN_W, SCREEN_H, 256, 0);
    Game game(640, 480, 0, 640 * 1.5);
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     if(key[KEY_F2]) rend.setStereo(-20);
     if(key[KEY_F3]) rend.setStereo(20);
     if(key[KEY_F4]) rend.setStereo(0);
     
     //if(hasTicked)//runtime - lastLogicTime > 1.0 / float(FPS))
     {
        hasTicked = false;
        game.tick(1.0 / FPS);
        lastLogicTime = runtime;
        frames++;
     }
     
     //frames++;
     
     if(runtime - lastFpsTime > 1.0)
     {
        currFps = frames;
        frames = 0;
        lastFpsTime = runtime;
     }
     
     rend.clear();
     game.draw(rend);
     textprintf_ex(rend.mLeft, font, 10, 10, makeacol(255, 255, 255, 255), -1, "%d", currFps);
     textprintf_ex(rend.mRight, font, 10, 10, makeacol(255, 255, 255, 255), -1, "%d", currFps);
     blit(rend.getBuffer(), screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    return 0;
}END_OF_MAIN()
