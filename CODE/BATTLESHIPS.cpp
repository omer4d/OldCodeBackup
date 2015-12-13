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

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
}

void deinit()
{
     destroy_bitmap(buffer);
}

struct Cell {
    int score;
    bool mark;
    
    Cell()
    {
       score = 0;
       mark = false;
    }
};

struct Board {
   static const int SIZE = 10;
   
   Cell data[SIZE + 2][SIZE + 2];
   
   Board()
   {
      for(int c = 0; c < SIZE + 2; ++c)
      {
         data[c][0].mark = true;
         data[c][SIZE + 1].mark = true;
         
         data[0][c].mark = true;
         data[SIZE + 1][c].mark = true;
      }
   }
   
   Cell& at(int i, int j)
   {
       return data[i + 1][j + 1];
   }
   
   static int score(int i0, int i1, int i2, int n)
   {
       return MIN(i2 - n - i0, i1 - i0);
   }
};

int main()
{
    bool exit = false;
    
    init();
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     clear_to_color(buffer, makecol(64, 64, 64));

     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
