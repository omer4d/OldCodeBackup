/* 
 * File:   SpriteClip.hpp
 * Author: Stas
 *
 * Created on June 19, 2012, 5:39 AM
 */

#ifndef SPRITECLIP_HPP
#define	SPRITECLIP_HPP

#include <GL/gl.h>
#include "Mat9f.hpp"

#include "Animation.hpp"
#include "Texture.hpp"
#include "Util.hpp"

class SpriteClip {
    Animation const* anim;
    float frame;
    float speed;
    bool looped;
    
public:
    SpriteClip(Animation const* anim)
    {
        this->anim = anim;
        this->frame = anim->firstFrame;
        this->speed = 1;
        this->looped = true;
    }
    
    GLuint getTexId() const
    {
        return anim->spriteSheet->getTexture().getId();
    }
    
    Frame const& getCurrFrame() const
    {
        return anim->spriteSheet->getFrame(frame);
    }
    
    void tick(float dt)
    {
        frame += dt * anim->fps * speed;
        
        if(looped)
            frame = wrapf(frame, anim->firstFrame, anim->firstFrame + anim->frameNum);
        else
            frame = clamp<float>(frame, anim->firstFrame, anim->firstFrame + anim->frameNum - 1);
    }

    void setLooped(bool looped)
    {
        this->looped = looped;
    }

    bool isLooped() const
    {
        return looped;
    }

    void setSpeed(float speed)
    {
        this->speed = speed;
    }

    float getSpeed() const
    {
        return speed;
    }

    void render(Mat9f const& mat) const
    {
        Frame const& frame = getCurrFrame();
        float h = frame.getHeight();
        float w = frame.getWidth();

        Vec2f const& texMin = frame.getTexMin();
        Vec2f const& texMax = frame.getTexMax();

        Vec2f v[4] = {Vec2f(0, 0), Vec2f(w, 0), Vec2f(w, h), Vec2f(0, h)};
        v[0] *= mat;
        v[1] *= mat;
        v[2] *= mat;
        v[3] *= mat;

        Texture::bindId(getTexId());

        glBegin(GL_QUADS);
           glTexCoord2f(texMin.x, texMin.y); glVertex2f(v[0].x, v[0].y);
           glTexCoord2f(texMax.x, texMin.y); glVertex2f(v[1].x, v[1].y);
           glTexCoord2f(texMax.x, texMax.y); glVertex2f(v[2].x, v[2].y);
           glTexCoord2f(texMin.x, texMax.y); glVertex2f(v[3].x, v[3].y);
        glEnd();
    }
};

#endif	/* SPRITECLIP_HPP */

