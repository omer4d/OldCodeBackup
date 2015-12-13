#define ALLEGRO_USE_CONSOLE

#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <list>
#include <float.h>

#include "Util.hpp"
#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

BITMAP* buffer;

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 512, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
     
     //install_timer();
}

void deinit()
{
     destroy_bitmap(buffer);
}

// ************
// * QuadTree *
// ************

struct Node {
  Vec2f min, max;
  bool occupied;
  Node* children[4];
  int col;
  
  Node()
  {
    for(int i = 0; i < 4; i++)
      children[i] = NULL;
  }
  
  Node(Vec2f const& min, float w, float h)
  {
    this->min = min;
    this->max = min + Vec2f(w, h);
    
    for(int i = 0; i < 4; i++)
      children[i] = NULL;
  }
};

bool isEmpty(BITMAP* bitmap, Vec2f const& min, Vec2f const& max, int& col)
{
   for(int y = MAX(int(min.y + 1), 0); y < MIN(int(max.y), bitmap->h); y++)
    for(int x = MAX(int(min.x + 1), 0); x < MIN(int(max.x), bitmap->w); x++)    
      if((col = PIXEL(bitmap, x, y)) != MASK_COLOR_32)
        return false;
   return true;
}

void initNode(Node* node, BITMAP* bitmap)
{
  Vec2f& min = node->min;
  Vec2f& max = node->max;
  float w = (max.x - min.x) * 0.5f, h = (max.y - min.y) * 0.5f;
  
  if(!isEmpty(bitmap, min, max, node->col))
  {
     node->occupied = true;
     
     if(w > 1 && h > 1)
     {
       node->children[0] = new Node(min, w, h);
       node->children[1] = new Node(Vec2f(min.x + w, min.y), w, h);
       node->children[2] = new Node(Vec2f(min.x, min.y + h), w, h);
       node->children[3] = new Node(Vec2f(min.x + w, min.y + h), w, h);
       
       initNode(node->children[0], bitmap);
       initNode(node->children[1], bitmap);
       initNode(node->children[2], bitmap);
       initNode(node->children[3], bitmap);
     }
  }
  
  else
    node->occupied = false;
}

Node* createQuadTree(BITMAP* bitmap)
{
  Node* root = new Node(Vec2f(0.f, 0.f), SCREEN_W, SCREEN_H);
  
  if(root->max.x <= root->min.x || root->max.y <= root->min.y)
    return NULL;
    
  initNode(root, bitmap);
  return root;
}

void drawQuadTree(BITMAP* buffer, Node* root)
{
  if(root->occupied)
     DrawVec2f::rect(buffer, root->min, root->max, makecol(255, 0, 0));
  for(int i = 0; root->children[i] && i < 4; i++)
     drawQuadTree(buffer, root->children[i]);
}

// ******** 
// * Line *
// ********


bool contained(float i1, float i2, float p)
{
	if(i1 < i2) return p >= i1 && p <= i2;
	else        return p >= i2 && p <= i1;
}

bool lineVsAABB(Vec2f const& p1, Vec2f const& p2, Vec2f const& boxMin, Vec2f const& boxMax)
{
 Vec2f lineMin, lineMax, n(p1.y - p2.y, p2.x - p1.x);//(p2 - p1).unitNormal();
 
 /*
 if(p1.x < p2.x) { lineMin.x = p1.x; lineMax.x = p2.x; }
 else            { lineMin.x = p2.x; lineMax.x = p1.x; }
 
 if(p1.y < p2.y) { lineMin.y = p1.y; lineMax.y = p2.y; }
 else            { lineMin.y = p2.y; lineMax.y = p1.y; }*/
 
 lineMin.x = MIN(p1.x, p2.x);
 lineMin.y = MIN(p1.y, p2.y);
 lineMax.x = MAX(p1.x, p2.x);
 lineMax.y = MAX(p1.y, p2.y);
 
 if(boxMin.x > lineMax.x || boxMin.y > lineMax.y || lineMin.x > boxMax.x || lineMin.y > boxMax.y)
  return false;

 if(n.x * n.y > 0.f)
 {
  float i1 = boxMin.x * n.x + boxMin.y * n.y,
		i2 = boxMax.x * n.x + boxMax.y * n.y,
		p = p1.x * n.x + p1.y * n.y;
		
  return contained(i1, i2, p);
 }
 
 else
 {
  float i1 = boxMax.x * n.x + boxMin.y * n.y,
		i2 = boxMin.x * n.x + boxMax.y * n.y,
		p = p1.x * n.x + p1.y * n.y;
		
  return contained(i1, i2, p);
 }

/*
 if(n.x * n.y > 0.f)
 {
  float i1 = boxMin.dot(n), i2 = boxMax.dot(n), p = p1.dot(n);
  return contained(i1, i2, p);
 }
 
 else
 {
  float i1 = Vec2f(boxMax.x, boxMin.y).dot(n), i2 = Vec2f(boxMin.x, boxMax.y).dot(n), p = p1.dot(n);
  return contained(i1, i2, p);
 }*/
}

// *********************
// * Line vs. QuadTree *
// *********************

int testNum = 0;



Node* lineVsNode(Vec2f const& p1, Vec2f const& p2, Node* node)
{
 Node* res = NULL;
 
 static const int sequence[2][2][4] = {3, 2, 1, 0,
                                       1, 0, 3, 2,
                                       2, 3, 0, 1,
                                       0, 1, 2, 3};
 
 int dx = (p2.x - p1.x) >= 0.f;
 int dy = (p2.y - p1.y) >= 0.f;
 
 //testNum++;
 
 if(!node->children[0])
   res = node;
   
 else
   for(int i = 0; i < 4 && res == NULL; i++)
   {
     Node* child = node->children[sequence[dx][dy][i]];
     
     if(child->occupied && lineVsAABB(p1, p2, child->min, child->max))
       res = lineVsNode(p1, p2, child);
   }
   
 return res;
}

#define ANG_SUBDIV 360

#define DEPTH_BUFF_RES 512

void fillDepthBuff(float depthBuff[DEPTH_BUFF_RES], Node* root, Vec2f const& src)
{
	for(int i = 0; i < DEPTH_BUFF_RES; i++)
    {
        float alpha = float(i) / float(DEPTH_BUFF_RES - 1) * 2.f * M_PI;
        Vec2f dest(src.x + 400.f * cos(alpha), src.y + 400.f * sin(alpha));
        
        Node* res = lineVsNode(src, dest, root);
          
        if(res)
        {
			Vec2f p = (res->min + res->max) * 0.5f;
          	depthBuff[i] = (p - src).length();
        }
        
        else
        	depthBuff[i] = 1000000000.f;
    }
}

#define LS 200

Vec2f offsets[LS];

void initOffsets()
{
	 for(int i = 0; i < LS; ++i)
	 {
	 		float a = randf();
	   	    float b = randf();
	   	    	
	   	    if(b > a)
	   	    {
	   	    	float t = b;
	   	    	b = a;
	   	    	a = t;
	   	    }
	   	    
	 	    Vec2f rs(b*20.f*cos(2*M_PI*a/b), b*20.f*sin(2*M_PI*a/b));
	 	    
	 	    offsets[i] = rs;
	 }
}

void renderShadows(BITMAP* dest, Node* root, Vec2f const& src)
{
	 float depthBuff[LS][DEPTH_BUFF_RES];
	 Vec2f srcs[LS];
	
	 for(int i = 0; i < LS; ++i)
	 {
	 	
	 	/*
	 		float a = randf();
	   	    float b = randf();
	   	    	
	   	    if(b > a)
	   	    {
	   	    	float t = b;
	   	    	b = a;
	   	    	a = t;
	   	    }
	   	    
	 	    Vec2f rs(src.x + b*40.f*cos(2*M_PI*a/b), src.y + b*40.f*sin(2*M_PI*a/b));
	 	    */
	 	    
	 	    srcs[i] = src + offsets[i];
	 	    
            fillDepthBuff(depthBuff[i], root, srcs[i]);
	 }
	
	for(int y = 0; y < dest->h; ++y)
	   for(int x = 0; x < dest->w; ++x)
	   {
	   	    int count = 0;
	   	    int n = 8;
	   	    
	   	    for(int z = 0; z < n; ++z)
	   	    {  
	   	      int zaza = (x * y * (z * 111) ^ (x + y * 11 + z)) % LS;
	   	      Vec2f delta = Vec2f(x, y) - srcs[zaza];
	   	      float dist = delta.length();
	   	      float ang = Vec2f::ang(Vec2f(1, 0), delta, 1, dist);
	   	      int i = (int)((ang / (2.f * M_PI)) * DEPTH_BUFF_RES);
	   	      
	   	      if(dist > depthBuff[zaza][i])
	   	        ++count;
	   	    }
	   	      
	   	    int br = 255 - (int)((float)count / n * 200.f);
	   	    
			/*int ps =    PIXEL(dest, x, y);
	   	    
	   	    int r = (int)((getr32(ps) * 0.75 + (br + 128) * 0.25 ));
	   	    int g = (int)((getg32(ps) * 0.75 + (br + 128) * 0.25 ));
	   	    int b = (int)((getb32(ps) * 0.75 + (br + 128) * 0.25));
	   	      
	   	      if(r > 255)
	   	      r = 255;
	   	      if(g > 255)
	   	      g = 255;
	   	      if(b > 255)
	   	      b = 255;*/
	   	      
		   PIXEL(dest, x, y) = makecol32(br,  br, br);
	   }
}

void renderShadows2(BITMAP* dest, Node* root, Vec2f const& src)
{
	 float depthBuff[DEPTH_BUFF_RES];
	 
	 fillDepthBuff(depthBuff, root, src);
	
	for(int y = 0; y < dest->h; ++y)
	   for(int x = 0; x < dest->w; ++x)
	   {
	   	   Vec2f delta = Vec2f(x, y) - src;
	   	   float dist = delta.length();
	   	   float ang = Vec2f::ang(Vec2f(1, 0), delta, 1, dist);
	   	   int i = (int)((ang / (2.f * M_PI)) * DEPTH_BUFF_RES);
		   PIXEL(dest, x, y) = dist > depthBuff[i] ? makecol32(128, 128, 128) : makecol32(255, 255, 255);
	   }
}

int main()
{
    bool exit = false;
    
    init();
    
    initOffsets();
    
    Vec2f p1, p2;
    BITMAP* map = load_bitmap("map2.bmp", NULL);
    BITMAP* accum = create_bitmap(map->w, map->h);
    Node* root = createQuadTree(map);
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b == 1) p1.init(mouse_x, mouse_y);
     if(mouse_b == 2) p2.init(mouse_x, mouse_y);
     
     clear_to_color(buffer, makecol(255, 255, 255));
     
     
     renderShadows(accum, root, p1);
     draw_sprite(buffer, accum, 0, 0);
     draw_sprite(buffer, map, 0, 0);
     
     DrawVec2f::line(buffer, p1, p2, makecol(0, 255, 0));
     DrawVec2f::circlefill(buffer, p1, 3, makecol(255, 255, 0));
     
     testNum = 0;
     Node* res = lineVsNode(p1, p2, root);
     
     if(res)
     {
       DrawVec2f::circlefill(buffer, (res->min + res->max) * 0.5, 3, res->col);
       DrawVec2f::circle(buffer, (res->min + res->max) * 0.5, 3, 0);
     }
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(map);
    deinit();
    return 0;
}END_OF_MAIN()
