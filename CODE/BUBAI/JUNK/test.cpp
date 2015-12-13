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

#define CELL_SIZE 30

#define TYPENUM 7

#define EMPTY -1
#define FULL 1
#define UNVISITED 0

void skeleton(GridMatrix<int>& mat, float fillChance)
{
    mat.clear(UNVISITED);

    for(int i = 0; i < (mat.getLogicColNum(0) + 1) / 2; ++i)
       mat.data[i][0] = Util::randf() < 0.95 ? FULL : EMPTY;
    
    fillChance = 0.99f;
    
    for(int j = 0; j < mat.rowNum; ++j)
    {
      fillChance -= 0.05f;
     
      if(fillChance > 0.8f)
        fillChance = 0.8f;
     
      for(int i = 0; i < (mat.getLogicColNum(j) + 1) / 2; ++i)
      {
        if(mat.data[i][j] == UNVISITED)
        {
            for(int k = 0; k < 6; ++k)
               if(mat.getNeighbor(i, j, k) == FULL)
               {           
                  mat.data[i][j] = Util::randf() < fillChance ? FULL : EMPTY;
                  break;
               }
        }
      }
    }
}

void symmetrize(GridMatrix<int>& mat)
{
   for(int j = 0; j < mat.rowNum; ++j)
      for(int i = 0; i < mat.getLogicColNum(j) / 2; ++i)
         mat.data[mat.getLogicColNum(j) - i - 1][j] = mat.data[i][j];
}

void normalize(float arr[], int n)
{
   float sum = 0.f;
   
   for(int i = 0; i < n; ++i)
       sum += arr[i];
   
   for(int i = 0; i < n; ++i)
      arr[i] /= sum;
}

void colorize(GridMatrix<int>& result, GridMatrix<int>& skeleton)
{ 
  std::default_random_engine generator;
  std::array<float, TYPENUM+1> intervals{0, 1, 2, 3, 4, 5, 6, 7};
  std::array<float, TYPENUM+1> baseWeights {10, 10, 10, 1, 1, 0, 0, 0};
  std::array<float, TYPENUM+1> weights;
  
  float clumping = 1.f;
  
   for(int j = 0; j < skeleton.rowNum; ++j)
     for(int i = 0; i < skeleton.getLogicColNum(j); ++i)
       if(skeleton.data[i][j] == FULL)
       {
          for(int k = 0; k < weights.size(); ++k)
             weights[k] = baseWeights[k];
          
          for(int k = 0; k < 6; ++k)
            if(result.getNeighbor(i, j, k) > 0)
               weights[result.getNeighbor(i, j, k) - 1] *= clumping;
        
          std::piecewise_constant_distribution<float> distribution (intervals.begin(),intervals.end(),weights.begin());
          result.data[i][j] = distribution(generator) + 1;
       }
}

float rayVsCircle(Vec2f const& src, Vec2f const& dir, Vec2f const& center, float rad)
{
    float a = dir.dot(dir);
    float b = 2 * (src - center).dot(dir);
    float c = (src - center).dot(src - center) - rad * rad;
    float d = b*b - 4.f * a * c;
    
    if(d >= 0.f)
    {
        d = sqrt(d);
        float t1 = (-b + d) / (2.f * a);
        float t2 = (-b - d) / (2.f * a);
        return MIN(t1, t2);
    }
    
    return -1.f;
}

int sgni(float f)
{
   return f < 0 ? -1 : 1;
}

int reflectionSequence(int i, int n)
{
   int t = Util::wrapi(i, n * 2);
   return t < n ? t : 2 * n - t - 1; 
}

int reflectionSequence(int i, int min, int max)
{
   return min + reflectionSequence(i - min, max - min + 1);
}

int raytrace(GridMatrix<int>& grid, Vec2f orig, Vec2f dir)
{
     float cellW = grid.cellSize * 0.5f;
     float cellH = grid.cellSize * (float)sqrt(3.f) * 0.5f;
     
     Vec2f realOrig = orig, realDir = dir;
     
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
     
     int minx = -grid.colNum * 2;
     int maxx = grid.colNum * 4;
     
     while(x >= minx && y >= 0 && x < maxx && y < grid.rowNum)
     {
        int i = (x - (y % 2)) / 2;
        int type = grid.at(i, y);
        
        Vec2f center(grid.toWx(i, y), grid.toWy(y));
        
        if(type != 0)
        {
            Vec2f center(grid.toWx(i, y), grid.toWy(y));
            
            //if(rayVsCircle(realOrig, realDir, center, CELL_SIZE / 2) > 0.f)
               return type;
           
        }
        
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
     
     return 0;
}

/*
int raytrace(GridMatrix<int>& mat, Vec2f const& src, Vec2f const& dir)
{
    float mind = 1000000.f, minc = 0;
 
    for(int j = 0; j < mat.rowNum; ++j)
      for(int i = 0; i < mat.colNum; ++i)
         if(mat.data[i][j])
         {
             Vec2f center(mat.toWx(i, j), mat.toWy(j));
             float d = rayVsCircle(src, dir, center, CELL_SIZE / 2);
             
             if(d >= 0.f && d < mind)
             {
                 mind = d;
                 minc = mat.data[i][j];
             }
         }
         
    return minc;
}*/

void test(int n)
{
    GridMatrix<int> mat(11, 20, CELL_SIZE, true, 0);    
    GridMatrix<int> result(11, 20, CELL_SIZE, true, 0);
    
    Vec2f src(mat.getWidth() / 2, mat.getHeight() - 20), dest;
    
    mat.clear(0);
    result.clear(0);
        
    skeleton(mat, 0.6f);
    colorize(result, mat);
    symmetrize(result);
    
    int sum = 0;
    
    for(int q = 0; q < n; ++q)
    {
       float range = 2.5f;
       float da = range / 100.f;
       Vec2f dir = Vec2f(0.f, -1.f).rotated(-range / 2);
     
       for(int i = 0; i < 100; ++i)
       {
          dir = dir.rotated(da);
          sum += raytrace(result, src, dir);
       }
       
       if(q % 1000)
       {
          printf("%f\n", (float)q / (float)n * 100.f);
       }
    }
    
    printf("%d\n", sum);
}

int main()
{
    bool exit = false;
    
    init();
    
    int cols[TYPENUM] = {makecol(255, 0, 0), makecol(0, 255, 0), makecol(0, 0, 255),
                         makecol(255, 255, 0), makecol(0, 255, 255), makecol(255, 0, 255),
                         makecol(255, 255, 255)};
    
    GridMatrix<int> mat(11, 20, CELL_SIZE, true, 0);    
    GridMatrix<int> result(11, 20, CELL_SIZE, true, 0);
    
    Vec2f src(mat.getWidth() / 2, mat.getHeight() - 20), dest;
    
    int t1 = time(NULL);
    //test(24000);
    printf("%d secs\n", time(NULL) - t1);
    
    for(int i = 1; i < 30; ++i)
      printf("%d", reflectionSequence(i, 1, 4));
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;

     if(key[KEY_SPACE])
     {
        mat.clear(0);
        result.clear(0);
        
        skeleton(mat, 0.6f);
        colorize(result, mat);
        symmetrize(result);
        rest(200);
     }
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     for(int j = 0; j < mat.rowNum; ++j)
       for(int i = 0; i < mat.getLogicColNum(j); ++i)
       {
          int type = result.rat(i, j);
          
          if(type > 0)
             circlefill(buffer, int(mat.toWx(i, j)), int(mat.toWy(j)), mat.cellSize / 2 - 1, cols[type - 1]);
          else
             circle(buffer, int(mat.toWx(i, j)), int(mat.toWy(j)), mat.cellSize / 2, makecol(50, 50, 50));
       }
     
     float range = 2.5f;
     float da = range / 100.f;
     Vec2f dir = Vec2f(0.f, -1.f).rotated(-range / 2);
     
     for(int i = 0; i < 100; ++i)
     {
        dir = dir.rotated(da);
        int type = raytrace(result, src, dir);
        int col = type > 0 ? cols[type - 1] : makecol(128, 128, 128);
        
        DrawVec2f::line(buffer, src, src + dir * 1000, col);
     }
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()