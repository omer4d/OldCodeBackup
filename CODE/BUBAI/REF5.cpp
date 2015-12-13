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
#include "Game.hpp"
#include "RayTracer.hpp"

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
     
     G_RAYTRACER_BUFFER = buffer;
}

void deinit()
{
     destroy_bitmap(buffer);
}

#define CELL_SIZE 30
#define RES 100
#define RANGE 2.5f

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
	static constexpr int MAX_SHOTS = 100;
	
	float mCannonY;
	std::vector<BubbleType> mAmmo;
	Game::State mState;
	int mShotCounter;
	
public:
	PiggyGame(int colNum, int rowNum, float cellSize): Game(colNum, rowNum, cellSize, true), mState(Game::STATE_ONGOING), mShotCounter(0)
	{
		GridMatrix<int> skel(colNum, rowNum, cellSize, true, 0);    
		std::vector<float> chances(BUB_TYPE_SENTINEL - 1, 0.f);
		
		chances[BUB_RED - 1] = 1.f;
		chances[BUB_GREEN - 1] = 1.f;
		chances[BUB_BLUE - 1] = 1.f;
		
		
		chances[BUB_RAINBOW - 1] = 1.f;
		chances[BUB_RED_DUCK - 1] = 1.f;
		chances[BUB_GREEN_DUCK - 1] = 1.f;
		chances[BUB_BLUE_DUCK - 1] = 1.f;
//		
		chances[BUB_MOLD - 1] = 1.f;
		chances[BUB_MAGIC - 1] = 1.f;
		chances[BUB_SPONGE - 1] = 1.f;
		chances[BUB_AIR - 1] = 1.f;
		chances[BUB_VENT - 1] = 1.f;
//		
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
		   {
		   		int type = gridMat.at(i, j);
		   	
		   		if(getGroup(type) == group)
		   		{
		   			gridMat.set(i, j, 0);
		   			counter += deathScore(type);
		   		}
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
		
		if(typeCounters[BUB_RED] || typeCounters[BUB_RED_DUCK])
			shootables.push_back(BUB_RED);
		if(typeCounters[BUB_GREEN] || typeCounters[BUB_GREEN_DUCK])
			shootables.push_back(BUB_GREEN);
		if(typeCounters[BUB_BLUE] || typeCounters[BUB_BLUE_DUCK])
			shootables.push_back(BUB_BLUE);
		
		if(shootables.size() > 0)
			return shootables[rand() % shootables.size()];
		else
			return BUB_NONE;
	}
	
	// *******************
	// * Implementations *
	// *******************
	
	Game::State getState()
	{
		return mState;
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
	
	int getShot(int n)
	{
		return mAmmo[n];
	}
	
	Vec2f cannonPos()
	{		
		return Vec2f(grid().getWidth() / 2, mCannonY);
	}
	
	void afterShot()
	{
		int typeCounters[BUB_TYPE_SENTINEL - 1] = {0};
		int maxJ = getGridStats(typeCounters);
		mCannonY = grid().toWy(maxJ) + MIN_DIST_TO_LOWEST_BUB;
		
		if(typeCounters[mAmmo[1]])
			mAmmo[0] = mAmmo[1];
		else
			mAmmo[0] = getShootable(typeCounters);
			
		mAmmo[1] = getShootable(typeCounters);
		
		++mShotCounter;
		
		if(!typeCounters[BUB_RED_DUCK] && !typeCounters[BUB_GREEN_DUCK] && !typeCounters[BUB_BLUE_DUCK])
			mState = Game::STATE_WON;
		else if(mShotCounter > MAX_SHOTS)
			mState = Game::STATE_LOST;
		else
			mState = Game::STATE_ONGOING;		
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
			
			case BUB_FOAM:
				return -1;
			
			case BUB_RAINBOW:
				grid.set(src.i, src.j, trigger.type);
				return 0;
			
			default:
				if(getGroup(src.type) == getGroup(trigger.type))
					return (src.type == BUB_RED_DUCK || src.type == BUB_GREEN_DUCK || src.type == BUB_BLUE_DUCK) ? 5 : 2;
				else
					return -2;
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

struct PlayStats {
	Game::State state;
	int shotCount;
	
	PlayStats(Game::State pState, int pShotCount):
		state(pState), shotCount(pShotCount)
	{
	}
};

class Simulator {
	PiggyGame mGame;
	Vec2f mAimDest;
	
public:
	Simulator(PiggyGame game): mGame(game)
	{
		//mAimDest = findBestMove(2.5f, 100);
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
		
		if(mGame.getState() == Game::STATE_LOST)
			printf("Game lost...\n");
		else if(mGame.getState() == Game::STATE_WON)
			printf("Game won...\n");
		
		mAimDest = findBestMove(2.5f, 100);
	}
	
	PlayStats play()
	{
		int shotCounter = 0;
		
		while(mGame.getState() != Game::STATE_WON && mGame.getState() != Game::STATE_LOST)
		{
			Vec2f dest = findBestMove(2.f, 100);
			mGame.shoot(dest);
			++shotCounter;
		}
		
		return PlayStats(mGame.getState(), shotCounter);
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
    
    
    //PiggyGame piggyGame(11, 40, CELL_SIZE);
    //std::list<PiggyGame> past;
    
    Simulator sim(PiggyGame(11, 40, CELL_SIZE));
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;

     if(key[KEY_SPACE])
     {
     	sim = Simulator(PiggyGame(11, 40, CELL_SIZE));
		//past.clear();
        rest(200);
     }
     
//     if(mouse_b == 1 && !lastMb)
//     {
//     	sim.makeBestMove();
//     	//past.push_back(piggyGame);
//     	//printf("%d\n", piggyGame.shoot(Vec2f(mouse_x, mouse_y)));
//     }
//     
//     if(mouse_b == 2 && !lastMb)
//     {
//     	PlayStats stats = sim.play();
//     	printf("%s (%d shots)\n", stats.state == Game::STATE_LOST ? "Game lost" : "Game won", stats.shotCount);
//     }
     
     
//     if(mouse_b == 2 && !lastMb && !past.empty())
//     {
//     	piggyGame = past.back();
//     	past.pop_back();
//     }
     
     lastMb = mouse_b;
     
     clear_to_color(buffer, makecol(64, 64, 64));

	//sim.draw(buffer, Vec2f(0.f, 0.f));
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }

    deinit();
    return 0;
}END_OF_MAIN()
