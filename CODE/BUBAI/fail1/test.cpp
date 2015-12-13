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



struct Line {
   Vec2f a, b;
   
   Line(float x1, float y1, float x2, float y2): a(x1, y1), b(x2, y2)
   {
   }
   
   Line()
   {
   }
};

Line SILHOUETTES[16];
Vec2f CELL_MIN;
Vec2f CELL_MAX;

void initLuts(float cellW, float cellH)
{ 
   CELL_MIN.init(-cellW / 2, -cellH / 2); 
   CELL_MAX.init(cellW / 2, cellH / 2);
  
   SILHOUETTES[1] = Line(CELL_MIN.x, CELL_MIN.y, CELL_MIN.x, CELL_MAX.y);
   SILHOUETTES[2] = Line(CELL_MAX.x, CELL_MAX.y, CELL_MAX.x, CELL_MIN.y);
   SILHOUETTES[4] = Line(CELL_MAX.x, CELL_MIN.y, CELL_MIN.x, CELL_MIN.y);
   SILHOUETTES[5] = Line(CELL_MAX.x, CELL_MIN.y, CELL_MIN.x, CELL_MAX.y);
   SILHOUETTES[6] = Line(CELL_MAX.x, CELL_MAX.y, CELL_MIN.x, CELL_MIN.y);
   SILHOUETTES[8] = Line(CELL_MIN.x, CELL_MAX.y, CELL_MAX.x, CELL_MAX.y);
   SILHOUETTES[9] = Line(CELL_MIN.x, CELL_MIN.y, CELL_MAX.x, CELL_MAX.y);
   SILHOUETTES[10] = Line(CELL_MIN.x, CELL_MAX.y, CELL_MAX.x, CELL_MIN.y);
}

// Returned points are in clockwise order relative to p!
Line silhouette(Vec2f p, Vec2f const& boxCenter)
{
   p -= boxCenter;
 
   int i = (p.x < CELL_MIN.x) +
           ((p.x > CELL_MAX.x) << 1) +
           ((p.y < CELL_MIN.y) << 2) +
           ((p.y > CELL_MAX.y) << 3);
   
   Line result = SILHOUETTES[i];
   result.a += boxCenter;
   result.b += boxCenter;
   
   return result;
}

bool between(Vec2f const& v, Vec2f const& a, Vec2f const& b)
{
    return a.normal().dot(v) >= 0.f && b.normal().dot(v) <= 0.f;
}

bool occlude(Vec2f& dest1, Vec2f& dest2, Vec2f const& src, Vec2f const& occCenter)
{
    Line sil = silhouette(src, occCenter);
    Vec2f a = dest1 - src, b = dest2 - src, c = sil.a - src, d = sil.b - src;
    
    if(between(a, c, d))
    {
        if(between(b, c, d))
           return true;
        else
        {
           dest1 = sil.b;
           return false;
        }
    }
    
    else
    {
        if(between(b, c, d))
            dest2 = sil.a;
        return false;
    }
}


bool occlusion(Vec2f& dest1, Vec2f& dest2, GridMatrix<int>& grid,
                Vec2f const& orig, int destI, int destJ)
{
    Vec2f dir1 = dest1 - orig;
    Vec2f dir2 = dest2 - orig;
    float cellW = grid.cellSize * 0.5f;
    float cellH = grid.cellSize * (float)sqrt(3.f) * 0.5f;
    float iPerJ1 = dir1.x / (dir1.y * cellW / cellH);
    float iPerJ2 = dir2.x / (dir2.y * cellW / cellH);
    float origI = orig.x / cellW;
    float origJ = orig.y / cellH;
    
    float dj1 = iPerJ1 > 0 ? 0 : 1, dj2 = iPerJ2 > 0 ? 1 : 0;
    
    int minj = destJ - 1;
    int maxj = grid.toBy(orig.y) - 1;
    
    float mini = origI + iPerJ1 * (maxj + dj1 - origJ);
    float maxi = origI + iPerJ2 * (maxj + dj2 - origJ);
    int dir = orig.x < grid.toWx(destI, destJ) ? 1 : -1;
    
    bool reachedTarget = false;
    
    for(int j = maxj; j >= minj; --j)
    {
        int realMinI = MAX(0, (int(mini) - (j % 2)) / 2);
        int realMaxI = MIN(grid.colNum - 1, (int(maxi) - (j % 2)) / 2);
        int start, stop;
        
        if(dir == 1)
           for(int i = realMinI; i <= realMaxI; ++i)
           {
               if(i == destI && j == destJ)
                  reachedTarget = true;
           
               Vec2f c(grid.toWx(i, j), grid.toWy(j));
               DrawVec2f::circlefill(buffer, c, 2, 0);
           
               if(grid.rat(i, j) != 0)
               {
                  if(reachedTarget)
                    return true;
                  else if(occlude(dest1, dest2, orig, c))
                    return false;
               }
           }
        
        else
           for(int i = realMaxI; i >= realMinI; --i)
           {
               if(i == destI && j == destJ)
                  reachedTarget = true;
           
               Vec2f c(grid.toWx(i, j), grid.toWy(j));
               DrawVec2f::circlefill(buffer, c, 2, 0);
           
               if(grid.rat(i, j) != 0)
               {
                  if(reachedTarget)
                    return true;
                  else if(occlude(dest1, dest2, orig, c))
                    return false;
               }
           }
        
        mini -= iPerJ1;
        maxi -= iPerJ2;
    }

    return false;
}






void box(BITMAP* buffer, float x, float y, float r, int col)
{
   float JFACTOR = (float)sqrt(3.f) * 0.5f;
 
   rect(buffer, int(x - r), int(y - r * JFACTOR), int(x + r), int(y + r * JFACTOR), col);
}


void test(int max)
{
    initLuts(CELL_SIZE, CELL_SIZE * (float)sqrt(3.f) * 0.5f);
    
    GridMatrix<int> mat(11, 20, CELL_SIZE, true, 0);    
    GridMatrix<int> result(11, 20, CELL_SIZE, true, 0);
    Vec2f src(mat.getWidth() / 2, mat.getHeight() - 20), dest;
    
    mat.clear(0);
    result.clear(0);
        
    skeleton(mat, 0.6f);
    colorize(result, mat);
    symmetrize(result);
    
    for(int counter = 0; counter < max; ++counter)
    { 
     int mouseI = rand() % result.colNum;
     int mouseJ = rand() % result.rowNum;
     dest.init(result.toWx(mouseI, mouseJ), result.toWy(mouseJ));
     
     Line sil = silhouette(src, dest);
     
     
     occlusion(sil.a, sil.b, result, src, mouseI, mouseJ);
     
     if(!(counter % 1000))
       printf("%f\n", float(counter) / float(max) * 100.f);
    }
}


int main()
{
    bool exit = false;
    
    init();
    
    initLuts(CELL_SIZE, CELL_SIZE * (float)sqrt(3.f) * 0.5f);
    
    GridMatrix<int> mat(11, 20, CELL_SIZE, true, 0);
    int cols[TYPENUM] = {makecol(255, 0, 0), makecol(0, 255, 0), makecol(0, 0, 255),
                         makecol(255, 255, 0), makecol(0, 255, 255), makecol(255, 0, 255),
                         makecol(255, 255, 255)};

    
    GridMatrix<int> result(11, 20, CELL_SIZE, true, 0);
    
    Vec2f src(mat.getWidth() / 2, mat.getHeight() - 20), dest;
    Vec2f src2(mat.getWidth() + mat.getWidth() / 2, mat.getHeight() - 20);
    
    //int t0 = time(NULL);
    //test(10000000);
    //printf("\n%d SECS\n", time(NULL) - t0);
    
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
     
     Line sil = silhouette(src, dest);
     Line sil2 = silhouette(src2, dest);

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
     
     box(buffer, dest.x, dest.y, CELL_SIZE / 2, makecol(255, 255, 255));
     DrawVec2f::circlefill(buffer, src, 2, makecol(255, 255, 0));
     
     int maxj = result.toBy(src.y) - 1;
     int minj = MAX(result.toBy(dest.y), 0);
     
     if(occlusion(sil.a, sil.b, result, src, mouseI, mouseJ))
     {
         DrawVec2f::line(buffer, src, src + (sil.a - src).unit() * 1000.f, makecol(255, 255, 0));
         DrawVec2f::line(buffer, src, src + (sil.b - src).unit() * 1000.f, makecol(255, 255, 0));
     }
     
     /*
     if(!occlusion(sil2.a, sil2.b, result, src2, dest, CELL_SIZE / 2, minj, maxj))
     {
         DrawVec2f::line(buffer, src2, src2 + (sil2.a - src2).unit() * 1000.f, makecol(255, 255, 0));
         DrawVec2f::line(buffer, src2, src2 + (sil2.b - src2).unit() * 1000.f, makecol(255, 255, 0));
     }*/
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
