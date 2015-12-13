#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

#include "Vec2f.hpp"
#include "Vec3f.hpp"
#include "DrawVec2f.hpp"
#include "CircularBuffer.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     //set_alpha_blender();
     set_difference_blender(255, 255, 255, 255);
     drawing_mode(DRAW_MODE_TRANS, NULL, 0, 0);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

#define FOCAL_DIST 256

#define ROOM_WIDTH          (640 * 0.8)
#define ROOM_HEIGHT         (480 * 0.8)
#define ROOM_NEAR           (0)
#define ROOM_FAR            (ROOM_NEAR + ROOM_WIDTH * 1.5)

#define PADDLE_WIDTH        (ROOM_WIDTH * 0.2)
#define PADDLE_HEIGHT       (ROOM_HEIGHT * 0.2)

#define BALL_RAD            (20)

float project(float x, float z)
{
   return x * FOCAL_DIST / (FOCAL_DIST + z);
}

Vec2f project(Vec3f in)
{
   return Vec2f(SCREEN_W / 2 + project(in.x, in.z),
                SCREEN_H / 2 + project(in.y, in.z));
}

void drawRect(BITMAP* buffer, Vec2f c, float w2, float h2, int col)
{
    rect(buffer, int(c.x - w2), int(c.y - h2), int(c.x + w2), int(c.y + h2), col);
}

void fillRect(BITMAP* buffer, Vec2f c, float w2, float h2, int col)
{
    rectfill(buffer, int(c.x - w2), int(c.y - h2), int(c.x + w2), int(c.y + h2), col);
}

void drawLine(BITMAP* buffer, Vec2f a, Vec2f b, int col)
{
   line(buffer, int(a.x), int(a.y), int(b.x), int(b.y), col);
}

void drawCorridor(BITMAP* buffer, float step, int col)
{
     for(float z = ROOM_NEAR; z < ROOM_FAR; z += step)
     {
         Vec2f c(SCREEN_W / 2, SCREEN_H / 2);
         float w2 = project(ROOM_WIDTH / 2, z),
               h2 = project(ROOM_HEIGHT / 2, z);
      
         drawRect(buffer, c, w2, h2, col);
     }
     
     Vec2f c(SCREEN_W / 2, SCREEN_H / 2);
     float w2 = project(ROOM_WIDTH / 2, ROOM_FAR),
           h2 = project(ROOM_HEIGHT / 2, ROOM_FAR);
     
     drawRect(buffer, c, w2, h2, col);
     drawLine(buffer, c + Vec2f(w2, h2), c + Vec2f(ROOM_WIDTH / 2, ROOM_HEIGHT / 2), col);
     drawLine(buffer, c - Vec2f(w2, h2), c - Vec2f(ROOM_WIDTH / 2, ROOM_HEIGHT / 2), col);
     drawLine(buffer, c + Vec2f(-w2, h2), c + Vec2f(-ROOM_WIDTH / 2, ROOM_HEIGHT / 2), col);
     drawLine(buffer, c - Vec2f(-w2, h2), c - Vec2f(-ROOM_WIDTH / 2, ROOM_HEIGHT / 2), col);
}

struct Paddle {
       Vec3f pos, vel, accel;
       
       Paddle(Vec3f p): pos(p)
       {
       }
       
       void constrain()
       {
            if(pos.x < -ROOM_WIDTH / 2 + PADDLE_WIDTH / 2)       { pos.x = -ROOM_WIDTH / 2 + PADDLE_WIDTH / 2; }
            if(pos.y < -ROOM_HEIGHT / 2 + PADDLE_HEIGHT / 2)     { pos.y = -ROOM_HEIGHT / 2 + PADDLE_HEIGHT / 2; }
            if(pos.x > ROOM_WIDTH / 2 - PADDLE_WIDTH / 2)        { pos.x = ROOM_WIDTH / 2 - PADDLE_WIDTH / 2; }
            if(pos.y > ROOM_HEIGHT / 2 - PADDLE_HEIGHT / 2)      { pos.y = ROOM_HEIGHT / 2 - PADDLE_HEIGHT / 2; }
       }
       
       void tick(float dt)
       {
            vel += accel * dt;
            pos += vel * dt;
            constrain();
       }
       
       void draw(BITMAP* buffer, int col)
       {
            Vec2f c = project(pos);
            float w2 = project(PADDLE_WIDTH / 2, pos.z),
                  h2 = project(PADDLE_HEIGHT / 2, pos.z);
     
            fillRect(buffer, c, w2, h2, makeacol(255, 255, 255, 128));
            drawRect(buffer, c, w2, h2, col);
            drawRect(buffer, c, w2 * 0.3, h2 * 0.3, col);
       }
};

struct Ball {
       Vec3f pos, vel, accel;
       
       Ball(): pos(0, 0, 0)
       {
       }
       
       void constrain()
       {
            if(pos.x < -ROOM_WIDTH / 2 + BALL_RAD)      { vel.x *= -1; pos.x = -ROOM_WIDTH / 2 + BALL_RAD; }
            if(pos.y < -ROOM_HEIGHT / 2 + BALL_RAD)     { vel.y *= -1; pos.y = -ROOM_HEIGHT / 2 + BALL_RAD; }
            if(pos.x > ROOM_WIDTH / 2 - BALL_RAD)       { vel.x *= -1; pos.x = ROOM_WIDTH / 2 - BALL_RAD; }
            if(pos.y > ROOM_HEIGHT / 2 - BALL_RAD)      { vel.y *= -1; pos.y = ROOM_HEIGHT / 2 - BALL_RAD; }
             
            //if(pos.z < ROOM_NEAR) { vel.z *= -1; pos.z = ROOM_NEAR; }
            if(pos.z > ROOM_FAR)  { vel.z *= -1; pos.z = ROOM_FAR; }
       }
       
       bool collidesPaddle(Vec3f paddle)
       {
            Vec3f min1(pos.x - BALL_RAD, pos.y - BALL_RAD, pos.z - BALL_RAD);
            Vec3f max1(pos.x + BALL_RAD, pos.y + BALL_RAD, pos.z + BALL_RAD);
            Vec3f min2(paddle.x - PADDLE_WIDTH / 2, paddle.y - PADDLE_HEIGHT / 2, paddle.z - 1);
            Vec3f max2(paddle.x + PADDLE_WIDTH / 2, paddle.y + PADDLE_HEIGHT / 2, paddle.z + 1);
            
            return !(max1.x < min2.x || max2.x < min1.x ||
                     max1.y < min2.y || max2.y < min1.y ||
                     max1.z < min2.z || max2.z < min1.z);
       }
       
       void tick(float dt)
       {
            float l = accel.length();
            
            if(l > 20.f)
                 accel *= 20.f / l;
        
            vel += accel * dt;
            pos += vel * dt;
            constrain();
       }
       
       void draw(BITMAP* buffer, int col)
       {
            Vec2f c = project(pos);
            float r = project(BALL_RAD, pos.z);
            
            circlefill(buffer, int(c.x), int(c.y), int(r), col);
       }
};

struct Game {
       
};

int main()
{   
    bool exit = false;
    Paddle p1(Vec3f(0, 0, ROOM_NEAR)),
           p2(Vec3f(0, 0, ROOM_FAR));
    Ball ball;
    bool serving = true;
    
    Vec3f dest;
    
    init();
    
    //int tx = 0, ty = 0;
    Vec3f v;

    int lastMb = 0;

    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     int mx, my;
     get_mouse_mickeys(&mx, &my);
     
     if(serving && !lastMb && mouse_b == 1 && ball.collidesPaddle(p1.pos))
     {
         v = p1.vel * 0.05;
         ball.pos.z = ROOM_NEAR + BALL_RAD + 1;
         ball.vel.init(0, 0, 70);
         ball.accel.init(-v.x, -v.y, 0);
         serving = false;
     }
     
     lastMb = mouse_b;
     
     dest.x += mx * 0.3;
     dest.y += my * 0.3;
     
     if(dest.x < -ROOM_WIDTH / 2 + PADDLE_WIDTH / 2)       { dest.x = -ROOM_WIDTH / 2 + PADDLE_WIDTH / 2; }
     if(dest.y < -ROOM_HEIGHT / 2 + PADDLE_HEIGHT / 2)     { dest.y = -ROOM_HEIGHT / 2 + PADDLE_HEIGHT / 2; }
     if(dest.x > ROOM_WIDTH / 2 - PADDLE_WIDTH / 2)        { dest.x = ROOM_WIDTH / 2 - PADDLE_WIDTH / 2; }
     if(dest.y > ROOM_HEIGHT / 2 - PADDLE_HEIGHT / 2)      { dest.y = ROOM_HEIGHT / 2 - PADDLE_HEIGHT / 2; }
     
     p1.accel = (dest - p1.pos) * 50;
     p1.vel *= 0.9;
     
     p1.tick(0.01);
     
     ball.tick(0.01);
     
     if(!serving && ball.collidesPaddle(p1.pos))
     {
        v = p1.vel * 0.09;
        ball.pos.z = ROOM_NEAR + BALL_RAD + 1;
        ball.vel.z *= -1.05;
        ball.accel += Vec3f(-v.x, -v.y, 0);
     }
     
     if(ball.pos.z < 0)
     {
        ball = Ball();
        serving = true;
     }
     
     clear_to_color(buffer, 0);
     
     textprintf_ex(buffer, font, 10, 10, makecol(255, 255, 255), -1, "%f - %f", v.x, v.y);
     
     drawCorridor(buffer, 100, makeacol(0, 64, 0, 255));
     
     Vec2f c(SCREEN_W / 2, SCREEN_H / 2);
     float w2 = project(ROOM_WIDTH / 2, ball.pos.z), h2 = project(ROOM_HEIGHT / 2, ball.pos.z);
     drawRect(buffer, c, w2, h2, makeacol(0, 255, 0, 255));
     
     //drawPaddle(buffer, p2, makeacol(0, 0, 255, 255));
     p2.draw(buffer, makeacol(0, 0, 255, 255));
     ball.draw(buffer, makeacol(255, 255, 255, 255));
     p1.draw(buffer, makeacol(255, 0, 0, 255));
     
     Vec2f dp = project(dest);
     
     //draw_sprite(buffer, mouse_sprite, int(dp.x), int(dp.y));
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
