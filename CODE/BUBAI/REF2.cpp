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
#include "Generator.hpp"

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
#define RES 100
#define RANGE 2.5f

struct HitInfo {
    bool hit;
    int parkI, parkJ;
    int hitI, hitJ;
    
    HitInfo()
    {
       this->hit = false;
       this->parkI = -1;
       this->parkJ = -1;
    }
    
    HitInfo(int hitI, int hitJ, int parkI, int parkJ)
    {
        this->hit = true;
        this->parkI = parkI;
        this->parkJ = parkJ;
        this->hitI = hitI;
        this->hitJ = hitJ;
    }
};

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

HitInfo raytrace(GridMatrix<int>& grid, Vec2f orig, Vec2f dir, int refs = 1, bool vis=false)
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
     
     HitInfo hitInfo;
     int lastI = (x - (y % 2)) / 2, lastJ = y;
     
     while(y >= 0 && y < grid.rowNum)
     {
        int i = (x - (y % 2)) / 2;

        if((i != lastI || y != lastJ) && grid.at(i, y) != 0) // Collision?
        {
            //Vec2f center(grid.toWx(i, y), grid.toWy(y));          
            //if(rayVsCircle(realOrig, realDir, center, CELL_SIZE / 2) > 0.f)
            
            hitInfo = HitInfo(i, y, lastI, lastJ);
            break;
        }
        
        if(tMaxX < tMaxY) // Horizontal movement?
        {
           lastI = i;
           lastJ = y;
         
           t = tMaxX;
           tMaxX += tDeltaX;
           x += stepX;
           
           if((x == 0 && stepX < 0) || (x == grid.colNum * 2 - 1 && stepX > 0)) // Reflection?
           {
              if(refs > 0)
                hitInfo = raytrace(grid, realOrig + realDir * (t * 0.999f) * cellW, 
                                         Vec2f(-realDir.x, realDir.y), refs - 1, vis);
              break;
           }
        }
        
        else // Vertical movement?
        {
            lastI = i;
            lastJ = y;
            t = tMaxY;
            tMaxY += tDeltaY;
            y += stepY;
            
            if(y < 0)
            {
            	hitInfo = HitInfo(i, y, lastI, lastJ);
            	break;
            }
        }
     }
     
     if(vis)
     {
        int cols[TYPENUM] = {makecol(255, 0, 0), makecol(0, 255, 0), makecol(0, 0, 255),
                             makecol(255, 255, 0), makecol(0, 255, 255), makecol(255, 0, 255),
                             makecol(255, 255, 255)};
     
        if(hitInfo.hit)
        {
        	if(hitInfo.hitJ < 0)
        		DrawVec2f::line(buffer, realOrig, realOrig + realDir * t * cellW, makecol(128, 128, 128));
        	else
           		DrawVec2f::line(buffer, realOrig, realOrig + realDir * t * cellW,
                                cols[grid.at(hitInfo.hitI, hitInfo.hitJ) - 1]);
        }
        else
           DrawVec2f::line(buffer, realOrig, realOrig + realDir * t * cellW, makecol(128, 128, 128));
     }
     
     return hitInfo;
}

HitInfo shootDebugRay(GridMatrix<int>& grid, Vec2f const& src, float range, int refs, int res, int n)
{
   Vec2f dir = Vec2f(0.f, -1.f).rotated(-range / 2 + range / (res + 1) * n);
   HitInfo info = raytrace(grid, src, dir, refs, true);
        
   if(info.hit)
   {
       circlefill(buffer, int(grid.toWx(info.parkI, info.parkJ)),
                          int(grid.toWy(info.parkJ)), 5, makecol(255, 255, 255));
                             
       circlefill(buffer, int(grid.toWx(info.hitI, info.hitJ)),
                          int(grid.toWy(info.hitJ)), 5, 0);
   }
   
   return info;
}

//void test(int n)
//{
//    GridMatrix<int> mat(11, 20, CELL_SIZE, true, 0);    
//    GridMatrix<int> result(11, 20, CELL_SIZE, true, 0);
//    
//    Vec2f src(mat.getWidth() / 2, mat.getHeight() - 20), dest;
//    
//    mat.clear(0);
//    result.clear(0);
//        
//    skeleton(mat, 0.6f);
//    colorize(result, mat);
//    symmetrize(result);
//    
//    int sum = 0;
//    
//    Vec2f dir = Vec2f(0.f, -1.f).rotated(-RANGE / 2);
//    Vec2f dirs[RES];
//    float da = RANGE / RES;
//    
//    for(int i = 0; i < RES; ++i)
//       dirs[i] = dir.rotated(da);
//    
//    for(int q = 0; q < n; ++q)
//    {
//       for(int i = 0; i < RES; ++i)
//          raytrace(result, src, dirs[i], 1);
//
//       if(q % 1000 == 0)
//       {
//          printf("%f\n", (float)q / (float)n * 100.f);
//       }
//    }
//    
//    printf("%d\n", sum);
//}


struct Cell {
	int i, j, type;
	
	Cell(int pi, int pj, int ptype):
		i(pi), j(pj), type(ptype)
	{
	}
};

class Game {
	GridMatrix<int> mGrid;
	
public:
	Game(int colNum, int rowNum, float cellSize, bool symmetric):
		mGrid(colNum, rowNum, cellSize, symmetric, 0)
	{
		
	}
	
	Game(Game const& other):
		mGrid(other.mGrid)
	{
	}
	
	virtual ~Game()
	{
		
	}
	
	GridMatrix<int> const& grid() const { return mGrid; }
	GridMatrix<int>& grid() { return mGrid; }
	
	void draw(BITMAP* buffer, Vec2f const& cam, int cols[])
	{
		for(int j = 0; j < mGrid.rowNum; ++j)
    		for(int i = 0; i < mGrid.getLogicColNum(j); ++i)
       		{
          		int type = mGrid.at(i, j);
          
          		if(type > 0)
             		circlefill(buffer, int(mGrid.toWx(i, j) - cam.x), int(mGrid.toWy(j) - cam.y), mGrid.cellSize / 2 - 1, cols[type - 1]);
          		else
             		circle(buffer, int(mGrid.toWx(i, j) - cam.x), int(mGrid.toWy(j) - cam.y), mGrid.cellSize / 2, makecol(50, 50, 50));
       		}
       		
       	Vec2f cp = cannonPos();
       	
       	circlefill(buffer, int(cp.x - cam.x), int(cp.y - cam.y), mGrid.cellSize / 2 - 1, cols[getShot(0) - 1]);
       	circlefill(buffer, int(cp.x - cam.x), int(cp.y + mGrid.cellSize - cam.y), mGrid.cellSize / 2 - 1, cols[getShot(1) - 1]);
	}
	
	void drawAim(BITMAP* buffer, Vec2f const& dest, Vec2f const& cam)
	{
		Vec2f dir = (dest - cannonPos()).unit();
   		HitInfo info = raytrace(mGrid, cannonPos(), dir, 2, true);
        
   		if(info.hit)
   		{
       		circlefill(buffer, int(mGrid.toWx(info.parkI, info.parkJ)),
                          		int(mGrid.toWy(info.parkJ)), 5, makecol(255, 255, 255));
        
       		circlefill(buffer, int(mGrid.toWx(info.hitI, info.hitJ)),
                          		int(mGrid.toWy(info.hitJ)), 5, 0);
   		}
	}
	
	virtual int getShot(int n) = 0;
	virtual Vec2f cannonPos() = 0;
	virtual bool onCollision(GridMatrix<int>& grid, Cell const& src, int shotType) = 0;
	virtual void onPark(GridMatrix<int>& grid, Cell const& src) = 0;
	virtual void onPop(GridMatrix<int>& grid, Cell const& src) = 0;
	virtual void onNeighborPark(GridMatrix<int>& grid, Cell const& src, Cell const& trigger) = 0;
	virtual void onNeighborPop(GridMatrix<int>& grid, Cell const& src, Cell const& trigger) = 0;
};

class PiggyGame: public Game {	
public:
	PiggyGame(int colNum, int rowNum, float cellSize): Game(colNum, rowNum, cellSize, true)
	{
		GridMatrix<int> skel(colNum, rowNum, cellSize, true, 0);    
		
		for(int i = 0; i < 100 || count(grid()) < 10; ++i)
		{
			skel.clear(0);
        	grid().clear(0);
        
        	skeleton(skel, 0.5f, 0.95f, 0.85f, -0.05f);
        	colorize(grid(), skel, {1, 1, 1}, 1.f);
        	symmetrize(grid());
        }
	}
	
	int getShot(int n)
	{
		return 1;
	}
	
	Vec2f cannonPos()
	{
		return Vec2f(grid().getWidth() / 2, grid().getHeight() - 100);
	}
	
	bool onCollision(GridMatrix<int>& grid, Cell const& src, int shotType)
	{
		
	}
	
	void onPark(GridMatrix<int>& grid, Cell const& src)
	{
		
	}
	
	void onPop(GridMatrix<int>& grid, Cell const& src)
	{
		
	}
	
	void onNeighborPark(GridMatrix<int>& grid, Cell const& src, Cell const& trigger)
	{
		
	}
	
	void onNeighborPop(GridMatrix<int>& grid, Cell const& src, Cell const& trigger)
	{
		
	}
};

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
    
    PiggyGame piggyGame(11, 20, CELL_SIZE);
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;

     if(key[KEY_SPACE])
     {
        mat.clear(0);
        result.clear(0);
        
        skeleton(mat, 0.5f, 0.95f, 0.85f, -0.05f);
        colorize(result, mat, {1, 1, 1}, 1.f);
        symmetrize(result);
        rest(200);
     }
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     for(int j = 0; j < mat.rowNum; ++j)
       for(int i = 0; i < mat.getLogicColNum(j); ++i)
       {
          int type = result.at(i, j);
          
          if(type > 0)
             circlefill(buffer, int(mat.toWx(i, j)), int(mat.toWy(j)), mat.cellSize / 2 - 1, cols[type - 1]);
          else
             circle(buffer, int(mat.toWx(i, j)), int(mat.toWy(j)), mat.cellSize / 2, makecol(50, 50, 50));
       }
     
     for(int i = 0; i < RES; ++i)
       shootDebugRay(result, src, RANGE, 1, RES, i);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
