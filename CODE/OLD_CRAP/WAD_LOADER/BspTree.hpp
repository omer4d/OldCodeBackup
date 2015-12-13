#ifndef _BSPTREE_HPP_
#define _BSPTREE_HPP_

#include <vector>
#include <list>

#include "Vec2f.hpp"

#include "BspNode.hpp"

struct SplitCount {
    int i, n;
    
    static bool cmp(SplitCount const& a, SplitCount const& b) { return (a.n < b.n ); }
};

struct BspTree {
     BspNode* root;
     PolyList polyList;
     Vec2f min, max;
     Wad::Lump<Wad::Linedef>* lines;
     
     int split(Wad::Linedef const& seg, Wad::Linedef const& plane)
     {
        Vec2f const &s1 = seg.v1, &s2 = seg.v2;
        Vec2f const &p1 = plane.v1, &p2 = plane.v2;
        
        Vec2f n = (p1 - p2).normal();
        float d1 = (s1 - p1).dot(n);
        float d2 = (s2 - p1).dot(n);
        
        if(d1 * d2 < 0)
           return 1;
        else
           return 0;
     }
     
     int splitCount(Wad::Lump<Wad::Linedef>& lineList, Wad::Linedef const& plane)
     {
         int c = 0;
         
         for(int i = 0; i < lineList.n; i++)
         {
            c += split(lineList.arr[i], plane);
         }
         
         return c;
     }
     
     void build(Wad& wad)
     {
         Wad::Lump<Wad::Linedef>& linedefLump = *wad.linedefLump;
         Wad::Lump<Wad::Vertex>& vertexLump = *wad.vertexLump;
         lines = wad.linedefLump;
         
         std::vector<SplitCount> order;
         order.reserve(linedefLump.n);
         
         for(int i = 0; i < linedefLump.n; i++)
         {
            SplitCount sc;
            sc.i = i;
            sc.n = splitCount(linedefLump, linedefLump.arr[i]);
            order.push_back(sc);
         }
         
         sort(order.begin(), order.end(), SplitCount::cmp);
         
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
         
         root = new BspNode(linedefLump.arr[order[0].i]);
         root->n = 0;
         
         for(int i = 1; i < linedefLump.n; i++)
         {
             Vec2f ip;
             BspNode* node = new BspNode(linedefLump.arr[order[i].i]);
             node->n = i;
             root->addChild(node);
         }
         
         for(int i = 0; i < linedefLump.n; i++)
             root->addLinedef(linedefLump.arr[i]);
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
          
          
          /*
          for(int i = 0; i < lines->n; i++)
          {
             Vec2f a = Vec2f(lines->arr[i].v1.x, lines->arr[i].v1.y);
             Vec2f b = Vec2f(lines->arr[i].v2.x, lines->arr[i].v2.y);
             Vec2f mid = (a + b) * 0.5;
             Vec2f n = (a - b).unitNormal() * 10;
             
             DrawVec2f::line(buffer, a, b, 0);
             
             
             
             //textprintf_ex(buffer, font, DrawVec2f::scrx((a.x + b.x) * 0.5),
             //                            DrawVec2f::scry((a.y + b.y) * 0.5), makecol(255, 255, 255), -1, "%d", i);
             
             //DrawVec2f::line(buffer, mid, mid + n, makecol(0, 0, 255));
          }*/
          
          //root->draw(buffer, Vec2f(), Vec2f());
          
          DrawVec2f::rect(buffer, min, max, 0);
          
          
          DrawVec2f::circlefill(buffer, pos, 3, 0);
          DrawVec2f::line(buffer, pos, pos + dir, 0);
     }
     
     void drawGraph(BITMAP* buffer, int x, int y, int r, int dMax)
     {
         root->drawGraph(buffer, x, y, r, 0, 0, dMax);
     }
};

#endif
