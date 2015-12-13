/* 
 * File:   Sprite.hpp
 * Author: Stas
 *
 * Created on July 6, 2012, 12:25 PM
 */

#ifndef SPRITE_HPP
#define	SPRITE_HPP

#include "SpriteBase.hpp"
#include "SpriteData.hpp"
#include "Mat9f.hpp"

class Sprite: public SpriteBase {
    SpriteData const& spriteData;
    SpriteClip clip;
    
public:
    Sprite(SpriteData const& data, char const* anim): spriteData(data), clip(data.getClip(anim))
    {
    }
    
    void tick(float dt)
    {
        clip.tick(dt);
    }
    
    void render(Mat9f const& mat) const
    {
        clip.render(mat);
    }
    
    SpriteClip const& getClip() const
    {
        return clip;
    }
    
    SpriteClip& getClip()
    {
        return clip;
    }
    
    Vec2f getPos(Mat9f mat, char const* pname)
    {
        Pivot const* pivot = clip.getCurrFrame().getPivot(pname);
        
        if(!pivot)
            return Vec2f();
        
        return pivot->getPos() * mat;
    }
};

#endif	/* SPRITE_HPP */

