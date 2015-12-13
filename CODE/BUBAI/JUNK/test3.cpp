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

struct HitInfo {
    int parkI, parkJ;
    
    HitInfo()
    {
       this->parkI = -1;
       this->parkJ = -1;
    }
    
    HitInfo(int parkI, int parkJ)
    {
        this->parkI = parkI;
        this->parkJ = parkJ;
    }
};

int raytrace(GridMatrix<int>& grid, Vec2f orig, Vec2f dir, int refs = 0)
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
     
     int hit = 0;
     
     while(y >= 0 && y < grid.rowNum)
     {
        int i = (x - (y % 2)) / 2;
        hit = grid.at(i, y);
        
        if(hit != 0)
        {
            Vec2f center(grid.toWx(i, y), grid.toWy(y));
            
            //if(rayVsCircle(realOrig, realDir, center, CELL_SIZE / 2) > 0.f)
            break;
        }
        
        if(tMaxX < tMaxY)
        {
           t = tMaxX;
           tMaxX += tDeltaX;
           x += stepX;
           
           if((x == 0 && stepX < 0) || (x == grid.colNum * 2 - 1 && stepX > 0))
           {
              if(refs > 0)
                hit = raytrace(grid, realOrig + realDir * (t * 0.999f) * cellW, Vec2f(-realDir.x, realDir.y), refs - 1);
              
              break;
           }
        }
        
        else
        {
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
        }
     }
     
     int cols[TYPENUM + 1] = {makecol(128, 128, 128), makecol(255, 0, 0), makecol(0, 255, 0), makecol(0, 0, 255),
                              makecol(255, 255, 0), makecol(0, 255, 255), makecol(255, 0, 255),
                              makecol(255, 255, 255)};
     
     DrawVec2f::line(buffer, realOrig, realOrig + realDir * t * cellW, cols[hit]);
     
     return hit;
}

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
          sum += raytrace(result, src, dir, 1);
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
     
     int resolution = 100;
     float range = 2.5f;
     float da = range / (resolution + 1);
     Vec2f dir = Vec2f(0.f, -1.f).rotated(-range / 2);
     
     for(int i = 0; i < resolution; ++i)
     {
        dir = dir.rotated(da);
        int type = raytrace(result, src, dir, 1);
        int col = type > 0 ? cols[type - 1] : makecol(128, 128, 128);
        
        //DrawVec2f::line(buffer, src, src + dir * 1000, col);
     }
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
