#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>

#include <vector>

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
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

int main()
{
    bool exit = false;
    
    init();
  
    float r = 12;
    float k = pow(2.0, 1.0 / 12.0);
    
    float y0 = pow(k, r);
    float y1 = pow(k, -r);
    
    printf("%.4f, %.4f", (y1 - y0) * 0.5, (y1 + y0) * 0.5);
    
  
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     
     clear_to_color(buffer, makecol(64, 64, 64));
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
