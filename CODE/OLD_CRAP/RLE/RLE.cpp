#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>
#include <list>

#include "Util.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 800, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

struct Row {
   int val, len;
   
   Row(int val, int len)
   {
      this->val = val;
      this->len = len;
   }
};

struct RLE {
   int w, h, len;
   int* data;
};

RLE* encode(BITMAP* bitmap)
{
   std::list<Row> rows;
   int* data = (int*)bitmap->dat;
   int lastCol = data[0], lastIndex = 0;
   
   for(int i = 1; i < bitmap->w * bitmap->h; i++)
      if(data[i] != lastCol)
      {
         rows.push_back(Row(lastCol, i - lastIndex));
         lastCol = data[i];
         lastIndex = i;
      }
      
   RLE* rle = new RLE();
   rle->w = bitmap->w;
   rle->h = bitmap->h;
   rle->len = rows.size() * 2;
   rle->data = new int[rows.size() * 2];
   int n = 0;
   
   for(std::list<Row>::iterator i = rows.begin(); i != rows.end(); ++i)
   {
      rle->data[n] = i->val;
      rle->data[n + 1] = i->len;
      n += 2;
   }
   
   return rle;
}

BITMAP* decode(RLE* rle)
{
   BITMAP* bitmap = create_bitmap(rle->w, rle->h);
   int px = 0;
   
   for(int i = 0; i < rle->len / 2; i++)
   {
       int val = rle->data[i * 2];
       int len = rle->data[i * 2 + 1];
       
       for(int j = 0; j < len; j++)
       {
           int x = px % rle->w, y = px / rle->w;
           PIXEL(bitmap, x, y) = val;
           px++;
       }
   }
   
   return bitmap;
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* test = create_bitmap(600, 600);
    clear_to_color(test, makecol(64, 64, 128));
    
    for(int i = 0; i < 20000; i++)
      circlefill(test, rand() % test->w, rand() % test->h, 100 + rand() % 10, makecol(rand() % 256, rand() % 256, rand() % 256));
    
    RLE* rle = encode(test);
    BITMAP* res = decode(rle);
    printf("%f", float(rle->len) / float(test->w * test->h) * 100.0);
    
    while(!exit)
    {                
     if(key[KEY_ESC]) exit = true;

     clear_to_color(buffer, 0); 
     
     draw_sprite(buffer, test, 0, 0);
     if(key[KEY_SPACE])
        draw_sprite(buffer, res, 0, 0);
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
