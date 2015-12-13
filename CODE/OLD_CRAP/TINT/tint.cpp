#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>

#include "Col3f.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

int mod(int x, int n)
{
    int r = x % n;
    if(r < 0)
     r += n;
    return r;
}

BITMAP* proc(BITMAP* src, int col)
{
   BITMAP* dest = create_bitmap(src->w, src->h);
   
   for(int j = 0; j < src->h; j++)
     for(int i = 0; i < src->w; i++)
     {
        int p = PIXEL(src, i, j);
        float br = (getr(p) * 0.25 + getg(p) * 0.6 + getb(p) * 0.15) / 255.0;
        
        float r = br * 255 + (255 * br + getr(col) * (1 - br)) - 255;
        float g = br * 255 + (255 * br + getg(col) * (1 - br)) - 255;
        float b = br * 255 + (255 * br + getb(col) * (1 - br)) - 255;
        
        if(r > 255) r = 255;
        if(g > 255) g = 255;
        if(b > 255) b = 255;
        
        if(r < 0) r = 0;
        if(g < 0) g = 0;
        if(b < 0) b = 0;
        
        PIXEL(dest, i, j) = makecol(int(r), int(g), int(b));
     }
   
   return dest;
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* src = load_bitmap("test.bmp", NULL);
    
    if(!src)
    printf("WTF?!");
    
    BITMAP* dest = proc(src, makecol(255, 0, 0));
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     if(key[KEY_SPACE])
     {
       destroy_bitmap(dest);
       
       float r = rand() % 255;
       float g = rand() % 255;
       float b = rand() % 255;
       
       float max = MAX(MAX(r, g), b);
       
       r *= 255.0 / max;
       g *= 255.0 / max;
       b *= 255.0 / max;
       
       printf("%f %f %f\n", r, g, b);
        
       dest = proc(src, makecol(int(r), int(g), int (b)));
       key[KEY_SPACE] = 0;
     }
     
     clear_to_color(buffer, 0);
     draw_sprite(buffer, dest, 0, 0);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
