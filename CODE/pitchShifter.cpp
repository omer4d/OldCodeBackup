#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

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

void graph(BITMAP* buffer, int dx, int dy, float scale, float buff[], int n, int col)
{
    float lastX = 0, lastY = buff[0];
    
    for(int i = 1; i < n; ++i)
    {
       line(buffer, int(dx + lastX), int(dy - lastY * scale), int(dx + i), int(dy - buff[i] * scale), col);
       lastX = i;
       lastY = buff[i]; 
    }
}

void convolve(float out[], float a[], int an, float b[], int bn)
{
     for(int i = 0; i < an; ++i)
     {
        out[i] = 0;
        
        for(int j = 0; j < bn; ++j)
        {
            float tmp = i + j < an ? a[i + j] : 0;
            out[i] += fabs(b[j] - tmp);
        }
     }
}

int best(float target[], int n, int frame)
{
     float bestD = 100000;
     int bestI = -1;
 
     for(int i = 0; i < n - frame * 2; ++i)
     {
        float d = 0;
        
        for(int j = 0; j < frame; ++j)
            d += fabs(target[n - frame + j] - target[i + j]);
            
        if(d < bestD)
        {
           bestD = d;
           bestI = i;
        }
     }
     
     return bestI + frame;
}

#define SZ 30

int main()
{   
    bool exit = false;
    
    init();
    
    float buff[512];
    float f = 2.61f;
    
    for(int i = 0; i < 512; ++i)
    {
       float t = float(i) / 512;
       buff[i] = (sin(M_PI * 2.f * f * t) * cos(f * 30 * t)) / (t * 10);
    }
    
    float chunk[SZ];
    
    for(int i = 0; i < SZ; ++i)
      chunk[SZ - i] = buff[511 - i];
      
    float conv[512];
    
    convolve(conv, buff, 512, chunk, SZ);
    
    int temp = best(buff, 512, SZ);
    
    /*
    for(int i = 0; i < 50; ++i)
    {
      float tmp = chunk[99 - i];
      chunk[99 - i] = chunk[i];
      chunk[i] = tmp;
    }*/
    
    float offs = 0;
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     if(key[KEY_LEFT]) offs -= 0.1;
     if(key[KEY_RIGHT]) offs += 0.1;
     
     clear_to_color(buffer, 0);
     
     graph(buffer, 130, SCREEN_H / 2, 100, buff, 512, makecol(255, 0, 0));
     graph(buffer, 130, SCREEN_H / 2, 5, conv, 512, makecol(0, 255, 0));
     graph(buffer, int(130 + offs), SCREEN_H / 2, 100, chunk, SZ, makecol(255, 255, 0));
     
     line(buffer, 130 + temp, SCREEN_H / 2, 130 + temp, SCREEN_H / 2 - 200, makecol(255, 0, 0));
     
     //for(int i = 0; i < 512; ++i)
       //line(buffer, i, SCREEN_H / 2, i, int(SCREEN_H / 2 + buff[i]), makecol(255, 0, 0));
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
