#include <stdio.h>
#include <conio.h>
#include <math.h>
#include <map>

#include <allegro.h>
#include <list>

#include <vector>

#include <fstream>

#include "DrawVec2f.hpp"

#include "Vec2f.hpp"
#include "Util.hpp"

#include "Poly.hpp"

#include "Clipper.hpp"
#include "Wad.hpp"

#include "BspNode.hpp"
#include "BspTree.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 1024, 768, 0, 0);
     
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
    
    std::ifstream file("maps/stas2.wad", std::ios::binary);
    
    if(!file.is_open())
    {
        printf("Couldn't load file!");
    }
    
    Wad wad(file);
    
    file.close();
    
    BspTree tree(wad);
    
    Vec2f pos, dir;
    float ang = 0.0;
    float movSpeed = 2.0, rotSpeed = 0.01;
    
    tree.calcLeaves();
    
    //printf("NODE NUM: %d\n", tree.root->nodeNum());
    printf("CONVEXNUM: %d\n", tree.polyList.size());
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     if(key[KEY_S]) pos.y += movSpeed;
     if(key[KEY_W]) pos.y -= movSpeed;
     if(key[KEY_A]) pos.x -= movSpeed;
     if(key[KEY_D]) pos.x += movSpeed;
     if(key[KEY_LEFT]) ang -= rotSpeed;
     if(key[KEY_RIGHT]) ang += rotSpeed;
      
     clear_to_color(buffer, makecol(64, 64, 64));
     
     //tree.drawGraph(buffer, SCREEN_W / 2, 300, 10, 1000);
     
     dir.x = 100 * cos(ang);
     dir.y = 100 * sin(ang);
     
     tree.draw(buffer, pos, dir);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
