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

struct Keyboard {
    int x, y, whiteW, whiteH, blackW, blackH;
    static const int halfKeyNum = 60;
    static const int keyNum = halfKeyNum * 2;
    bool pressed[halfKeyNum * 2], lastPressed[halfKeyNum * 2], held[halfKeyNum * 2];
    int lastMb;
    
    Keyboard(int x, int y, int kw, int kh, float sw, float sh)
    {
        this->x = x;
        this->y = y;
        this->whiteW = kw;
        this->whiteH = kh;
        this->blackW = int(kw * sw);
        this->blackH = int(kh * sh);
        lastMb = 0;
        
        for(int i = 0; i < halfKeyNum * 2; ++i)
        {
            held[i] = false;
            lastPressed[i] = false;
            pressed[i] = false;
        }
    }
    
    void hcenteredRect(BITMAP* buffer, int x, int y, int w, int h, int c)
    {
        rectfill(buffer, x - w / 2, y, x + w / 2, y + h, c);
        rect(buffer, x - w / 2, y, x + w / 2, y + h, 0);
    }
    
    bool isInside(int px, int py)
    {
        int w = whiteW * halfKeyNum;
        return px >= x && py >= y &&
               px < x + w && py < y + whiteH;
    }
    
    int whiteIndex(int i)
    {
        return i * 2 + 1;
    }
    
    int blackIndex(int i)
    {
        return i * 2;
    }
    
    int getKey(int px, int py)
    {
        if(isInside(px, py))
        {
            int i1 = (px - x) / whiteW;
            int i2 = (px - x + whiteW / 2) / whiteW;
            
            if(py - y <= blackH && fabs(mouse_x - (x + i2 * whiteW)) <= blackW / 2)
               return blackIndex(i2);
            else
               return whiteIndex(i1);
        }
        
        return -1;
    }
    
    void logic()
    {
        int key = getKey(mouse_x, mouse_y);
        int mb = mouse_b;
     
        for(int i = 0; i < keyNum; ++i)
          if(!held[i])
           pressed[i] = false;
     
        if(!lastMb && mb == 2 && key > -1)
        {
           held[key] = !held[key];
           pressed[key] = !pressed[key];
        }
     
        if(mouse_b == 1 && key > -1)
        {
           held[key] = false;
           pressed[key] = true;
        }
        
        lastMb = mb;
        
        for(int i = 0; i < keyNum; ++i)
        {
           /*
           if(!lastPressed[i] && pressed[i])
             printf("On %d\n", i);
           else if(lastPressed[i] && !pressed[i])
             printf("Off %d\n", i);*/
           
           lastPressed[i] = pressed[i];
        }
    }
    
    void draw(BITMAP* buffer)
    {
        for(int i = 0; i < halfKeyNum; ++i)
        {
            int col = pressed[whiteIndex(i)] ? makecol(255, 200, 200) : makecol(255, 255, 255);   
            rectfill(buffer, x + i * whiteW, y, x + (i + 1) * whiteW, y + whiteH, col);
        }
        
        for(int i = 0; i < halfKeyNum; ++i)
        {
            rect(buffer, x + i * whiteW, y, x + (i + 1) * whiteW, y + whiteH, 0);
        }
        
        for(int i = 0; i < halfKeyNum; ++i)
        {
            int col = pressed[blackIndex(i)] ? makecol(130, 0, 0) : 0;  
            hcenteredRect(buffer, x + i * whiteW, y, blackW, blackH, col);
        }
    }
};

int main()
{
    bool exit = false;
    Keyboard keyboard(0, 0, 20, 50, 0.5, 0.6);
    
    init();
  
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     keyboard.logic();
     
     clear_to_color(buffer, makecol(64, 64, 64));
     keyboard.draw(buffer);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
