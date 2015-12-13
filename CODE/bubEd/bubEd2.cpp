#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <iostream>
#include <fstream>

#include <allegro.h>
#include <alpng/alpng.h>
#include <list>
#include <tuple>
#include <vector>
#include <json/json.h>

#include "Buffer.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

#define BOARD_W 12
#define BOARD_H 40
#define BUBBLE_TYPES 17

#define FPS 60

int bubbleCols[BUBBLE_TYPES];
BITMAP* bubBmps[BUBBLE_TYPES];

double runtime = 0.0;

void tick()
{
   runtime += 0.001;
}

BITMAP* myLoadBmp(char const* path)
{
   BITMAP* bmp = load_bitmap(path, NULL);
   
   for(int j = 0; j < bmp->h; ++j)
      for(int i = 0; i < bmp->w; ++i)
      {
         if(geta(PIXEL(bmp, i, j)) < 100)
           PIXEL(bmp, i, j) = makecol(255, 0, 255);
      }
      
    return bmp;
}

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     alpng_init();
     
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
     
     bubBmps[0] = myLoadBmp("data/red_bub.idle.png");
     bubBmps[1] = myLoadBmp("data/blue_bub.idle.png");
     bubBmps[2] = myLoadBmp("data/green_bub.idle.png");
     bubBmps[3] = myLoadBmp("data/cyan_bub.idle.png");
     bubBmps[4] = myLoadBmp("data/yellow_bub.idle.png");
     bubBmps[5] = myLoadBmp("data/magenta_bub.idle.png");
     bubBmps[6] = myLoadBmp("data/grey_bub.idle.png");
     
     bubBmps[7] = myLoadBmp("data/foam_bub.idle.png");
     bubBmps[8] = myLoadBmp("data/duck_bub.idle.png");
     bubBmps[9] = myLoadBmp("data/soap_bub.idle.png");
     bubBmps[10] = myLoadBmp("data/magic_bub.idle.png");
     bubBmps[11] = myLoadBmp("data/sponge_bub.idle.png");
     bubBmps[12] = myLoadBmp("data/mold_bub.idle.png");
     bubBmps[13] = myLoadBmp("data/vent_bub.idle.png");
     bubBmps[14] = myLoadBmp("data/air_bub.idle.png");
     bubBmps[15] = myLoadBmp("data/rainbow_bub.idle.png");
     bubBmps[16] = myLoadBmp("data/blinking_bub.idle.png");
}

void deinit()
{
     destroy_bitmap(buffer);
}
    
class Board {
      public:
      int mat[BOARD_W][BOARD_H];
      float JFACTOR = sqrt(3.f) * 0.5f;
      float CELL_SIZE = 32.f;
      float zoom = 1.f;
      
      int xoffs(int j)
      {
          return CELL_SIZE / 2 * (j % 2);
      }

      int toBy(float sy)
      {
          return int(sy / (CELL_SIZE * JFACTOR));
      }

      int toBx(float sx, float sy)
      {
          int j = toBy(sy);
          return int((sx - xoffs(j)) / CELL_SIZE);
      }

      float toWy(int j)
      {
            return j * CELL_SIZE * JFACTOR + CELL_SIZE / 2;
      }

      float toWx(int i, int j)
      {
            return i * CELL_SIZE + CELL_SIZE / 2 + xoffs(j);
      }
      
      float toSx(int i, int j, float x)
      {
          return (toWx(i, j) - x) * zoom + SCREEN_W / 2;
      }
      
      float toSy(int j, int y)
      {
          return (toWy(j) - y) * zoom + SCREEN_H / 2;
      }
      
      int sxToI(float sx, float sy, float camX, float camY)
      {
          float wx = (sx - SCREEN_W / 2) / zoom + camX;
          float wy = (sy - SCREEN_H / 2) / zoom + camY;
          
          return toBx(wx - CELL_SIZE * 0.5, wy - CELL_SIZE * 0.5);
      }
      
      int syToJ(float sy, float camY)
      {
         float wy = (sy - SCREEN_H / 2) / zoom + camY;
         return toBy(wy - CELL_SIZE * 0.5);
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
           
           /*for(int j = 0; j < BOARD_H / 2; j++)
            for(int i = 0; i < BOARD_W; i++)
                mat[i][j] = randomType();*/
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
      
      static void drawBubble(BITMAP* buffer, int x, int y, int rad, int idx)
      {
          //circlefill(buffer, x, y, rad, col);
          stretch_sprite(buffer, bubBmps[idx], x - rad, y - rad, rad * 2, rad*2);
      }
      
      void render(BITMAP* buffer, int x, int y)
      {
         int rad = CELL_SIZE / 2;
       
         for(int j = 0; j < BOARD_H; j++)
            for(int i = 0; i < BOARD_W; i++)
               if(mat[i][j])
                  drawBubble(buffer, toSx(i, j, x) + CELL_SIZE / 2,
                                     toSy(j, y) + CELL_SIZE / 2,
                                     (rad - 1) * zoom, mat[i][j] - 1);
               else
                  circle(buffer, toSx(i, j, x) + CELL_SIZE / 2,
                                 toSy(j, y) + CELL_SIZE / 2,
                                     (rad - 1) * zoom, makecol(255, 255, 255));
         /*
         rect(buffer, toSx(0, 0, x),
                      toSy(0, y),
                      toSx(BOARD_W, BOARD_H, x) + CELL_SIZE / 2,
                      toSy(BOARD_H, y), 0);*/
      }
};


int main()
{
    bool exit = false;
    
    init();
    
    Board board;
    int lastMb = 0;
    
    float camX = board.toWx(BOARD_W / 2, 0), camY = board.toWy(0) + 200;
    float camSpeed = 0.5f;
    
    int currBub = 1;
    int currGroup = 0;
    int groupStart[2] = {0, 7};
    int groupMax[2] = {7, 10};
    
    int lastKeySpace = 0;
    
    int numkeys[10] = {KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0};
    
    while(!exit)
    { 
      
     if(key[KEY_ESC]) exit = true;
     if(key[KEY_UP]) camY -= camSpeed;
     if(key[KEY_DOWN]) camY += camSpeed;
     if(key[KEY_LEFT]) camX -= camSpeed;
     if(key[KEY_RIGHT]) camX += camSpeed;
     if(key[KEY_EQUALS]) board.zoom += board.zoom * 0.001;
     if(key[KEY_MINUS]) board.zoom -= board.zoom * 0.001;
     
     if(key[KEY_SPACE])
     {
        if(!lastKeySpace)
          currGroup = (currGroup + 1) % 2;
        lastKeySpace = 1;
     }
     
     else
       lastKeySpace = 0;
     
     for(int i = 0; i < groupMax[currGroup]; ++i)
       if(key[numkeys[i]])
          currBub = groupStart[currGroup] + i + 1;
     
     if(board.zoom < 0.3)
       board.zoom = 0.3;
     if(board.zoom > 3)
       board.zoom = 3;
     
     if(mouse_b == 1)
     {
         int i = board.sxToI(mouse_x, mouse_y, camX, camY);
         int j = board.syToJ(mouse_y, camY);
         
         if(i >= 0 && j >=0 && i < BOARD_W && j < BOARD_H)
              board.mat[i][j] = currBub;
     }
     
     if(mouse_b == 2)
     {
         int i = board.sxToI(mouse_x, mouse_y, camX, camY);
         int j = board.syToJ(mouse_y, camY);
         
         if(i >= 0 && j >=0 && i < BOARD_W && j < BOARD_H)
              board.mat[i][j] = 0;
     }
     
     clear_to_color(buffer, makecol(64, 64, 64));
     
     board.render(buffer, camX, camY);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     rectfill(buffer, 0, 0, SCREEN_W, 50, 0);
     
     for(int i = 0; i < groupMax[currGroup]; ++i)
     {
        board.drawBubble(buffer, 100 + i * 30 + 10, 15, 10, groupStart[currGroup] + i);
        
        if(i + groupStart[currGroup] == currBub - 1)
           circle(buffer, 100 + i * 30 + 10, 15, 13, makecol(255, 0, 0));
        
        textprintf_ex(buffer, font, 108 + i * 30, 30, makecol(255, 255, 200),
		       -1, "%d", i + 1);
     }
     
     line(buffer, SCREEN_W / 2 - 5, SCREEN_H / 2, SCREEN_W / 2 + 5, SCREEN_H / 2, makecol(255, 0, 0));
     line(buffer, SCREEN_W / 2, SCREEN_H / 2 - 5, SCREEN_W / 2, SCREEN_H / 2 + 5, makecol(255, 0, 0));
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    Json::Value root;
    Json::Value data;
    int counter = 0;
    
    for(int j = 0; j < BOARD_H; ++j)
      for(int i = 0; i < BOARD_W; ++i)
        data[counter++] = board.mat[i][j];
      
    root["shots"] = 20;
    root["shotsForOneStar"] = 5;
    root["shotsForTwoStars"] = 10;
    root["shotsForThreeStars"] = 15;
    root["rows"] = BOARD_H;
    root["cols"] = BOARD_W;
    root["data"] = data;
    
    Json::Value mapping;
    
    int i = 0;
    
    mapping[i++] = "RED";
    mapping[i++] = "BLUE";
    mapping[i++] = "GREEN";
    mapping[i++] = "CYAN";
    mapping[i++] = "YELLOW";
    mapping[i++] = "MAGENTA";
    mapping[i++] = "GREY";
    
    mapping[i++] = "FOAM";
    mapping[i++] = "DUCK";
    mapping[i++] = "SOAP";
    mapping[i++] = "MAGIC";
    mapping[i++] = "SPONGE";
    mapping[i++] = "MOLD";
    mapping[i++] = "VENT";
    mapping[i++] = "AIR";
    mapping[i++] = "RAINBOW";
    mapping[i++] = "BLINKING";
    
    root["mapping"] = mapping;
    
    Json::Value infoRoot;
    
    infoRoot["worldId"] = "world1";
    infoRoot["name"] = "Awesome Level";
    infoRoot["difficulty"] = "Easy";
    infoRoot["contentFile"] = "out.lvl";
    
    Json::FastWriter writer;

    std::ofstream myfile;
    myfile.open ("out.lvl");
    myfile << writer.write(root);
    myfile.close();
    
    myfile.open ("out_info.json");
    myfile << writer.write(infoRoot);
    myfile.close();
    
    deinit();
    return 0;
}END_OF_MAIN()
