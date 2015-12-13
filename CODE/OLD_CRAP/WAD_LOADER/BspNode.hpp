#ifndef _BSPNODE_HPP_
#define _BSPNODE_HPP_

#include <list>

#include "Vec2f.hpp"
#include "DrawVec2f.hpp"

#include "Clipper.hpp"
#include "Wad.hpp"

typedef std::list<Poly> PolyList;
typedef std::list<Poly>::iterator PolyIter;

struct BspNode {
    Vec2f a, b;
    bool negSide, posSide;
    BspNode *negChild, *posChild;
    std::list<Wad::Linedef>* linedefList;
    
    int n;
    
    enum Side { NEG, POS, BOTH, COPLANAR };
    
    BspNode()
    {
        linedefList = NULL;
        negChild = NULL;
        posChild = NULL;
    }
    
    BspNode(Wad::Linedef const& linedef)
    {
        linedefList = NULL;
        negChild = NULL;
        posChild = NULL;
        
        a = linedef.v1;
        b = linedef.v2;

        negSide = linedef.negSidedef != NULL;
        posSide = linedef.posSidedef != NULL;
    }
    
    BspNode(Vec2f const& a, Vec2f const& b)
    {
        linedefList = NULL;
        negChild = NULL;
        posChild = NULL;
        
        this->a = a;
        this->b = b;
    }
    
    ~BspNode()
    {
        delete linedefList;
        delete negChild;
        delete posChild;
    }
    
    void copyProperties(BspNode const& other)
    {
        negSide = other.negSide;
        posSide = other.posSide;
        n = other.n;
    }
    
    static Side split(Vec2f& ip, Vec2f const& s1, Vec2f const& s2,
                                 Vec2f const& p1, Vec2f const& p2)
    {
        Vec2f n = (p1 - p2).normal();
        float d1 = (s1 - p1).dot(n);
        float d2 = (s2 - p1).dot(n);
        
        if(fabs(d1) < 0.001 && fabs(d2) < 0.001)
           return COPLANAR;
        else if(d1 <= 0 && d2 <= 0)
           return NEG;
        else if(d1 >= 0 && d2 >= 0)
           return POS;
        else
        {
            float k = fabs(d1) / (fabs(d1) + fabs(d2));
            ip = s1 + (s2 - s1) * k;
            return BOTH;
        }
    }
                        
    // Classify this node against the other:         
    Side classify(Vec2f& ip, BspNode* other) const
    {
         return split(ip, a, b, other->a, other->b);
    }
    
    // Classify a point against this node:
    Side classifyPoint(Vec2f const& p) const
    {
        Vec2f n = (a - b).unitNormal();
        float d1 = (p - a).dot(n);
        
        if(d1 < 0)
          return NEG;
        else
          return POS;
    }
    
    Side facingSide(Vec2f const& v)
    {
        Vec2f n = (a - b).normal();
        return n.dot(v) < 0 ? NEG : POS;
    }
    
    void linedefListPush(Wad::Linedef const& linedef)
    {
         if(!linedefList)
            linedefList = new std::list<Wad::Linedef>();
         linedefList->push_back(linedef);
    }
    
    void addLinedef(Wad::Linedef const& linedef)
    {
         Vec2f ip; 
         Side side = split(ip, linedef.v1, linedef.v2, a, b);
         
         if(side == NEG)
         {
              if(negChild == NULL) linedefListPush(linedef);
              else                 negChild->addLinedef(linedef);
         }

         else if(side == POS || side == COPLANAR)
         {
              if(posChild == NULL) linedefListPush(linedef);
              else                 posChild->addLinedef(linedef);
         }
         
         else if(side == BOTH)
         {
              Wad::Linedef negLinedef, posLinedef;
              
              if(classifyPoint(linedef.v1) == POS)
              {
                   posLinedef.v1 = linedef.v1;
                   posLinedef.v2 = ip;
                   
                   negLinedef.v1 = ip;
                   negLinedef.v2 = linedef.v2;
              }
              
              else
              {
                   posLinedef.v1 = ip;
                   posLinedef.v2 = linedef.v2;
                   
                   negLinedef.v1 = linedef.v1;
                   negLinedef.v2 = ip;
              }
              
              if(negChild == NULL) linedefListPush(negLinedef);
              else                 negChild->addLinedef(negLinedef);
              
              if(posChild == NULL) linedefListPush(posLinedef);
              else                 posChild->addLinedef(posLinedef);
         }
    }
    
    void addChild(BspNode* node)
    {
         Vec2f ip; 
         Side side = node->classify(ip, this);
         
         if(side == NEG)
         {
              if(negChild == NULL) negChild = node;
              else                 negChild->addChild(node);
         }

         else if(side == POS)
         {
              if(posChild == NULL) posChild = node;
              else                 posChild->addChild(node);
         }
         
         else if(side == BOTH)
         {
              BspNode *negNode, *posNode;
              
              if(classifyPoint(node->a) == POS)
              {
                   posNode = node;
                   negNode = new BspNode(ip, node->b);
                   negNode->copyProperties(*node);
              }
              
              else
              {
                   negNode = node;
                   posNode = new BspNode(ip, node->b);
                   posNode->copyProperties(*node);
              }
              
              node->b = ip;
              
              if(negChild == NULL) negChild = negNode;
              else                 negChild->addChild(negNode);
              
              if(posChild == NULL) posChild = posNode;
              else                 posChild->addChild(posNode);
          
          

         }
    }
    
    void calcLeaves(PolyList& polyList, Clipper& clipper)
    {
         if(posChild)
         {
           clipper.pushState();
           clipper.clip(a, b, 1);
           posChild->calcLeaves(polyList, clipper);
           clipper.popState();
         }
         
         /*
         else
         {
            clipper.pushState();
            clipper.clip(a, b, 1);
            polyList.push_back(clipper.popPoly());
            clipper.popState();
         }*/
         
         if(negChild)
         {
           clipper.pushState();
           clipper.clip(a, b, -1);
           negChild->calcLeaves(polyList, clipper);
           clipper.popState();
         }
         
         if(!posChild || !negChild)
         {
            if(posSide && !posChild)
            {
               clipper.pushState();
               clipper.clip(a, b, 1);
               polyList.push_back(clipper.popPoly());
               clipper.popState();
            }
            
            if(negSide && !negChild)
            {
               clipper.pushState();
               clipper.clip(a, b, -1);
               polyList.push_back(clipper.popPoly());
               clipper.popState();
            }
         }
    }
    
    void draw(BITMAP* buffer, Vec2f pos, Vec2f dir)
    {
         //DrawVec2f::line(buffer, a, b, makecol(255, 0, 0));
         //DrawVec2f::circlefill(buffer, a, 3, makecol(255, 0, 0));
         //DrawVec2f::circlefill(buffer, b, 3, makecol(255, 0, 0));
         
         if(linedefList)
         {
             for(std::list<Wad::Linedef>::iterator i = linedefList->begin(); i != linedefList->end(); ++i)
             {
                DrawVec2f::line(buffer, i->v1, i->v2, 0);
                DrawVec2f::circlefill(buffer, i->v1, 1, 0);
                DrawVec2f::circlefill(buffer, i->v2, 1, 0);
             }
         }
         
         if(posChild) posChild->draw(buffer, pos, dir);
         if(negChild) negChild->draw(buffer, pos, dir);
    }
    
    static Vec2f treeToScreen(int x, int y, int r, int i, int j, int dMax)
    {
        int q = int(pow(2, j) + 0.5),
            d = dMax / q,
            offs = -(q - 1) * d / 2;
            
        return Vec2f(x + i * d + offs, y + j * r * 2.5);
    }
    
    void drawGraph(BITMAP* buffer, int x, int y, int r, int i, int j, int dMax)
    {  
       Vec2f v = treeToScreen(x, y, r, i, j, dMax);
       Vec2f v2 = treeToScreen(x, y, r, i * 2, j + 1, dMax),
             v3 = treeToScreen(x, y, r, i * 2 + 1, j + 1, dMax);
             
       if(negChild)
        line(buffer, int(v.x), int(v.y), int(v2.x), int(v2.y), 0);
       if(posChild)
        line(buffer, int(v.x), int(v.y), int(v3.x), int(v3.y), 0);
        
        circlefill(buffer, int(v.x), int(v.y), r, makecol(255, 0, 0));
        textprintf_ex(buffer, font, int(v.x - 3),
                                    int(v.y - 3), 0, -1, "%d", n);
        
        if(negChild)
           negChild->drawGraph(buffer, x, y, r, i * 2,     j + 1, dMax);
        if(posChild)
           posChild->drawGraph(buffer, x, y, r, i * 2 + 1, j + 1, dMax);
    }
    
    int nodeNum()
    {
      int n = 1;
      if(posChild) n += posChild->nodeNum();
      if(negChild) n += negChild->nodeNum();
      return n;
    }
    
    /*
    void draw(BITMAP* buffer, Vec2f pos, Vec2f dir)
    {    
         Side posSide = classifyPoint(pos);
         Side dirSide = facingSide(dir);
         
         if(posSide == NEG)
         {
             if(positive) positive->draw(buffer, pos, dir);
             
             n++;
             line(buffer, int(SCREEN_W / 2 + a.x / 2), int(SCREEN_H / 2 + a.y / 2), 
                          int(SCREEN_W / 2 + b.x / 2), int(SCREEN_H / 2 + b.y / 2), 0);
             //textprintf_ex(buffer, font, int(SCREEN_W / 2 + a.x / 2),
             //                            int(SCREEN_H / 2 + a.y / 2 - 8), makecol(255, 255, 255), -1, "%d", n);
             
             if(negative) negative->draw(buffer, pos, dir);
         }
         
         else
         {
             if(negative) negative->draw(buffer, pos, dir);
             
             n++;
             line(buffer, int(SCREEN_W / 2 + a.x / 2), int(SCREEN_H / 2 + a.y / 2), 
                          int(SCREEN_W / 2 + b.x / 2), int(SCREEN_H / 2 + b.y / 2), 0);
             //textprintf_ex(buffer, font, int(SCREEN_W / 2 + a.x / 2),
             //                            int(SCREEN_H / 2 + a.y / 2 - 8), makecol(255, 255, 255), -1, "%d", n);
             
             if(positive) positive->draw(buffer, pos, dir);
         }
    }*/
};

#endif
