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

void tangents(Vec2f& t1, Vec2f& t2, Vec2f const& c, float r, Vec2f const& p)
{
   Vec2f d = c - p;
   float invlen = 1.f / d.length();
   d *= invlen;
   float sina = r * invlen;
   float cosa = cos(asin(sina));
   
   t1 = d.rotated(cosa, sina);
   t2 = d.rotated(cosa, -sina);
}

Vec2f tangent(Vec2f const& c, float r, Vec2f const& p, float sgn)
{
   Vec2f d = c - p;
   float invlen = 1.f / d.length();
   d *= invlen;
   float sina = r * invlen;
   float cosa = cos(asin(sina));
   
   Vec2f t = d.rotated(cosa, sina * sgn);
}

bool lineVsCircle(Vec2f const& src, Vec2f const& dir, Vec2f const& c, float r)
{
    Vec2f n = dir.normal();
    float proj = (c - src).dot(n);
    
    return proj > -r && proj < r;
}

bool block(Vec2f& t1, Vec2f& t2, Vec2f const& src, Vec2f const& c, float rad)
{
     bool f1 = lineVsCircle(src, t1, c, rad);
     bool f2 = lineVsCircle(src, t2, c, rad);
     
     if(f1 && !f2)
     {
       t1 = tangent(c, rad, src, -1.f);
       return false;
     }
     
     else if(f2 && !f1)
     {
       t2 = tangent(c, rad, src, 1.f);
       return false;
     }
     
     else if(f2 && f1)
     {
       return true;
     }
     
     else
       return false;
}

bool occlusion(Vec2f& t2, Vec2f& t1, GridMatrix<int>& grid, Vec2f const& src, Vec2f const& dest, float rad)
{
    bool occluded = false;
 
     int maxj = grid.toBy(src.y) - 1;
     int minj = MAX(grid.toBy(dest.y) - 1, 0);
     
     for(int j = maxj; j >= minj; --j)
     {
        float y = grid.toWy(j);
        float ymin = grid.toWy(j - 0.5f);
        float ymax = grid.toWy(j + 0.5f);
        
        float x0ymin = src.x + (ymin - src.y) / t1.y * t1.x;
        float x0ymax = src.x + (ymax - src.y) / t1.y * t1.x;
        
        float x1ymin = src.x + (ymin - src.y) / t2.y * t2.x;
        float x1ymax = src.x + (ymax - src.y) / t2.y * t2.x;
        
        float x0 = MIN(x0ymin, x0ymax);
        float x1 = MAX(x1ymin, x1ymax);
        
        int mini = MAX(0, int(floor(grid.toBx(x0, y))) - 1);
        int maxi = MIN(ceil(grid.toBx(x1, y)) + 1, grid.getLogicColNum(j));

        //if(mini < maxi)
        for(int i = mini; i < maxi; ++i)
        {
           Vec2f c(grid.toWx(i, j), grid.toWy(j));
            DrawVec2f::circlefill(buffer, c, 2, 0);
           
           if(grid.at(i, j) != 0 && (c - dest).dot((dest - src).unit()) < 0.f)
              if(block(t2, t1, src, c, rad))
                 return true;
              
           
        }
     }
     
     return false;
}

int main()
{
    bool exit = false;
    
    init();
    
    GridMatrix<int> mat(11, 20, CELL_SIZE, true, 0);
    int cols[TYPENUM] = {makecol(255, 0, 0), makecol(0, 255, 0), makecol(0, 0, 255),
                         makecol(255, 255, 0), makecol(0, 255, 255), makecol(255, 0, 255),
                         makecol(255, 255, 255)};

    
    GridMatrix<int> result(11, 20, CELL_SIZE, true, 0);
    
    Vec2f src(mat.getWidth() / 2, mat.getHeight() - 20), dest;
    
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
     
     int mouseI = result.toBx(mouse_x, mouse_y), mouseJ = result.toBy(mouse_y);
     dest.init(result.toWx(mouseI, mouseJ), result.toWy(mouseJ));
     
     bool blocked = false;
     Vec2f t1, t2;
     tangents(t1, t2, dest, CELL_SIZE, src);

     clear_to_color(buffer, makecol(64, 64, 64));
     
      
     
     for(int j = 0; j < mat.rowNum; ++j)
       for(int i = 0; i < mat.getLogicColNum(j); ++i)
       {
          int type = result.at(i, j);
          
          if(type > 0)
          {
             circlefill(buffer, int(mat.toWx(i, j)), int(mat.toWy(j)), mat.cellSize / 2 - 1, cols[type - 1]);
             circle(buffer, int(mat.toWx(i, j)), int(mat.toWy(j)), mat.cellSize, cols[type - 1]);
          }
          else
             circle(buffer, int(mat.toWx(i, j)), int(mat.toWy(j)), mat.cellSize / 2, makecol(50, 50, 50));
       }
     
     blocked = occlusion(t1, t2, result, src, dest, CELL_SIZE);
     
     DrawVec2f::circle(buffer, dest, CELL_SIZE / 2, makecol(255, 255, 255));
     DrawVec2f::circle(buffer, dest, CELL_SIZE, makecol(255, 255, 255));
     DrawVec2f::circlefill(buffer, src, CELL_SIZE / 2, makecol(255, 255, 0));
     
     if(!blocked)
     {
       DrawVec2f::line(buffer, src, src + t1 * 1000, makecol(255, 255, 255));
       DrawVec2f::line(buffer, src, src + t2 * 1000, makecol(255, 255, 255));
     }
     
     //Vec2f n1 = (dest - src).normal().unit() * (CELL_SIZE * 0.5f);
     //DrawVec2f::line(buffer, src + n1, dest + n1, makecol(255, 255, 255));
     //DrawVec2f::line(buffer, src - n1, dest - n1, makecol(255, 255, 255));
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
