#ifndef _BSPNODE_HPP_
#define _BSPNODE_HPP_

#include <list>

#include "Vec2f.hpp"

#include "Clipper.hpp"
#include "Wad.hpp"

typedef std::list<Poly> PolyList;
typedef std::list<Poly>::iterator PolyIter;

struct BspNode {
    Vec2f a, b;
    short flags, specialFlag, sectorTag;
    Wad::Sidedef *negSide, *posSide;
    BspNode *negChild, *posChild;
    
    enum Side { NEG, POS, BOTH, COPLANAR };
    
    BspNode()
    {
        negChild = NULL;
        posChild = NULL;
    }
    
    ~BspNode()
    {
        delete negChild;
        delete posChild;
    }
    
    BspNode(Wad::Linedef const& linedef)
    {
        negChild = NULL;
        posChild = NULL;
        
        a = Vec2f(linedef.v1.x, linedef.v1.y);
        b = Vec2f(linedef.v2.x, linedef.v2.y);
        flags = linedef.flags;
        specialFlag = linedef.specialFlag;
        negSide = linedef.negSidedef;
        posSide = linedef.posSidedef;
    }
    
    BspNode(Vec2f const& a, Vec2f const& b)
    {
        negChild = NULL;
        posChild = NULL;
        
        this->a = a;
        this->b = b;
    }
    
    void copyProperties(BspNode const& other)
    {
        flags = other.flags;
        specialFlag = other.specialFlag;
        negSide = other.negSide;
        posSide = other.posSide;
    }
    
    // Classify this node against the other:
    Side classify(Vec2f& ip, BspNode* other) const
    {
        Vec2f n = (other->a - other->b).normal();
        float d1 = (a - other->a).dot(n);
        float d2 = (b - other->a).dot(n);
        
        if(fabs(d1) < 0.001 && fabs(d2) < 0.001)
           return COPLANAR;
        else if(d1 <= 0 && d2 <= 0)
           return NEG;
        else if(d1 >= 0 && d2 >= 0)
           return POS;
        else
        {
            float k = fabs(d1) / (fabs(d1) + fabs(d2));
            ip = a + (b - a) * k;
            return BOTH;
        }
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
         
         else if(side == COPLANAR)
         {
          /*
              if(negChild == NULL) negChild = node;
              else                 negChild->addChild(node);
              
              if(posChild == NULL) posChild = node;
              else                 posChild->addChild(node);*/
         }
         
         else
         {
              printf("CUT");
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
         DrawVec2f::line(buffer, a, b, 0);
         DrawVec2f::circlefill(buffer, a, 3, 0);
         DrawVec2f::circlefill(buffer, b, 3, 0);
         
         if(posChild) posChild->draw(buffer, pos, dir);
         if(negChild) negChild->draw(buffer, pos, dir);
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
