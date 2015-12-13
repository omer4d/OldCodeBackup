#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>
#include <array>
#include <random>

#include "Buffer.hpp"
#include "GridMatrix.hpp"
#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

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
     
     //install_int(tick, 1);
     
     srand(time(NULL));
     //srand(100);
}

void deinit()
{
     destroy_bitmap(buffer);
}

void box(BITMAP* buffer, float x, float y, float r, int col)
{
   float JFACTOR = (float)sqrt(3.f) * 0.5f;
 
   rect(buffer, int(x - r), int(y - r * JFACTOR), int(x + r), int(y + r * JFACTOR), col);
}



int main()
{
    bool exit = false;
    
    init();
    
    GridMatrix<int> grid(20, 20, 30, true, 0);
    
    Vec2f orig(413, 300), dir1, dir2;
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b == 1)
        orig.init(mouse_x, mouse_y);
     
     //printf("%d %d", grid.toBx(mouse_x) );
     
     Vec2f d = (Vec2f(mouse_x, mouse_y) - orig).unit();
     dir1 = d.rotated(-0.4f);
     dir2 = d.rotated(0.4f);
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     int maxj = grid.toBy(orig.y) - 1;
     
     for(int j = maxj; j >= 0; --j)
     {
        float y = grid.toWy(j);
        float ymin = grid.toWy(j - 0.5f);
        float ymax = grid.toWy(j + 0.5f);
        
        float x0ymin = orig.x + (ymin - orig.y) / dir1.y * dir1.x;
        float x0ymax = orig.x + (ymax - orig.y) / dir1.y * dir1.x;
        
        float x1ymin = orig.x + (ymin - orig.y) / dir2.y * dir2.x;
        float x1ymax = orig.x + (ymax - orig.y) / dir2.y * dir2.x;
        
        float x0 = MIN(x0ymin, x0ymax);
        float x1 = MAX(x1ymin, x1ymax);
        
        int mini = MAX(0, int(floor(grid.toBx(x0, y))) - 1);
        int maxi = MIN(ceil(grid.toBx(x1, y)) + 1, grid.getLogicColNum(j));

        if(mini < maxi)
        for(int i = mini; i < maxi; ++i)
        {
           //circle(buffer, (int)grid.toWx(i, j), (int)grid.toWy(j), (int)(grid.cellSize), makecol(50, 0, 0));
           //circle(buffer, (int)grid.toWx(i, j), (int)grid.toWy(j), (int)(grid.cellSize), 0);
           circlefill(buffer, (int)grid.toWx(i, j), (int)grid.toWy(j), (int)(2), 0);
        }
     }
     
     for(int j = 0; j < grid.rowNum; ++j)
       for(int i = 0; i < grid.getLogicColNum(j); ++i)
          box(buffer, (int)grid.toWx(i, j), (int)grid.toWy(j), (int)(grid.cellSize / 2), makecol(50, 50, 50));
     
     DrawVec2f::circlefill(buffer, orig, 3, makecol(255, 255, 0));
     DrawVec2f::line(buffer, orig, orig + dir1 * 1000, makecol(255, 255, 0));
     DrawVec2f::line(buffer, orig, orig + dir2 * 1000, makecol(255, 255, 0));
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
