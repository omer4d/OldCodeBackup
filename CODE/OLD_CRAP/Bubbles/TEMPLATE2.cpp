#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <tuple>
#include <vector>

#include "DrawVec2f.hpp"

#include "Vec2f.hpp"
#include "Util.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

#define BOARD_W 12
#define BOARD_H 16
#define BUBBLE_TYPES 8

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
     
     srand(time(NULL));
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

int sign(float f)
{
    return f < 0 ? -1 : 1;
}

int trunc(float f)
{
    return f >= 0 ? int(f) : int(f - 1);
}

int mod(int n, int max)
{
    int m = n % max;
    return m >= 0 ? m : max + m;
}
    
class Board {
      public:
      int mat[BOARD_W][BOARD_H];
      bool dirty[BOARD_W][BOARD_H];
      
      int xoffs(int j)
      {
          return CELL_SIZE / 2 * mod(j, 2);
      }

      int toBy(float sy)
      {
          return trunc(sy / CELL_SIZE);
      }

      int toBx(float sx, float sy)
      {
          int j = toBy(sy);
          return trunc((sx - xoffs(j)) / CELL_SIZE);
      }

      float toWy(int j)
      {
            return j * CELL_SIZE + CELL_SIZE / 2;
      }

      float toWx(int i, int j)
      {
            return i * CELL_SIZE + CELL_SIZE / 2 + xoffs(j);
      }

      Vec2f toWorld(int i, int j)
      {
            Vec2f w;
            
            w.x = toWx(i, j);
            w.y = toWy(j);

            return w;
      }
      
      std::tuple<int, int> toBoard(Vec2f const& w)
      {
          return std::tuple<int, int>(toBx(w.x, w.y), toBy(w.y));
      }
      
      Vec2f nearest(Vec2f p)
      {
            return toWorld(toBx(p.x, p.y), toBy(p.y));
      }
      
      /*
      std::tuple<int, int> next(Vec2f const& pos, Vec2f const& vel)
      {
            float dx = xoffs(toBy(pos.y));
            Vec2f orig((pos.x - dx) / CELL_SIZE, pos.y / CELL_SIZE);
            Vec2f dir = vel.unit();
     
            int x = trunc(orig.x), y = trunc(orig.y);
            int stepX = sign(dir.x), stepY = sign(dir.y);    
     
            float tMaxX = (trunc(orig.x + (stepX > 0)) - orig.x) / dir.x;
            float tMaxY = (trunc(orig.y + (stepY > 0)) - orig.y) / dir.y;
            float t = (tMaxX < tMaxY ? tMaxX : tMaxY);
     
            Vec2f dest = (orig + dir * (t + 0.01)); 
     
            dest *= CELL_SIZE;
            dest.x += dx;
     
            return std::tuple<int, int>(toBx(dest.x, dest.y), toBy(dest.y));
     }*/
     
     std::tuple<int, int> next(Vec2f const& pos, Vec2f const& vel)
     {
           float dx = xoffs(toBy(pos.y));
           Vec2f orig((pos.x - dx) / CELL_SIZE, pos.y / CELL_SIZE);
           Vec2f dir = vel.unit();
     
           int x = trunc(orig.x), y = trunc(orig.y);
           int stepX = sign(dir.x), stepY = sign(dir.y);
     
           float tMaxX = (trunc(orig.x + (stepX > 0)) - orig.x) / dir.x;
           float tMaxY = (trunc(orig.y + (stepY > 0)) - orig.y) / dir.y;
           float t = (tMaxX < tMaxY ? tMaxX : tMaxY);
     
           if(tMaxX < tMaxY)
           {
               return std::tuple<int, int>(x + stepX, y);
           }
     
           else
           {
               Vec2f dest = orig + dir * t - Vec2f(x, y);
               bool isOdd = mod(y, 2);
               return dest.x < 0.5 ? std::tuple<int, int>(x - 1 + isOdd, y + stepY) : std::tuple<int, int>(x + isOdd, y + stepY);
           }
     }
     
      
      
      int at(int x, int y) const
      {
          if(x < 0 || y < 0 || x >= BOARD_W || y >= BOARD_H)
               return 0;
          else
              return mat[x][y];
      }
      
      void clearDirty()
      {
         for(int j = 0; j < BOARD_H; j++)
            for(int i = 0; i < BOARD_W; i++)
              dirty[i][j] = false;
      }
      
      void clear()
      {
          for(int j = 0; j < BOARD_H; j++)
            for(int i = 0; i < BOARD_W; i++)
              mat[i][j] = 0;
      }
      
      float dist(Vec2f p1, int i, int j)
      {
           if(!isValidPos(i, j) || mat[i][j])
               return 100000000.0;
           else
               return Vec2f::squaredDist(p1, toWorld(i, j));
      }
      
      void fillNearestNeighbor(Vec2f pos, int type)
      {
           int i = toBx(pos.x, pos.y);
           int j = toBy(pos.y);
           int k = j % 2 ? 1 : - 1;
           
           int ni[6] = {i + 1, i - 1, i, i, i + k, i + k},
               nj[6] = {j, j, j - 1, j + 1, j - 1, j + 1};
           
           int minIndex = -1;
           float minDist = 100000000.0;
           
           for(int c = 0; c < 6; c++)
              if(dist(pos, ni[c], nj[c]) < minDist)
              {
                 minDist = dist(pos, ni[c], nj[c]);
                 minIndex = c;
              }
              
           if(minIndex > -1)
           {
              mat[ni[minIndex]][nj[minIndex]] = type;
              removeGroup(ni[minIndex], nj[minIndex]);
              removeFloaters();
           }
      }
      
      int floaterFloodFill(int i, int j)
      {
          int count = 0;
          
          if(i >= 0 && j >= 0 && i < BOARD_W && j < BOARD_H && mat[i][j] && !dirty[i][j])
          {
            int k = j % 2 ? 1 : - 1;
           
            dirty[i][j] = true;
            count++;
              
            count += floaterFloodFill(i + 1, j);
            count += floaterFloodFill(i - 1, j);
              
            count += floaterFloodFill(i, j - 1);
            count += floaterFloodFill(i, j + 1);
           
            count += floaterFloodFill(i + k, j - 1);
            count += floaterFloodFill(i + k, j + 1);
          }
          
          return count;
      }
      
      int removeFloaters()
      {
           int count = 0;
           
           clearDirty();
           
           for(int i = 0; i < BOARD_W; i++)
              count += floaterFloodFill(i, 0);
           
           for(int j = 0; j < BOARD_H; j++)
              for(int i = 0; i < BOARD_W; i++)
                 if(!dirty[i][j])
                     mat[i][j] = 0;
                     
           return count;
      }
      
      int groupFloodFill(int i, int j, int c)
      {
          int count = 0;
          
          if(i >= 0 && j >= 0 && i < BOARD_W && j < BOARD_H && mat[i][j] != 0 && mat[i][j] == c && !dirty[i][j])
          {
            int k = j % 2 ? 1 : - 1;
           
            dirty[i][j] = true;
            count++;
              
            count += groupFloodFill(i + 1, j, c);
            count += groupFloodFill(i - 1, j, c);
              
            count += groupFloodFill(i, j - 1, c);
            count += groupFloodFill(i, j + 1, c);
           
            count += groupFloodFill(i + k, j - 1, c);
            count += groupFloodFill(i + k, j + 1, c);
          }
          
          return count;
      }
      
      void removeGroup(int i, int j)
      {
           int count = 0;
           clearDirty();
           
           count += groupFloodFill(i, j, mat[i][j]);
           //printf("%d\n", count);
           
           if(count > 2)
           for(int j = 0; j < BOARD_H; j++)
              for(int i = 0; i < BOARD_W; i++)
                 if(dirty[i][j])
                     mat[i][j] = 0;
      }
      
      int countBubbles()
      {
         int c = 0;
         
         for(int j = 0; j < BOARD_H; j++)
            for(int i = 0; i < BOARD_W; i++)
               if(mat[i][j])
               c++;
         
         return c;
      }
      
      static int randomType()
      {
          return rand() % BUBBLE_TYPES + 1;
      }
      
      void randomFill()
      {
          clear();
          
          while(countBubbles() < 60)
          {
           clear();
           
           for(int j = 0; j < BOARD_H / 2; j++)
            for(int i = 0; i < BOARD_W; i++)
             if(rand() % 2)
                mat[i][j] = randomType();
             
            removeFloaters();
          }
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
           {
              mat[i][j] = type;
              removeGroup(i, j);
              removeFloaters();
              
           }
      }
      
      static void drawBubble(BITMAP* buffer, int x, int y, int rad, int col)
      {
          circlefill(buffer, x, y, rad, col);
          //rectfill(buffer, x - rad, y - rad, x + rad, y + rad, col);
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

class Bubble {
public:
      
      Vec2f pos, vel;
      int type;
      
      Bubble()
      {
         type = Board::randomType();
      }
      
      void logic(float dt)
      {
         pos += vel * dt;
      }
      
      void render(BITMAP* buffer)
      {
           circlefill(buffer, int(pos.x), int(pos.y), CELL_SIZE / 2, bubbleCols[type - 1]);
      }
};

class Game {
      Board board;
      Bubble bubble;
      int lastI, lastJ;
      
      Vec2f borderMin, borderMax;
      float borderW, borderH;
      
      Vec2f bubbleOrigin;
      
      float rotSpeed, shotSpeed;
      float angle;
      
public:
      Game(float x, float y)
      {
           int rad = CELL_SIZE / 2;
           
           borderMin.init(x, y);
           borderMax.init(x + BOARD_W * CELL_SIZE + rad, y + BOARD_H * CELL_SIZE + rad);
           borderW = borderMax.x - borderMin.x;
           borderH = borderMax.y - borderMin.y;
           bubbleOrigin.init(borderMin.x + borderW * 0.5f, borderMax.y - rad - 20);
           
           board.randomFill();
           
           bubble.pos = bubbleOrigin;
           bubble.vel.x = 0;
           bubble.vel.y = 0;
           
           angle = -M_PI * 0.5;
           rotSpeed = 2.5;
           shotSpeed = 500.0;
      }
      
      void regenBubble()
      {
          bubble.pos = bubbleOrigin;
          bubble.vel.init(0, 0);
          bubble.type = Board::randomType();
          lastI = board.toBx(bubble.pos.x, bubble.pos.y);
          lastJ = board.toBy(bubble.pos.y);
      }
      
      void bubbleVsBorder()
      {
           float rad = CELL_SIZE * 0.5f;
           
           if(bubble.pos.x - rad < borderMin.x)
           {
               bubble.pos.x = borderMin.x + rad;
               bubble.vel.x *= -1;
           }
           
           if(bubble.pos.x + rad > borderMax.x)
           {
               bubble.pos.x = borderMax.x - rad;
               bubble.vel.x *= -1;
           }
           
           if(bubble.pos.y - rad < borderMin.y)
           {
               regenBubble();
           }
      }
      
      void bubbleVsBoard()
      {
           Vec2f pos = bubble.pos - borderMin;
           int i = board.toBx(pos.x, pos.y);
           int j = board.toBy(pos.y);
           int ni, nj;
           
           std::tie<int, int>(ni, nj) = board.next(pos, bubble.vel);
           
           if(board.isOccupiedPos(ni, nj))
           {
              board.putBubble(i, j, bubble.type);
              //board.fillNearestNeighbor(bubble.pos, bubble.type);
              
              
              regenBubble();
           }
           
           //board.removeGroup();
      }
      
      void logic(float dt)
      {
           float rs = key[KEY_LSHIFT] ? rotSpeed * 0.3 : rotSpeed;
       
           if(key[KEY_LEFT]) angle -= rs * dt;
           if(key[KEY_RIGHT]) angle += rs * dt;
           
           if(angle > -0.2) angle = -0.2;
           if(angle < -M_PI + 0.2) angle = -M_PI + 0.2;
           
           if(key[KEY_SPACE] && bubble.vel.squaredLength() < 0.1)
           {
              bubble.vel.x = shotSpeed * cos(angle);
              bubble.vel.y = shotSpeed * sin(angle);
           }
           
           lastI = board.toBx(bubble.pos.x, bubble.pos.y);
           lastJ = board.toBy(bubble.pos.y);
           bubbleVsBorder();
           bubbleVsBoard();
           
           bubble.logic(dt);
      }
      
      void render(BITMAP* buffer)
      {
           Vec2f dest = bubbleOrigin;
           dest.x += 100 * cos(angle);
           dest.y += 100 * sin(angle);
           
           board.render(buffer, int(borderMin.x), int(borderMin.y), 1);
           bubble.render(buffer);
           
           
           Vec2f pos = bubble.pos - borderMin;
           int ni, nj;
           std::tie<int, int>(ni, nj) = board.next(pos, bubble.vel);
           Vec2f test = borderMin + board.toWorld(ni, nj);
           //DrawVec2f::circlefill(buffer, test, CELL_SIZE / 5, 0);
           
           //int i = board.toBx(mouse_x, mouse_y);
           //int j = board.toBy(mouse_y);
           //Vec2f test = borderMin + board.toWorld(i, j);
           
           //DrawVec2f::circlefill(buffer, test, CELL_SIZE / 2, 0);
           
           DrawVec2f::line(buffer, bubbleOrigin, dest, 0);
      }
};

int main()
{
    bool exit = false;
    
    init();
    
    Board board;
    int lastMb = 0;
    
    Game game(0, 0);
    double lastLogicTime = -1000;
    double lastFpsTime = -1000;
    
    int frames = 0;
    
    while(!exit)
    { 
      
     if(key[KEY_ESC]) exit = true;
     
     if(runtime - lastLogicTime > 1.0 / float(FPS))
     {
        game.logic(runtime - lastLogicTime);
        lastLogicTime = runtime;
        frames++;
     }
     
     if(runtime - lastFpsTime > 1.0)
     {
        printf("%d\n", frames);
        frames = 0;
        lastFpsTime = runtime;
     }
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     
     game.render(buffer);
     
     //if(mouse_b == 1 && !lastMb)
        //board.removeGroup(board.toBx(mouse_x, mouse_y), board.toBy(mouse_y));
     
     lastMb = mouse_b;
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
