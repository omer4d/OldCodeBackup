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
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0);
     
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


void calcFinalWeights(Buffer<float>& weights)
{
      float len2 = weights.len * 0.5;
 
      for(int i = 0; i < weights.len; i++)
      {
         weights.data[i] = len2 / sqrt(len2*len2 + SQUARED(i - len2));
         weights.data[i] *= weights.data[i];
      }
}

void calcSurfaceWeights(Buffer<float>& weights, int dir)
{
      float len2 = weights.len * 0.5;

 
      for(int i = 0; i < weights.len; i++)
      {
         int h;
         if(dir) h = i;
         else    h = weights.len - i;
         
         float A = h, B = sqrt(len2 * len2 + len2 * len2), C = sqrt(len2 * len2 + (h - len2) * (h - len2));
         
         
         weights.data[i] = len2 / sqrt(len2*len2 + SQUARED(i - len2));
         //weights.data[i] *= weights.data[i];
         weights.data[i] *= (B * B + C * C - A * A) / (2.0 * B * C);
      }
}

void drawWeights(BITMAP* buffer, Buffer<float>& weights)
{
        int w = 100, h = 1;
        
        for(int i = 0; i < weights.len; i++)
        {
         float k = 255 * weights.data[i];
         rectfill(buffer, 0, i * h, w, (i + 1) * h, makecol(clamp255(k), clamp255(k), clamp255(k)));
        }
}

void drawColorBuffer(BITMAP* buffer, Buffer<Col3f>& colorBuffer)
{
        int w = 100, h = 5;
        
        for(int i = 0; i < colorBuffer.len; i++)
        {
         Col3f& c = colorBuffer.data[i];
         rectfill(buffer, 0, i * h, w, (i + 1) * h, makecol(clamp255(c.r), clamp255(c.g), clamp255(c.b)));
        }
}

float brightness(float l, float e)
{
    return 1 - exp(-l * e);
}

float luminance(float r, float g, float b)
{
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

int main()
{
    bool exit = false;
    
    init();
    
    Scene scene(512);
    scene.initProjection(1, -1, 1, 1000);
    
    for(int i = 0; i < 30; i++)
    {
      Vec2f a(randf(-SCREEN_W * 0.5, SCREEN_W * 0.5),
              randf(-SCREEN_H * 0.5, SCREEN_H * 0.5));
      
      float rad = 30 + float(rand()) / RAND_MAX * 40;
      float ang = float(rand()) / RAND_MAX * 2 * M_PI;
      Vec2f b(a.x + rad * cos(ang), a.y + rad * sin(ang));
      Vec2f n = (b - a).normal().unit() * 3;
      Vec2f c = b + n;
      Vec2f d = a + n;

      scene.pushColor(Col3f(randf(), randf(), randf()));
      scene.pushVertex(a);
      scene.pushVertex(b);
      
      scene.pushVertex(b);
      scene.pushVertex(c);
      
      scene.pushVertex(c);
      scene.pushVertex(d);
      
      scene.pushVertex(d);
      scene.pushVertex(a);
    }
    
    Vec2f box[4];
    box[3] = Vec2f(SCREEN_W * -0.49, SCREEN_H * -0.49);
    box[2] = Vec2f(SCREEN_W * 0.49, SCREEN_H * -0.49);
    box[1] = Vec2f(SCREEN_W * 0.49, SCREEN_H * 0.49);
    box[0] = Vec2f(SCREEN_W * -0.49, SCREEN_H * 0.49);
                   
    scene.pushColor(Col3f(1, 1, 1));
    scene.pushPoly(box, 4);
    

    scene.setMode(POINT_MODE);
    for(int i = 0; i < 1; i++)
    {
     Vec2f a(randf(-SCREEN_W * 0.5, SCREEN_W * 0.5),
             randf(-SCREEN_H * 0.5, SCREEN_H * 0.5));
     
     a.x = 0;
     a.y = 0;
     
      scene.pushColor(Col3f(200, 200, 200));
      scene.pushVertex(a);
    }
    
    scene.uploadFinished();
    
    
    float camSpeed = 0.5;
    
    BITMAP* lightmap = create_bitmap(SCREEN_W, SCREEN_H);
    Col3f light;
    
    int startTime = time(NULL);
    
    
    // ***************
    // * SURFACE LMS *
    // ***************
    
    std::vector<Buffer<Col3f>*> lightmapVec;
    Buffer<float> surfaceWeights1(scene.colorBuffer.len);
    Buffer<float> surfaceWeights2(scene.colorBuffer.len);
    
    calcSurfaceWeights(surfaceWeights1, 0);
    calcSurfaceWeights(surfaceWeights2, 1);
    
    for(int i = 0; i < scene.lineVertexVec.size() / 2; i++)
    {
      Buffer<Col3f>* lm = new Buffer<Col3f>(SURFACE_LIGHTMAP_SIZE);
      lm->clear(Col3f(0, 0, 0));
      lightmapVec.push_back(lm);
    }
    
    for(int pass = 0; pass < 20; pass++)
    {
    
    for(int i = 0; i < scene.lineVertexVec.size() / 2; i++)
    {
     Vec2f v1 = scene.lineVertexVec[i* 2].pos, v2 = scene.lineVertexVec[i * 2 + 1].pos;
     
     for(int j = 0; j < SURFACE_LIGHTMAP_SIZE; j++)
      {
       float alpha = float(j) / float(SURFACE_LIGHTMAP_SIZE - i);
       scene.camDisp = v1 * alpha + v2 * (1 - alpha);
       
       scene.camAng = ang(v2 - v1) + M_PI * 0.25;
       scene.render();
       light = scene.colorBufferSum(surfaceWeights1);
       
       scene.camAng = ang(v2 - v1) + M_PI * 0.75;
       scene.render();
       light += scene.colorBufferSum(surfaceWeights2);
       
       lightmapVec[i]->data[j] = light * (1.0 / 600.0);
      }
    }
    
    for(int i = 0; i < scene.lineVertexVec.size() / 2; i++)
     scene.lightmapVec[i]->copy(*lightmapVec[i]);
    }
    
    
    // *************
    // * FINAL LM  *
    // *************
    
    Buffer<float> finalWeights(scene.colorBuffer.len);
    calcFinalWeights(finalWeights);
    
    
    for(int j = 0; j < lightmap->h; j++)
    {
     for(int i = 0; i < lightmap->w; i++)
      {
       scene.camDisp.x = i - SCREEN_W / 2;
       scene.camDisp.y = j - SCREEN_H / 2;
       
       scene.camAng = 0.0;
       scene.render();
       light = scene.colorBufferSum(finalWeights);
       
       scene.camAng = M_PI * 0.5;
       scene.render();
       light += scene.colorBufferSum(finalWeights);
       
       scene.camAng = M_PI;
       scene.render();
       light += scene.colorBufferSum(finalWeights);
       
       scene.camAng = M_PI * 1.5;
       scene.render();
       light += scene.colorBufferSum(finalWeights);
       
       //float q = brightness(luminance(light.r, light.g, light.b), 0.001) / luminance(light.r, light.g, light.b) * 255;
       light *= (1.0 / 300.0);
       
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
     
     if(key[KEY_UP]) scene.camAng -= camSpeed * 0.002;
     if(key[KEY_DOWN]) scene.camAng += camSpeed * 0.002;
     
     /*
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
     scene.camAng = ang(v2 - v1) + M_PI * 0.75;*/
     
     
     //scene.camAng = ang(scene.camDisp - Vec2f(mouse_x - SCREEN_W / 2, mouse_y - SCREEN_H / 2) );
     //scene.camDisp = 
     
     clear_to_color(buffer, makecol(200, 100, 100));
     draw_sprite(buffer, lightmap, 0, 0);
     blit(lightmap, buffer, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
     
     //scene.render();
     //scene.debugRender(buffer);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     //scene.output(buffer);
     //drawColorBuffer(buffer, *lightmapVec[0]);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(lightmap);
    deinit();
    return 0;
}END_OF_MAIN()
