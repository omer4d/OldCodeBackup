/* 
 * File:   CompositeSpriteElement.hpp
 * Author: Stas
 *
 * Created on July 2, 2012, 2:16 AM
 */

#ifndef COMPOSITESPRITEELEMENT_HPP
#define	COMPOSITESPRITEELEMENT_HPP

#include <json/json.h>
#include <list>
#include <string>

#include "Vec2f.hpp"
#include "Mat9f.hpp"
#include "SpriteData.hpp"
#include "SpriteManager.hpp"

class CompositeSpriteElement {
    typedef std::list<CompositeSpriteElement*> ChildList;
    typedef ChildList::iterator ChildIter;
    typedef ChildList::const_iterator ConstChildIter;

    CompositeSpriteElement* parent;
    
    SpriteData const* sprite;
    SpriteClip clip;
    ChildList childList;
    std::string name;
    
    std::string src, dest;
    float ang;
    
    static bool validate(Json::Value const& jval)
    {
        return jval.isMember("name") && jval["name"].isString() &&
               jval.isMember("sprite") && jval["sprite"].isString() &&
               jval.isMember("anim") && jval["anim"].isString() &&
               jval.isMember("src") && jval["src"].isString() &&
               jval.isMember("dest") && jval["dest"].isString() &&
               jval.isMember("ang") && jval["ang"].isNumeric();
    }
    
    static CompositeSpriteElement* create(char const* modelPath, Json::Value const& jval)
    {
        if(!CompositeSpriteElement::validate(jval))
            return NULL;
        
        std::string src, dest;
        float ang;
        char const* name = jval["name"].asCString();
        char const* spritePath = jval["sprite"].asCString();
        char buffer[500];
        replace_filename(buffer, modelPath, spritePath, 500);
        
        SpriteData const* sprite = &SpriteManager::getInstance().get(buffer);
        char const* anim = jval["anim"].asCString();

        src = jval["src"].asString();
        dest = jval["dest"].asString();
        ang = jval["ang"].asFloat();
        
        CompositeSpriteElement* elem = new CompositeSpriteElement(name, sprite, anim, src, dest, ang);
        
        Json::Value const& children = jval["children"];
        
        for(int i = 0; i < children.size(); i++)
        {
            CompositeSpriteElement* temp = create(modelPath, children[i]);
            
            if(temp)
                elem->addChild(temp);
        }
            
        return elem;
    }

    CompositeSpriteElement(SpriteClip const& pClip): clip(pClip)
    {
        this->parent = NULL;
        this->sprite = NULL;
    }

    bool transform(Mat9f& mat) const
    {
        Pivot const* pivot = NULL;
        
        if(parent)
        {
            pivot = parent->clip.getCurrFrame().getPivot(dest);

            if(!pivot)
                return false;
            
            Vec2f p = pivot->getPos();
            mat.translate(p.x, p.y);
        }

        mat.rotate(ang);

        pivot = clip.getCurrFrame().getPivot(src);

        if(!pivot)
            return false;

        Vec2f p = pivot->getPos();
        mat.translate(-p.x, -p.y);
        
        return true;
    }
    
    bool inverseTransform(Mat9f& mat) const
    {
        Pivot const* pivot = clip.getCurrFrame().getPivot(src);
        
        if(!pivot)
            return false;

        Vec2f p = pivot->getPos();
        mat.translate(p.x, p.y);

        mat.rotate(-ang);

        if(parent)
        {
            pivot = parent->clip.getCurrFrame().getPivot(dest);

            if(!pivot)
                return false;
            
            Vec2f p = pivot->getPos();
            mat.translate(-p.x, -p.y);
        }
        
        return true;
    }
    
public:
    
    CompositeSpriteElement(char const* name, SpriteData const* sprite, char const* anim,
                           std::string const& src, std::string const& dest, float ang) : clip(sprite->getClip(anim))
    {
        this->parent = NULL;
        this->name = name;
        this->sprite = sprite;
        this->src = src;
        this->dest = dest;
        this->ang = ang;
    }
         
    ~CompositeSpriteElement()
    {
        for(CompositeSpriteElement::ChildIter i = childList.begin(); i != childList.end(); ++i)
            delete *i;
    }
    
    void addChild(CompositeSpriteElement* elem)
    {
        elem->parent = this;
        childList.push_back(elem);
    }

    void play(char const* anim)
    {
        clip = sprite->getClip(anim);
    }

    SpriteClip const& getClip()
    {
        return clip;
    }
    
    CompositeSpriteElement* deepCopy() const
    {
        CompositeSpriteElement* elem = new CompositeSpriteElement(clip);
        elem->name = name;
        elem->sprite = sprite;
        elem->src = src;
        elem->dest = dest;
        elem->ang = ang;
        
        for(ConstChildIter i = childList.begin(); i != childList.end(); ++i)
            elem->addChild((*i)->deepCopy());
        
        return elem;
    }

    void tick(float dt)
    {
        clip.tick(dt);
        
        for(CompositeSpriteElement::ChildIter i = childList.begin(); i != childList.end(); ++i)
            (*i)->tick(dt);
    }
    
    void setAng(float ang)
    {
        this->ang = ang;
    }
    
    float getAng() const
    {
        return ang;
    }
    
    void rotate(float d)
    {
        ang += d;
    }
    
    void render(Mat9f mat) const
    {
        if(!transform(mat))
            return;
        
        for(CompositeSpriteElement::ConstChildIter i = childList.begin(); i != childList.end(); ++i)
            (*i)->render(mat);
        
        clip.render(mat);
    }
    
    void debug() const
    {
        printf("%s - %s %s {\n", name.c_str(), src.c_str(), dest.c_str());
        
        for(CompositeSpriteElement::ConstChildIter i = childList.begin(); i != childList.end(); ++i)
            (*i)->debug();
        
        printf("}\n");
    }
    
    Vec2f getPos(Mat9f mat, char const* pname)
    {
        Mat9f inv;
        CompositeSpriteElement* elem = this;
        
        Pivot const* pivot = clip.getCurrFrame().getPivot(pname);
        
        if(!pivot)
            return Vec2f();

        Vec2f p = pivot->getPos();
        
        while(elem)
        {
            elem->inverseTransform(inv);
            elem = elem->parent;
        }
        
        mat *= inv.invert();
        
        return p * mat;
    }
    
    /*
    void render(Mat9f mat) const
    {
        mat.translate(dest.x, dest.y);
        mat.rotate(ang);
        mat.translate(-src.x, -src.y);
        
        for(CompositeSpriteElement::ConstChildIter i = childList.begin(); i != childList.end(); ++i)
            (*i)->render(mat);
        
        clip.render(mat);
    }*/
    
    friend class CompositeSprite;
};

#endif	/* COMPOSITESPRITEELEMENT_HPP */

