#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>

#include "Buffer.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

#define BOARD_W 12
#define BOARD_H 16
#define BUBBLE_TYPES 4

#define CELL_SIZE 32

#define FPS 60

int bubbleCols[BUBBLE_TYPES];

double runtime = 0.0;

void tick()
{
   runtime += 0.001;
}

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     install_int(tick, 1);
     
     //srand(time(NULL));
     //srand(100);
     
     bubbleCols[0] = makecol(255, 0, 0),
     bubbleCols[1] = makecol(0, 255, 0),
     bubbleCols[2] = makecol(0, 0, 255),
     bubbleCols[3] = makecol(255, 255, 0),
     bubbleCols[4] = makecol(230, 230, 230),
     bubbleCols[5] = makecol(100, 100, 100),
     bubbleCols[6] = makecol(255, 140, 0),
     bubbleCols[7] = makecol(200, 70, 255);
}

void deinit()
{
     destroy_bitmap(buffer);
}
    
class Board {
      public:
      int mat[BOARD_W][BOARD_H];
      
      int xoffs(int j)
      {
          return CELL_SIZE / 2 * (j % 2);
      }

      int toBy(float sy)
      {
          return int(sy / CELL_SIZE);
      }

      int toBx(float sx, float sy)
      {
          int j = toBy(sy);
          return int((sx - xoffs(j)) / CELL_SIZE);
      }

      float toWy(int j)
      {
            return j * CELL_SIZE + CELL_SIZE / 2;
      }

      float toWx(int i, int j)
      {
            return i * CELL_SIZE + CELL_SIZE / 2 + xoffs(j);
      }
      
      int at(int x, int y) const
      {
          if(x < 0 || y < 0 || x >= BOARD_W || y >= BOARD_H)
               return 0;
          else
              return mat[x][y];
      }

      
      void clear()
      {
          for(int j = 0; j < BOARD_H; j++)
            for(int i = 0; i < BOARD_W; i++)
              mat[i][j] = 0;
      }
      
      
      
      static int randomType()
      {
          return rand() % BUBBLE_TYPES + 1;
      }
      
      void randomFill()
      {
          clear();
           
           for(int j = 0; j < BOARD_H / 2; j++)
            for(int i = 0; i < BOARD_W; i++)
                mat[i][j] = randomType();
      }
      
      Board()
      {
          randomFill();
      }
      
      bool isValidPos(int i, int j)
      {
           return i >= 0 && j >= 0 && i < BOARD_W && j < BOARD_H;
      }
      
      bool isOccupiedPos(int i, int j)
      {
           return isValidPos(i, j) && mat[i][j];
      }
      
      void putBubble(int i, int j, int type)
      {
           if(isValidPos(i, j))
              mat[i][j] = type;
      }
      
      static void drawBubble(BITMAP* buffer, int x, int y, int rad, int col)
      {
          circlefill(buffer, x, y, rad, col);
      }
      
      void render(BITMAP* buffer, int x, int y, bool hex)
      {
         int rad = CELL_SIZE / 2;
       
         for(int j = 0; j < BOARD_H; j++)
            for(int i = 0; i < BOARD_W; i++)
               if(mat[i][j])
                  drawBubble(buffer, x + rad + i * CELL_SIZE + hex * rad * (j % 2),
                                     y + rad + j * CELL_SIZE,
                                     rad, bubbleCols[mat[i][j] - 1]);
         
         rect(buffer, x, y, x + BOARD_W * CELL_SIZE + rad, y + BOARD_H * CELL_SIZE + rad, 0);
      }
};

struct Point {
   int x, y;
   
   Point(int px, int py): x(px), y(py)
   {
   }
};


struct FloodFiller {
   struct Visitor {
      virtual bool visit(int c) = 0;
   };
       
   Buffer<bool> mDirty;
   Board& mBoard;
   Visitor* mVisitor;
   
   FloodFiller(Board board): mBoard(board), mDirty(BOARD_W, BOARD_H)
   {
      mDirty.fill(false);
   }

   bool needsMarking(int i, int j)
   {
       return !mDirty.at(i, j) && mVisitor->visit(mBoard.mat[i][j]);
   }

void fillRow(std::list<Point>& queue, int mat[BOARD_W][BOARD_H], int x0, int y0)
{
   int x, firstX, lastX;
   bool lastTopFillable = false, lastBottomFillable = false;

   for(x = x0 - 1; x >= 0 && needsMarking(x, y0); --x)
      mDirty.at(x, y0) = true;
   
   firstX = x + 1;
   
   for(x = x0; x < BOARD_W && needsMarking(x, y0); ++x)
      mDirty.at(x, y0) = true;
   
   lastX = x - 1;
   
   if(y0 % 2)
       ++lastX;
   else
      --firstX;
   
   if(firstX < 0)
     firstX = 0;
   
   if(lastX > BOARD_W - 1)
     lastX = BOARD_W - 1;
   
   for(int x = firstX; x <= lastX; ++x)
   {
        if(y0 > 0)
        {
           if(mat[x][y0 - 1] == src)
           {
               if(!lastTopFillable)
                  queue.push_back(Point(x, y0 - 1));
               lastTopFillable = true;
           }
           
           else
               lastTopFillable = false;
        }
        
        
        if(y0 < BOARD_H - 1)
        {
           if(mat[x][y0 + 1] == src)
           {
               if(!lastBottomFillable)
                  queue.push_back(Point(x, y0 + 1));
               lastBottomFillable = true;
           }
           
           else
               lastBottomFillable = false;
        }
   }
}

void floodFill(int mat[BOARD_W][BOARD_H], int x0, int y0, int dest)
{
   std::list<Point> queue;
   int src = mat[x0][y0];
   
   if(src == dest)
     return;
   
   queue.push_back(Point(x0, y0));
   
   int rowCounter = 0;
   
   while(!queue.empty())
   {
      ++rowCounter;
      Point p = queue.front();
      queue.pop_front();
      
      fillRow(queue, mat, p.x, p.y, src, dest);
   }
   
   printf("%d\n", rowCounter);
}   

};

int main()
{
    bool exit = false;
    
    init();
    
    Board board;
    int lastMb = 0;
    
    while(!exit)
    { 
      
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b == 1)
     {
         int i = board.toBx(mouse_x, mouse_y);
         int j = board.toBy(mouse_y);
         
         printf("%d %d\n", i, j);
         
         //floodFill(board.mat, i, j, 5);
     }
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     board.render(buffer, 0, 0, true);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
