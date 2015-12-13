/* 
 * File:   SpriteBase.hpp
 * Author: Stas
 *
 * Created on July 6, 2012, 12:30 PM
 */

#ifndef SPRITEBASE_HPP
#define	SPRITEBASE_HPP

class Mat9f;

class SpriteBase {
public:
    
    virtual void tick(float dt) = 0;
    virtual void render(Mat9f const& mat) const = 0;
    virtual ~SpriteBase()
    {
        
    }
};

#endif	/* SPRITEBASE_HPP */

