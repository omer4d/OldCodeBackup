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

#include "Flood.hpp"

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
    int hitI, hitJ, hitType;
    
    HitInfo()
    {
       this->hit = false;
       this->parkI = -1;
       this->parkJ = -1;
    }
    
    HitInfo(int hitI, int hitJ, int hitType, int parkI, int parkJ)
    {
        this->hit = true;
        this->parkI = parkI;
        this->parkJ = parkJ;
        this->hitI = hitI;
        this->hitJ = hitJ;
        this->hitType = hitType;
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

typedef std::function<bool(int, int, int)> RayTracerCallback;
typedef std::function<int(int)> GetTypeColor;

HitInfo raytrace(GridMatrix<int>& grid, Vec2f orig, Vec2f dir, int refs = 1, RayTracerCallback const& callback=nullptr, bool vis=false, GetTypeColor getTypeColor=nullptr)
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
        int cellContent = grid.at(i, y);

        if((i != lastI || y != lastJ) && cellContent != 0) // Collision?
        {
            //Vec2f center(grid.toWx(i, y), grid.toWy(y));          
            //if(rayVsCircle(realOrig, realDir, center, CELL_SIZE / 2) > 0.f)
            
            if(!callback || callback(i, y, cellContent))
            {
            	hitInfo = HitInfo(i, y, cellContent, lastI, lastJ);
            	break;
            }
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
                                         Vec2f(-realDir.x, realDir.y), refs - 1, callback, vis, getTypeColor);
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
            	hitInfo = HitInfo(i, y, 0, lastI, lastJ);
            	break;
            }
        }
     }
     
     if(vis)
     {
        //int cols[TYPENUM] = {makecol(255, 0, 0), makecol(0, 255, 0), makecol(0, 0, 255),
        //                     makecol(255, 255, 0), makecol(0, 255, 255), makecol(255, 0, 255),
        //                     makecol(255, 255, 255)};
     
        if(hitInfo.hit && getTypeColor)
        {
        	if(hitInfo.hitJ < 0)
        		DrawVec2f::line(buffer, realOrig, realOrig + realDir * t * cellW, makecol(128, 128, 128));
        	else
           		DrawVec2f::line(buffer, realOrig, realOrig + realDir * t * cellW,
                                getTypeColor(grid.at(hitInfo.hitI, hitInfo.hitJ)));
        }
        else
           DrawVec2f::line(buffer, realOrig, realOrig + realDir * t * cellW, makecol(128, 128, 128));
     }
     
     return hitInfo;
}

HitInfo shootDebugRay(GridMatrix<int>& grid, Vec2f const& src, float range, int refs, int res, int n)
{
   Vec2f dir = Vec2f(0.f, -1.f).rotated(-range / 2 + range / (res + 1) * n);
   HitInfo info = raytrace(grid, src, dir, refs, nullptr, true);
        
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
	
	virtual ~Game()
	{
	}
	
	GridMatrix<int> const& grid() const { return mGrid; }
	GridMatrix<int>& grid() { return mGrid; }
	
	void draw(BITMAP* buffer, Vec2f const& cam, GetTypeColor getTypeColor)
	{
		for(int j = 0; j < mGrid.rowNum; ++j)
    		for(int i = 0; i < mGrid.getLogicColNum(j); ++i)
       		{
          		int type = mGrid.at(i, j);
          
          		if(type > 0)
             		circlefill(buffer, int(mGrid.toWx(i, j) - cam.x), int(mGrid.toWy(j) - cam.y), mGrid.cellSize / 2 - 1, getTypeColor(type));
          		else
             		circle(buffer, int(mGrid.toWx(i, j) - cam.x), int(mGrid.toWy(j) - cam.y), mGrid.cellSize / 2, makecol(50, 50, 50));
       		}
       		
       	Vec2f cp = cannonPos();
       	
       	circlefill(buffer, int(cp.x - cam.x), int(cp.y - cam.y), mGrid.cellSize / 2 - 1, getTypeColor(getShot(0)));
       	circlefill(buffer, int(cp.x - cam.x), int(cp.y + mGrid.cellSize - cam.y), mGrid.cellSize / 2 - 1, getTypeColor(getShot(1)));
	}
	
	void drawAim(BITMAP* buffer, Vec2f const& dest, Vec2f const& cam, GetTypeColor getTypeColor, RayTracerCallback const& callback=nullptr)
	{
		Vec2f dir = (dest - cannonPos()).unit();
   		HitInfo info = raytrace(mGrid, cannonPos(), dir, 1, callback, true, getTypeColor);
        
   		if(info.hit)
   		{
       		circlefill(buffer, int(mGrid.toWx(info.parkI, info.parkJ)),
                          		int(mGrid.toWy(info.parkJ)), 5, makecol(255, 255, 255));
        
       		circlefill(buffer, int(mGrid.toWx(info.hitI, info.hitJ)),
                          		int(mGrid.toWy(info.hitJ)), 5, 0);
   		}
	}
	
	int getGridStats(int typeCounter[])
	{
		int maxJ = 0;
		
		for(int j = 0; j < grid().rowNum; ++j)
		{
			bool found = false;
			
			for(int i = 0; i < grid().getLogicColNum(j); ++i)
			{
				int type = grid().at(i, j);
				
				if(type)
				{
					maxJ = j;
					++typeCounter[type];
					found = true;
					break;
				}
			}
			
			if(!found)
				break;
		}
		
		return maxJ;
	}
	
	bool hasMarkedNeighbors(Flood<int>& flood, int i, int j)
	{
		for(int k = 0; k < 6; ++k)
			if(flood.getMarkerAt(mGrid.getNeighborI(i, j, k), mGrid.getNeighborJ(i, j, k)))
				return true;
		
		return false;
	}
	
	int pop(Flood<int>& flood, int i, int j, int shot)
	{
		int shotGroup = getGroup(shot);
		int score = 0;
		
		int lumpSize = flood.run(i, j, false, [=](int t, int i, int j) {
			return getGroup(t) == shotGroup; 
		});
		
		if(lumpSize >= 3)
			for(int j = 0; j < mGrid.rowNum; ++j)
				for(int i = 0; i < mGrid.getLogicColNum(j); ++i)
				{
					int type = mGrid.at(i, j);
					
					if(flood.getMarkerAt(i, j))
					{
						score += onPop(mGrid, Cell(i, j, mGrid.at(i, j)));
						grid().set(i, j, 0);
					}
					
					else if(type && hasMarkedNeighbors(flood, i, j))
						score += onNeighborPop(mGrid, Cell(i, j, type), shot);
				}
					
		return score;
	}
	
	int removeFloaters(Flood<int>& flood)
	{
		int score = 0;
		
		flood.reset();
		
		for(int i = 0; i < mGrid.colNum; ++i)
			flood.run(i, 0, false, [](int t, int i, int j) { return t != 0; });
		
		for(int j = 0; j < mGrid.rowNum; ++j)
			for(int i = 0; i < mGrid.getLogicColNum(j); ++i)
			{
				int t = mGrid.at(i, j);
				if(!flood.getMarkMat()[i][j] && t)
				{
					score += onFall(mGrid, Cell(i, j, t));
					mGrid.set(i, j, 0);
				}
			}
			
		return score;
	}
	
	int park(HitInfo const& hitInfo, int shot)
	{
		Flood<int> flood(&mGrid);
		int score = 0;
		
		mGrid.set(hitInfo.parkI, hitInfo.parkJ, shot);
   		score += onPark(mGrid, Cell(hitInfo.parkI, hitInfo.parkJ, shot), Cell(hitInfo.hitI, hitInfo.hitJ, hitInfo.hitType));
   		score += pop(flood, hitInfo.parkI, hitInfo.parkJ, shot) + removeFloaters(flood);
   		
   		for(int k = 0; k < 6; ++k)
   		{
   			int ni = mGrid.getNeighborI(hitInfo.parkI, hitInfo.parkJ, k);
			int nj = mGrid.getNeighborJ(hitInfo.parkI, hitInfo.parkJ, k);
			int nt = mGrid.at(ni, nj);
			
			if(nt != 0)
				score += onNeighborPark(mGrid, Cell(ni, nj, nt), Cell(hitInfo.parkI, hitInfo.parkJ, shot));
   		}
   		
   		return score;
	}
	
	int shoot(Vec2f const& dest)
	{
		int shot = getShot(0);
		Vec2f dir = (dest - cannonPos()).unit();
   		HitInfo info = raytrace(grid(), cannonPos(), dir, 1, [&](int i, int j, int content) {
   			return onCollision(mGrid, Cell(i, j, content), shot);
   		}, false);
   		
   		int score = -1000;
   		
   		if(info.hit)
   		   score = park(info, shot);
   		   
   		onShot();
   		
   		return score;
	}
	
	virtual int getGroup(int type) = 0;
	virtual int getShot(int n) = 0;
	virtual Vec2f cannonPos() = 0;
	virtual bool onCollision(GridMatrix<int>& grid, Cell const& src, int shotType) = 0;					// Called before park.
	virtual int onPark(GridMatrix<int>& grid, Cell const& src, Cell const& trigger) = 0;				// Called after set, before pop.
	virtual int onPop(GridMatrix<int>& grid, Cell const& src) = 0;										
	virtual int onFall(GridMatrix<int>& grid, Cell const& src) = 0;
	virtual int onNeighborPark(GridMatrix<int>& grid, Cell const& src, Cell const& trigger) = 0;		// Called after pop.
	virtual int onNeighborPop(GridMatrix<int>& grid, Cell const& src, int trigger) = 0;
	virtual void onShot() = 0;
};

class PiggyGame: public Game {
public:
	enum BubbleType {
		BUB_NONE,
		BUB_RED, BUB_GREEN, BUB_BLUE,
		BUB_RED_DUCK, BUB_GREEN_DUCK, BUB_BLUE_DUCK,
		BUB_VENT, BUB_SPONGE, BUB_MOLD, BUB_SOAP, BUB_FOAM, BUB_RAINBOW, BUB_MAGIC, BUB_AIR,
		BUB_TYPE_SENTINEL
	};
	
private:
	static constexpr float MIN_DIST_TO_LOWEST_BUB = 100.f;
	
	float mCannonY;
	std::vector<BubbleType> mAmmo;
	
public:
	PiggyGame(int colNum, int rowNum, float cellSize): Game(colNum, rowNum, cellSize, true)
	{
		GridMatrix<int> skel(colNum, rowNum, cellSize, true, 0);    
		std::vector<float> chances(BUB_TYPE_SENTINEL - 1, 0.f);
		
		chances[BUB_RED - 1] = 1.f;
		chances[BUB_GREEN - 1] = 1.f;
		chances[BUB_BLUE - 1] = 1.f;
		
		
		//chances[BUB_RAINBOW - 1] = 1.f;
		//chances[BUB_RED_DUCK - 1] = 1.f;
		
		//chances[BUB_MOLD - 1] = 1.f;
		//chances[BUB_MAGIC - 1] = 1.f;
		//chances[BUB_SPONGE - 1] = 1.f;
		//chances[BUB_AIR - 1] = 1.f;
		//chances[BUB_VENT - 1] = 1.f;
		
		chances[BUB_SOAP - 1] = 1.f;
		
		for(int i = 0; i < 100 || count(grid()) < 10; ++i)
		{
			skel.clear(0);
        	grid().clear(0);
        	
        	skeleton(skel, 0.5f, 0.95f, 0.85f, -0.05f);
        	colorize(grid(), skel, chances, 1.f);
        	symmetrize(grid());
        }
        
        int typeCounters[BUB_TYPE_SENTINEL - 1] = {0};
        int maxJ = getGridStats(typeCounters);
        
        mCannonY = grid().toWy(maxJ) + MIN_DIST_TO_LOWEST_BUB;
        mAmmo.push_back(getShootable(typeCounters));
        mAmmo.push_back(getShootable(typeCounters));
	}
	
	static int getTypeColor(int type)
	{
		switch(type)
		{
			case BUB_RED:
				return makecol(255, 0, 0);
			case BUB_GREEN:
				return makecol(0, 255, 0);
			case BUB_BLUE:
				return makecol(0, 0, 255);
			
			case BUB_RED_DUCK:
				return makecol(128, 0, 0);
			case BUB_GREEN_DUCK:
				return makecol(0, 128, 0);
			case BUB_BLUE_DUCK:
				return makecol(0, 0, 128);
			
			case BUB_VENT:
				return makecol(64, 0, 0);
			case BUB_SPONGE:
				return makecol(255, 255, 0);
			case BUB_MOLD:
				return makecol(32, 32, 32);
			case BUB_SOAP:
				return makecol(128, 255, 255);
			case BUB_FOAM:
				return makecol(255, 255, 255);
			case BUB_RAINBOW:
				return makecol(255, 128, 255);
			case BUB_MAGIC:
				return makecol(100, 100, 255);
			case BUB_AIR:
				return makecol(128, 128, 128);
			
			default:
				return makecol(0, 20, 0);
		}
	}
	
	int removeGroup(int group)
	{
		GridMatrix<int>& gridMat = grid();
		int counter = 0;
		
		for(int j = 0; j < gridMat.rowNum; ++j)
		   for(int i = 0; i < gridMat.getLogicColNum(j); ++i)
		   	if(getGroup(gridMat.at(i, j)) == group)
		   	{
		   		gridMat.set(i, j, 0);
		   		++counter;
		   	}
		   	
		return counter;
	}
	
	int deathScore(int type)
	{
		switch(type)
		{
			case BUB_RED: case BUB_GREEN: case BUB_BLUE:
				return 1;
			case BUB_RED_DUCK: case BUB_GREEN_DUCK: case BUB_BLUE_DUCK:
				return 1000;
			case BUB_VENT: case BUB_MOLD:
				return 3;
			case BUB_SOAP: case BUB_FOAM:
				return 2;
			case BUB_RAINBOW: case BUB_MAGIC: case BUB_AIR:
				return 0;
			default:
				return 0;
		}
	}
	
	BubbleType getShootable(int typeCounters[BUB_TYPE_SENTINEL - 1])
	{
		std::vector<BubbleType> shootables;
		
		if(typeCounters[BUB_RED])
			shootables.push_back(BUB_RED);
		if(typeCounters[BUB_GREEN])
			shootables.push_back(BUB_GREEN);
		if(typeCounters[BUB_BLUE])
			shootables.push_back(BUB_BLUE);
		
		if(shootables.size() > 0)
			return shootables[rand() % shootables.size()];
		else
			return BUB_NONE;
	}
	
	// *******************
	// * Implementations *
	// *******************
	
	void onShot()
	{
		int typeCounters[BUB_TYPE_SENTINEL - 1] = {0};
		int maxJ = getGridStats(typeCounters);
		mCannonY = grid().toWy(maxJ) + MIN_DIST_TO_LOWEST_BUB;
		
		mAmmo[0] = mAmmo[1];
		mAmmo[1] = getShootable(typeCounters);
	}
	
	int getShot(int n)
	{
		return mAmmo[n];
	}
	
	Vec2f cannonPos()
	{		
		return Vec2f(grid().getWidth() / 2, mCannonY);
	}
	
	bool onCollision(GridMatrix<int>& grid, Cell const& src, int shotType)
	{
		if(src.type == BUB_AIR)
		{
			grid.set(src.i, src.j, 0);
			return false;
		}
		else
			return true;
	}
	
	int onPark(GridMatrix<int>& grid, Cell const& src, Cell const& trigger)
	{
		switch(trigger.type)
		{
			case BUB_MOLD:
				grid.set(src.i, src.j, BUB_MOLD);
				return -30;
			
			case BUB_MAGIC:
				grid.set(trigger.i, trigger.j, 0);
				return removeGroup(getGroup(src.type));
				
			case BUB_VENT:
				grid.set(src.i, src.j, 0);
				return -20;
			
			default:
				return -10;
		}
	}
	
	int onPop(GridMatrix<int>& grid, Cell const& src)
	{
		return deathScore(src.type);
	}
	
	int onFall(GridMatrix<int>& grid, Cell const& src)
	{	
		return deathScore(src.type);
	}
	
	int onNeighborPark(GridMatrix<int>& grid, Cell const& src, Cell const& trigger)
	{
		switch(src.type)
		{
			case BUB_SOAP:
				grid.set(src.i, src.j, BUB_FOAM);
				grid.set(trigger.i, trigger.j, BUB_FOAM);
				return -1;
			
			case BUB_RAINBOW:
				grid.set(src.i, src.j, trigger.type);
				return 0;
			
			default:
				if(getGroup(src.type) == getGroup(trigger.type))
					return (src.type == BUB_RED_DUCK || src.type == BUB_GREEN_DUCK || src.type == BUB_BLUE_DUCK) ? 5 : 2;
				else
					return 0;
		}
	}
	
	int getGroup(int type)
	{
		switch(type)
		{
			case BUB_RED: case BUB_RED_DUCK:
				return 1;
			case BUB_GREEN: case BUB_GREEN_DUCK:
				return 2;
			case BUB_BLUE: case BUB_BLUE_DUCK:
				return 3;
			default:
				return 0;
		}
	}
	
	int onNeighborPop(GridMatrix<int>& grid, Cell const& src, int trigger)
	{	
		switch(src.type)
		{
			case BUB_FOAM:
				grid.set(src.i, src.j, trigger);
				return 1;
			default:
				return 0;
		}
	}
};

class Simulator {
	PiggyGame mGame;
	Vec2f mAimDest;
	
public:
	Simulator(PiggyGame game): mGame(game)
	{
		mAimDest = findBestMove(2.5f, 100);
	}
	
	Vec2f findBestMove(float range, int res)
	{
		int bestScore = -1000000;
		Vec2f bestDest;
		
		for(int i = 0; i < res; ++i)
		{
			float ang = -M_PI / 2 - range / 2 + range / (res - 1) * i;
			Vec2f dest = mGame.cannonPos() + Vec2f(cos(ang), sin(ang));
			
			PiggyGame temp = mGame;
			int score = temp.shoot(dest);
			
			if(score > bestScore)
			{
				bestScore = score;
				bestDest = dest;
			}
		}
		
		return bestDest;
	}
	
	void makeBestMove()
	{
		mGame.shoot(mAimDest);
		mAimDest = findBestMove(2.5f, 100);
	}
	
	void draw(BITMAP* buffer, Vec2f const& cam)
	{
		mGame.draw(buffer, cam, PiggyGame::getTypeColor);
		mGame.drawAim(buffer, mAimDest, Vec2f(0, 0), PiggyGame::getTypeColor, [](int i, int j, int t) {
	 	return t != PiggyGame::BUB_AIR;
	 });
	}
};

int main()
{
    bool exit = false;
    int lastMb = 0;
    
    init();
    
    int cols[TYPENUM] = {makecol(255, 0, 0), makecol(0, 255, 0), makecol(0, 0, 255),
                         makecol(255, 255, 0), makecol(0, 255, 255), makecol(255, 0, 255),
                         makecol(255, 255, 255)};
    
    PiggyGame piggyGame(11, 40, CELL_SIZE);
    std::list<PiggyGame> past;
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;

     if(key[KEY_SPACE])
     {
     	piggyGame = PiggyGame(11, 20, CELL_SIZE);
     	past.clear();
        rest(200);
     }
     
     if(mouse_b == 1 && !lastMb)
     {
     	past.push_back(piggyGame);
     	printf("%d\n", piggyGame.shoot(Vec2f(mouse_x, mouse_y)));
     }
     
     if(mouse_b == 2 && !lastMb && !past.empty())
     {
     	piggyGame = past.back();
     	past.pop_back();
     }
     
     lastMb = mouse_b;
     
     clear_to_color(buffer, makecol(64, 64, 64));

	 piggyGame.draw(buffer, Vec2f(0, 0), PiggyGame::getTypeColor);
	 piggyGame.drawAim(buffer, Vec2f(mouse_x, mouse_y), Vec2f(0, 0), PiggyGame::getTypeColor, [](int i, int j, int t) {
	 	return t != PiggyGame::BUB_AIR;
	 });
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

    deinit();
    return 0;
}END_OF_MAIN()
