#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include <vector>

#include "DrawVec2f.hpp"

#include "Vec2f.hpp"
#include "Util.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

#define BOARD_W 5
#define BOARD_H 5
#define CELL_SIZE 64

// **************
// * Util funcs *
// **************

int sign(float f)
{
   return f < 0 ? -1 : 1;
}

int trunc(float f)
{
   return f > 0 ? int(f) : int(f - 1);
}

int mod(int n, int max)
{
   int m = n % max;
   return m >= 0 ? m : max + m;
}

//
// ***************
//

int toBy(float sy)
{
    return trunc(sy) / CELL_SIZE;
}

int toBx(float sx, float sy)
{
    int j = toBy(sy);
    return (trunc(sx) - CELL_SIZE / 2 * mod(j, 2)) / CELL_SIZE;
}

    float toWy(float j)
    {
        return j * CELL_SIZE + CELL_SIZE / 2;
    }

    float toWx(float i, float j)
    {
        return i * CELL_SIZE + CELL_SIZE / 2 * mod(rnd(j), 2) + CELL_SIZE / 2;
    }

    Vec2f toWorld(float i, float j)
    {
        Vec2f w;
        w.x = toWx(i, j);
        w.y = toWy(j);

        return w;
    }
    
    Vec2f crap(Vec2f p)
    {
        return toWorld(toBx(p.x, p.y), toBy(p.y));
    }

Vec2f drawRay(Vec2f const& pos, Vec2f const& vel)
{
     float xoffs = CELL_SIZE / 2 * (mod(rnd(pos.y / CELL_SIZE), 2));
     
     Vec2f orig((pos.x - xoffs) / CELL_SIZE, pos.y / CELL_SIZE);
     Vec2f dir = vel.unit();
     
     int x = rnd(orig.x), y = rnd(orig.y);
     int stepX = sgni(dir.x), stepY = sgni(dir.y);    
     
     float tMaxX = (rnd(orig.x + (stepX > 0)) - orig.x) / dir.x;
     float tMaxY = (rnd(orig.y + (stepY > 0)) - orig.y) / dir.y;
     float t = tMaxX < tMaxY ? tMaxX : tMaxY;
     
     t *= 1.1;
     
     Vec2f dest = (orig + dir * t); //* CELL_SIZE;
     //dest.x -= 2;
     //dest.y -= 2;
     
     printf("%d\n", rnd(dest.x));
     
     dest *= CELL_SIZE;
     
     dest.x += xoffs;
     
     //printf("%f\n",xoffs);
         
     return dest;
}

int main()
{
    bool exit = false;
    
    init();
    
    Vec2f pos(330, 270), vel(100, 100);
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b == 2)
        pos.init(mouse_x, mouse_y);
     if(mouse_b == 1)
     vel = Vec2f(mouse_x, mouse_y) - pos;
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     for(int j = 0; j < BOARD_H + 10; j++)
        for(int i = 0; i < BOARD_W + 10; i++)
        {
            int col = i >= 0 && j >= 0 && i < BOARD_W && j < BOARD_H ? makecol(255, 255, 255) : makecol(255, 0, 0);
         
            rect(buffer, 100 + i * CELL_SIZE + CELL_SIZE / 2 * (j % 2), 100 + j * CELL_SIZE,
                         100 + (i + 1) * CELL_SIZE + CELL_SIZE / 2 * (j % 2), 100 + (j + 1) * CELL_SIZE,
                         col);
        }
     
     Vec2f pos2 = pos;
     Vec2f vel2 = vel;
     pos2.x -= 100;
     pos2.y -= 100;
     
     Vec2f p = crap(drawRay(pos2, vel));
     Vec2f p2 = drawRay(pos2, vel);
     Vec2f p3 = crap(pos2);
     
     p.x += 100;
     p.y += 100;
     p2.x += 100;
     p2.y += 100;
     p3.x += 100;
     p3.y += 100;
     
     //DrawVec2f::circlefill(buffer, pos, 5, makecol(255, 0, 0));
     DrawVec2f::line(buffer, pos, pos + vel, makecol(255, 0, 0));
     //DrawVec2f::circlefill(buffer, p, 2, makecol(0, 255, 0));
     //DrawVec2f::circlefill(buffer, p2, 2, makecol(0, 0, 255));
     DrawVec2f::circlefill(buffer, p3, 2, makecol(255, 255, 0));
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
