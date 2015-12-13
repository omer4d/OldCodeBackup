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


void tri(GridMatrix<int>& grid, Vec2f const& orig, Vec2f const& dir1, Vec2f const& dir2)
{
    float cellW = grid.cellSize * 0.5f;
    float cellH = grid.cellSize * (float)sqrt(3.f) * 0.5f;
    float iPerJ1 = dir1.x / (dir1.y * cellW / cellH);
    float iPerJ2 = dir2.x / (dir2.y * cellW / cellH);
    float origI = orig.x / cellW;
    float origJ = orig.y / cellH;
    
    float dj1 = iPerJ1 > 0 ? 0 : 1, dj2 = iPerJ2 > 0 ? 1 : 0;
    
    float mini = origI + iPerJ1 * (int(origJ) - 1 + dj1 - origJ);
    float maxi = origI + iPerJ2 * (int(origJ) - 1 + dj2 - origJ);
    
    for(int j = int(origJ) - 1; j >= 0; --j)
    {
        int realMinI = (int(mini) - (j % 2)) / 2;
        int realMaxI = (int(maxi) - (j % 2)) / 2;
     
        for(int i = realMinI; i <= realMaxI; ++i)
        {
            circlefill(buffer,
                              grid.toWx(i, j),
                              grid.toWy(j),
                              (int)(2), 0);
        }
        
        mini -= iPerJ1;
        maxi -= iPerJ2;
    }
}

int sgni(float f)
{
   return f < 0 ? -1 : 1;
}


void drawRay(GridMatrix<int>& grid, Vec2f orig, Vec2f dir, int refs=3)
{
     Vec2f realOrig = orig, realDir = dir;
 
     float cellW = grid.cellSize * 0.5f;
     float cellH = grid.cellSize * (float)sqrt(3.f) * 0.5f;
     
     dir.y *= (cellW / cellH);
     orig.x /= cellW;
     orig.y /= cellH;
     
     int x = int(orig.x), y = int(orig.y);
     int stepX = sgni(dir.x), stepY = sgni(dir.y);    
     
     float tMaxX = (int(orig.x + (stepX > 0)) - orig.x) / dir.x;
     float tMaxY = (int(orig.y + (stepY > 0)) - orig.y) / dir.y;
     float tDeltaX = float(stepX) / dir.x;
     float tDeltaY = float(stepY) / dir.y;
     float t = 0.0;
     
     while(y >= 0 && y < grid.rowNum)
     {
        int i = (x - (y % 2)) / 2;
        circlefill(buffer, int(grid.toWx(i, y)), int(grid.toWy(y)), 2, 0);
        
        if(tMaxX < tMaxY)
        { 
           t = tMaxX;
           tMaxX += tDeltaX;
           x += stepX;
           
           if((x == 0 && stepX < 0) || (x == grid.colNum * 2 - 1 && stepX > 0))
           {
              DrawVec2f::line(buffer, realOrig, realOrig + realDir * t * cellW, makecol(255, 255, 0));
              if(refs > 0)
               drawRay(grid, realOrig + realDir * (t * 0.999f) * cellW, Vec2f(-realDir.x, realDir.y), refs - 1);
              return;
           }
        }
        
        else
        {
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
        }
     }
     
     DrawVec2f::line(buffer, realOrig, realOrig + realDir * t * cellW, makecol(255, 255, 0));
}

/*
void drawRay(GridMatrix<int>& grid, Vec2f orig, Vec2f dir)
{
     float cellW = grid.cellSize * 0.5f;
     float cellH = grid.cellSize * (float)sqrt(3.f) * 0.5f;
     
     dir.y *= (cellW / cellH);
     orig.x /= cellW;
     orig.y /= cellH;
     
     int x = int(orig.x), y = int(orig.y);
     int stepX = sgni(dir.x), stepY = sgni(dir.y);    
     
     float tMaxX = (int(orig.x + (stepX > 0)) - orig.x) / dir.x;
     float tMaxY = (int(orig.y + (stepY > 0)) - orig.y) / dir.y;
     float tDeltaX = float(stepX) / dir.x;
     float tDeltaY = float(stepY) / dir.y;
     float t = 0.0;
     
     while(x >= 0 && y >= 0 && x < grid.colNum * 2 && y < grid.rowNum)
     {
        int i = (x - (y % 2)) / 2;
        circlefill(buffer, int(grid.toWx(i, y)), int(grid.toWy(y)), 2, 0);
        
        if(tMaxX < tMaxY)
        {
           t = tMaxX;
           tMaxX += tDeltaX;
           x += stepX;
        }
        
        else
        {
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
        }
     }
}*/

int main()
{
    bool exit = false;
    
    init();
    
    GridMatrix<int> grid(20, 20, 30, true, 0);
    
    Vec2f orig(413, 300), dir;
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b == 1)
        orig.init(mouse_x, mouse_y);
     
     dir = (Vec2f(mouse_x, mouse_y) - orig).unit();
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     for(int j = 0; j < grid.rowNum; ++j)
       for(int i = 0; i < grid.getLogicColNum(j); ++i)
          box(buffer, (int)grid.toWx(i, j), (int)grid.toWy(j), (int)(grid.cellSize / 2), makecol(50, 50, 50));
     
     DrawVec2f::circlefill(buffer, orig, 3, makecol(255, 255, 0));
     drawRay(grid, orig, dir);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
