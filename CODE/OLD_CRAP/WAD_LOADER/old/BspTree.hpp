#ifndef _BSPTREE_HPP_
#define _BSPTREE_HPP_

#include "Vec2f.hpp"

#include "BspNode.hpp"

struct BspTree {
     BspNode* root;
     PolyList polyList;
     Vec2f min, max;
     Wad::Lump<Wad::Linedef>* lines;
     
     void build(Wad& wad)
     {
         Wad::Lump<Wad::Linedef>& linedefLump = *wad.linedefLump;
         Wad::Lump<Wad::Vertex>& vertexLump = *wad.vertexLump;
         lines = wad.linedefLump;
         
         min.x = FLT_MAX;
         min.y = FLT_MAX;
         max.x = -FLT_MAX;
         max.y = -FLT_MAX;
         
         for(int i = 0; i < vertexLump.n; i++)
         {
             Wad::Vertex& vtx = vertexLump.arr[i];
             
             if(vtx.x < min.x) min.x = vtx.x;
             if(vtx.y < min.y) min.y = vtx.y;
             if(vtx.x > max.x) max.x = vtx.x;
             if(vtx.y > max.y) max.y = vtx.y;
         }
         
         min.x -= 10.0;
         min.y -= 10.0;
         max.x += 10.0;
         max.y += 10.0;
         
         root = new BspNode(linedefLump.arr[0]);
         
         for(int i = 1; i < linedefLump.n; i++)
         {
             Vec2f ip;
             BspNode* node = new BspNode(linedefLump.arr[i]);
             root->addChild(node);
         }
     }
     
     void destroy()
     {
         delete root;
     }
     
     BspTree(Wad& wad)
     {
         build(wad);
     }
     
     ~BspTree()
     {
         destroy();
     }
     
     void calcLeaves()
     {
          Clipper* clipper = new Clipper();
          
          clipper->pushPoint(Vec2f(min.x, min.y));
          clipper->pushPoint(Vec2f(max.x, min.y));
          clipper->pushPoint(Vec2f(max.x, max.y));
          clipper->pushPoint(Vec2f(min.x, max.y));
          
          root->calcLeaves(polyList, *clipper);
          
          delete clipper;
     }
     
     void draw(BITMAP* buffer, Vec2f pos, Vec2f dir)
     {
          DrawVec2f::sx = 0.3;
          DrawVec2f::sy = 0.3;
          DrawVec2f::tx = -pos.x * DrawVec2f::sx + SCREEN_W / 2;
          DrawVec2f::ty = -pos.y * DrawVec2f::sy + SCREEN_H / 2;
          
          //root->draw(buffer, pos, dir);
          
          for(PolyIter i = polyList.begin(); i != polyList.end(); ++i)
             DrawVec2f::poly(buffer, *i, makecol(255, 0, 0), -1);
          
          for(PolyIter i = polyList.begin(); i != polyList.end(); ++i)
             DrawVec2f::poly(buffer, *i, -1, 0);
          
          for(int i = 0; i < lines->n; i++)
          {
             Vec2f a = Vec2f(lines->arr[i].v1.x, lines->arr[i].v1.y);
             Vec2f b = Vec2f(lines->arr[i].v2.x, lines->arr[i].v2.y);
             
             
             //DrawVec2f::line(buffer, a, b, 0);
             //textprintf_ex(buffer, font, DrawVec2f::scrx((a.x + b.x) * 0.5),
                                         //DrawVec2f::scry((a.y + b.y) * 0.5), makecol(255, 255, 255), -1, "%d", i);
             
          }
          
          DrawVec2f::rect(buffer, min, max, 0);
          
          
          DrawVec2f::circlefill(buffer, pos, 3, 0);
          DrawVec2f::line(buffer, pos, pos + dir, 0);
     }
};

#endif
