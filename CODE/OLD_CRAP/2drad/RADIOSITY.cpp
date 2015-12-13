#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>


#include "Util.hpp"
#include "Vec2f.hpp"
#include "Col3f.hpp"

#include "Scene.hpp"

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 320, 240, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
     
     DrawVec2f::init(SCREEN_W / 2, SCREEN_H / 2);
}

void deinit()
{
     destroy_bitmap(buffer);
}

float ang(Vec2f d)
{
 return atan2(-d.y, -d.x);
}

int main()
{
    bool exit = false;
    
    init();
    
    Scene scene(256);
    scene.initProjection(1, -1, 1, 1000);
    
    for(int i = 0; i < 10; i++)
    {
      Vec2f a(randf(-SCREEN_W * 0.5, SCREEN_W * 0.5),
              randf(-SCREEN_H * 0.5, SCREEN_H * 0.5));
      
      float rad = 10 + float(rand()) / RAND_MAX * 50;
      float ang = float(rand()) / RAND_MAX * 2 * M_PI;
      Vec2f b(a.x + rad * cos(ang), a.y + rad * sin(ang));
      Vec2f n = (b - a).normal().unit() * 3;
      Vec2f c = b + n;
      Vec2f d = a + n;

      scene.pushColor(Col3f(randf(0, 50), randf(0, 50), randf(0, 50)));
      scene.pushVertex(a);
      scene.pushVertex(b);
      
      scene.pushVertex(b);
      scene.pushVertex(c);
      
      scene.pushVertex(c);
      scene.pushVertex(d);
      
      scene.pushVertex(d);
      scene.pushVertex(a);
    }
    

    scene.setMode(POINT_MODE);
    for(int i = 0; i < 1; i++)
    {
     Vec2f a(randf(-SCREEN_W * 0.5, SCREEN_W * 0.5),
             randf(-SCREEN_H * 0.5, SCREEN_H * 0.5));
     
     a.x = 0;
     a.y = 0;
     
      scene.pushColor(Col3f(70, 70, 70));
      scene.pushVertex(a);
    }
    
    scene.uploadFinished();
    
    
    float camSpeed = 0.5;
    
    BITMAP* lightmap = create_bitmap(SCREEN_W, SCREEN_H);
    Col3f light;
    
    int startTime = time(NULL);
    
    for(int j = 0; j < lightmap->h; j++)
    {
     for(int i = 0; i < lightmap->w; i++)
      {
       scene.camDisp.x = i - SCREEN_W / 2;
       scene.camDisp.y = j - SCREEN_H / 2;
       
       scene.camAng = 0.0;
       scene.render();
       light = scene.colorBufferSum();
       
       scene.camAng = M_PI * 0.5;
       scene.render();
       light += scene.colorBufferSum();
       
       scene.camAng = M_PI;
       scene.render();
       light += scene.colorBufferSum();
       
       scene.camAng = M_PI * 1.5;
       scene.render();
       light += scene.colorBufferSum();
       
       light *= (1.0 / 100.0);
       
       putpixel(lightmap, i, j, makecol(clamp255(light.r), clamp255(light.g), clamp255(light.b)));
      }
      
      printf("%f\n", j / float(lightmap->h - 1) * 100.0);
     }
     
     printf("%d SECONDS!", time(NULL) - startTime);
     
     
     int lineNum = 0;
     bool lastSpace = 0;
     float alpha = 0.0;
     
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     if(key[KEY_W]) scene.camDisp.y -= camSpeed * 0.2;
     if(key[KEY_S]) scene.camDisp.y += camSpeed * 0.2;
     if(key[KEY_A]) scene.camDisp.x -= camSpeed * 0.2;
     if(key[KEY_D]) scene.camDisp.x += camSpeed * 0.2;
     
     if(key[KEY_UP]) alpha += 0.001; //scene.camAng -= camSpeed * 0.002;
     if(key[KEY_DOWN]) alpha -= 0.001; //scene.camAng += camSpeed * 0.002;
     
     if(alpha < 0.0) alpha = 1.0;
     if(alpha > 1.0) alpha = 0.0;
     
     if(key[KEY_SPACE])
     {
      
      if(!lastSpace)
      {
      if(lineNum > scene.lineVertexVec.size() / 2 - 1)
       lineNum = 0;
     
     lineNum++;
     }
     
     lastSpace = true;
     }
     
     else
      lastSpace = false;
     
     
     Vec2f v1 = scene.lineVertexVec[lineNum * 2].pos, v2 = scene.lineVertexVec[lineNum * 2 + 1].pos;
     scene.camDisp = v1 * alpha + v2 * (1 - alpha);
     scene.camAng = ang((v2 - v1).normal());
     
     
     //scene.camAng = ang(scene.camDisp - Vec2f(mouse_x - SCREEN_W / 2, mouse_y - SCREEN_H / 2) );
     //scene.camDisp = 
     
     clear_to_color(buffer, makecol(200, 100, 100));
     draw_sprite(buffer, lightmap, 0, 0);
     blit(lightmap, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
     
     scene.render();
     scene.debugRender(buffer);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     scene.output(buffer);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(lightmap);
    deinit();
    return 0;
}END_OF_MAIN()
