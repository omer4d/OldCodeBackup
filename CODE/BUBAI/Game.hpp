#ifndef _GAME_HPP_
#define _GAME_HPP_

#include "GridMatrix.hpp"
#include "RayTracer.hpp"

struct Cell {
	int i, j, type;
	
	Cell(int pi, int pj, int ptype):
		i(pi), j(pj), type(ptype)
	{
	}
};

class Game {
public:
	enum State {
		STATE_WON, STATE_LOST, STATE_ONGOING
	};
	
	void draw(BITMAP* buffer, Vec2f const& cam, GetTypeColor getTypeColor)
	{
		GridMatrix<int> const& grid = getGrid();
		
		for(int j = 0; j < grid.rowNum; ++j)
    		for(int i = 0; i < grid.getLogicColNum(j); ++i)
       		{
          		int type = grid.at(i, j);
          
          		if(type > 0)
             		circlefill(buffer, int(grid.toWx(i, j) - cam.x), int(grid.toWy(j) - cam.y), grid.cellSize / 2 - 1, getTypeColor(type));
          		else
             		circle(buffer, int(grid.toWx(i, j) - cam.x), int(grid.toWy(j) - cam.y), grid.cellSize / 2, makecol(50, 50, 50));
       		}
       		
       	Vec2f cp = cannonPos();
       	
       	circlefill(buffer, int(cp.x - cam.x), int(cp.y - cam.y), grid.cellSize / 2 - 1, getTypeColor(getShot(0)));
       	circlefill(buffer, int(cp.x - cam.x), int(cp.y + grid.cellSize - cam.y), grid.cellSize / 2 - 1, getTypeColor(getShot(1)));
	}
	
	void drawAim(BITMAP* buffer, Vec2f const& dest, Vec2f const& cam, GetTypeColor getTypeColor, RayTracerCallback const& callback=nullptr)
	{
		GridMatrix<int>& grid = getGrid();
		
		Vec2f dir = (dest - cannonPos()).unit();
   		HitInfo info = raytrace(grid, cannonPos(), dir, 1, callback, true, getTypeColor);
        
   		if(info.hit)
   		{
       		circlefill(buffer, int(grid.toWx(info.parkI, info.parkJ)),
                          		int(grid.toWy(info.parkJ)), 5, makecol(255, 255, 255));
        
       		circlefill(buffer, int(grid.toWx(info.hitI, info.hitJ)),
                          		int(grid.toWy(info.hitJ)), 5, 0);
   		}
	}
	
	int getGridStats(int typeCounter[])
	{
		GridMatrix<int> const& grid = getGrid();
		
		int maxJ = 0;
		
		for(int j = 0; j < grid.rowNum; ++j)
		{
			bool found = false;
			
			for(int i = 0; i < grid.getLogicColNum(j); ++i)
			{
				int type = grid.at(i, j);
				
				if(type)
				{
					maxJ = j;
					++typeCounter[type];
					found = true;
				}
			}
			
			if(!found)
				break;
		}
		
		return maxJ;
	}
	
	bool hasMarkedNeighbors(GridMatrix<int> const& grid, Flood<int>& flood, int i, int j)
	{
		for(int k = 0; k < 6; ++k)
			if(flood.getMarkerAt(grid.getNeighborI(i, j, k), grid.getNeighborJ(i, j, k)))
				return true;
		
		return false;
	}
	
	int pop(GridMatrix<int>& grid, Flood<int>& flood, int i, int j, int shot)
	{
		int shotGroup = getGroup(shot);
		int score = 0;
		
		int lumpSize = flood.run(i, j, false, [=](int t, int i, int j) {
			return getGroup(t) == shotGroup; 
		});
		
		if(lumpSize >= 3)
			for(int j = 0; j < grid.rowNum; ++j)
				for(int i = 0; i < grid.getLogicColNum(j); ++i)
				{
					int type = grid.at(i, j);
					
					if(flood.getMarkerAt(i, j))
					{
						score += onPop(grid, Cell(i, j, grid.at(i, j)));
						grid.set(i, j, 0);
					}
					
					else if(type && hasMarkedNeighbors(grid, flood, i, j))
						score += onNeighborPop(grid, Cell(i, j, type), shot);
				}
					
		return score;
	}
	
	int removeFloaters(GridMatrix<int>& grid, Flood<int>& flood)
	{
		int score = 0;
		
		flood.reset();
		
		for(int i = 0; i < grid.colNum; ++i)
			flood.run(i, 0, false, [](int t, int i, int j) { return t != 0; });
		
		for(int j = 0; j < grid.rowNum; ++j)
			for(int i = 0; i < grid.getLogicColNum(j); ++i)
			{
				int t = grid.at(i, j);
				if(!flood.getMarkMat()[i][j] && t)
				{
					score += onFall(grid, Cell(i, j, t));
					grid.set(i, j, 0);
				}
			}
			
		return score;
	}
	
	int park(GridMatrix<int>& grid, HitInfo const& hitInfo, int shot)
	{
		Flood<int> flood(&grid);
		int score = 0;
		
		grid.set(hitInfo.parkI, hitInfo.parkJ, shot);
   		score += onPark(grid, Cell(hitInfo.parkI, hitInfo.parkJ, shot), Cell(hitInfo.hitI, hitInfo.hitJ, hitInfo.hitType));
   		score += pop(grid, flood, hitInfo.parkI, hitInfo.parkJ, shot) + removeFloaters(grid, flood);
   		
   		for(int k = 0; k < 6; ++k)
   		{
   			int ni = grid.getNeighborI(hitInfo.parkI, hitInfo.parkJ, k);
			int nj = grid.getNeighborJ(hitInfo.parkI, hitInfo.parkJ, k);
			int nt = grid.at(ni, nj);
			
			if(nt != 0)
				score += onNeighborPark(grid, Cell(ni, nj, nt), Cell(hitInfo.parkI, hitInfo.parkJ, shot));
   		}
   		
   		return score;
	}
	
	int shoot(Vec2f const& dest)
	{
		GridMatrix<int>& grid = getGrid();
		
		int shot = getShot(0);
		Vec2f dir = (dest - cannonPos()).unit();
   		HitInfo info = raytrace(grid, cannonPos(), dir, 1, [&](int i, int j, int content) {
   			return onCollision(grid, Cell(i, j, content), shot);
   		}, false);
   		
   		int score = -1000;
   		
   		if(info.hit)
   		   score = park(grid, info, shot);
   		   
   		afterShot();
   		
   		return score;
	}
	
	virtual GridMatrix<int> const& getGrid() const;
	virtual GridMatrix<int>& getGrid();
	
	virtual int getGroup(int type) = 0;
	virtual int getShot(int n) = 0;
	virtual Vec2f cannonPos() = 0;
	virtual bool onCollision(GridMatrix<int>& grid, Cell const& src, int shotType) = 0;					// Called before park.
	virtual int onPark(GridMatrix<int>& grid, Cell const& src, Cell const& trigger) = 0;				// Called after set, before pop.
	virtual int onPop(GridMatrix<int>& grid, Cell const& src) = 0;										
	virtual int onFall(GridMatrix<int>& grid, Cell const& src) = 0;
	virtual int onNeighborPark(GridMatrix<int>& grid, Cell const& src, Cell const& trigger) = 0;		// Called after pop.
	virtual int onNeighborPop(GridMatrix<int>& grid, Cell const& src, int trigger) = 0;
	virtual void afterShot() = 0;																		// After everything.
	
	virtual State getState() = 0;
};

#endif
