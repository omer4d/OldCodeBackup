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
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
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

void findBitmapBounds(BITMAP* bitmap, Vec2f& min, Vec2f& max)
{
  min.init(FLT_MAX, FLT_MAX);
  max.init(-FLT_MAX, -FLT_MAX);
  
  for(int y = 0; y < bitmap->h; y++)
    for(int x = 0; x < bitmap->w; x++)
      if(PIXEL(bitmap, x, y))
      {
        if(x < min.x) min.x = x;
        else if(x > max.x) max.x = x;
        
        if(y < min.y) min.y = y;
        else if(y > max.y) max.y = y;
      }
      
  float w = max.x - min.x, h = max.y - min.y;
      
  if(w < h) max.x = min.x + h;
  else max.y = min.y + w;
}

bool isEmpty(BITMAP* bitmap, Vec2f const& min, Vec2f const& max)
{
   for(int y = MAX(int(min.y + 1), 0); y < MIN(int(max.y), bitmap->h); y++)
    for(int x = MAX(int(min.x + 1), 0); x < MIN(int(max.x), bitmap->w); x++)    
      if(PIXEL(bitmap, x, y))
        return false;
   return true;
}

void initNode(Node* node, BITMAP* bitmap)
{
  Vec2f& min = node->min;
  Vec2f& max = node->max;
  float w = (max.x - min.x) * 0.5, h = (max.y - min.y) * 0.5;
  
  if(!isEmpty(bitmap, min, max))
  {
     node->occupied = true;
     
     if(w > 2 && h > 2)
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
  Node* root = new Node();
  findBitmapBounds(bitmap, root->min, root->max);
  
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
 return fabs(p - i1) + fabs(p - i2) <= fabs(i2 - i1);
}

bool lineVsAABB(Vec2f const& p1, Vec2f const& p2, Vec2f const& boxMin, Vec2f const& boxMax)
{
 Vec2f lineMin, lineMax, n = (p2 - p1).normal();
 
 lineMin.x = p1.x < p2.x ? p1.x : p2.x;
 lineMin.y = p1.y < p2.y ? p1.y : p2.y;
 lineMax.x = p1.x > p2.x ? p1.x : p2.x;
 lineMax.y = p1.y > p2.y ? p1.y : p2.y;
 
 if(boxMin.x > lineMax.x || boxMin.y > lineMax.y || lineMin.x > boxMax.x || lineMin.y > boxMax.y)
  return false;
 
 if(n.x * n.y > 0)
 {
  float i1 = boxMin.dot(n), i2 = boxMax.dot(n), p = p1.dot(n);
  return contained(i1, i2, p);
 }
 
 else
 {
  float i1 = Vec2f(boxMax.x, boxMin.y).dot(n), i2 = Vec2f(boxMin.x, boxMax.y).dot(n), p = p1.dot(n);
  return contained(i1, i2, p);
 }
}

// *********************
// * Line vs. QuadTree *
// *********************

int testNum = 0;

Node* lineVsNode(Vec2f const& p1, Vec2f const& p2, Node* root)
{
 int sequence[2][2][4] = {3, 2, 1, 0,
                          1, 0, 3, 2,
                          2, 3, 0, 1,
                          0, 1, 2, 3};
 
 int dx = (p2.x - p1.x) >= 0;
 int dy = (p2.y - p1.y) >= 0;
  
 Node* res = NULL;
 
      testNum++;
 
 if(!root->children[0])
   res = root;
   
 else
   for(int i = 0; i < 4 && res == NULL; i++)
   {
     Node* node = root->children[sequence[dx][dy][i]];
     if(node->occupied && lineVsAABB(p1, p2, node->min, node->max))
       res = lineVsNode(p1, p2, node);
   }
   
 return res;
}

int main()
{
    bool exit = false;
    
    init();
    
    BITMAP* map = create_bitmap(SCREEN_W, SCREEN_H);
    clear_bitmap(map);
    
    for(int i = 0; i < 100; i++)
      circlefill(map, randi(100, SCREEN_W - 100), randi(100, SCREEN_H - 100), randi(10, 70), makecol(255, 255, 255));
      
    Node* root = createQuadTree(map);
    
    Vec2f p1(100, 100), p2(200, 200);
    
    while(!exit)
    { 
     if(key[KEY_ESC]) exit = true;
     
     if(mouse_b == 1) p1.init(mouse_x, mouse_y);
     if(mouse_b == 2) p2.init(mouse_x, mouse_y);
     
     
     
     clear_to_color(buffer, 0);
     
     draw_sprite(buffer, map, 0, 0);
     
     if(key[KEY_SPACE])
     drawQuadTree(buffer, root);
     DrawVec2f::line(buffer, p1, p2, makecol(0, 255, 0));
     DrawVec2f::circlefill(buffer, p1, 3, makecol(255, 255, 0));
     
     testNum = 0;
     Node* res = lineVsNode(p1, p2, root);
     if(res)
       DrawVec2f::circlefill(buffer, (res->min + res->max) * 0.5, 2, makecol(0, 0, 255));
       
     printf("%d\n", testNum);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(map);
    deinit();
    return 0;
}END_OF_MAIN()
