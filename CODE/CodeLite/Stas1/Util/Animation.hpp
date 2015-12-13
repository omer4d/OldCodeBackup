/* 
 * File:   Animation.hpp
 * Author: Stas
 *
 * Created on June 19, 2012, 5:39 AM
 */

#ifndef ANIMATION_HPP
#define	ANIMATION_HPP

#include "SpriteSheet.hpp"

struct Animation {
    SpriteSheet const* spriteSheet;
    int firstFrame, frameNum;
    float fps;
    
    Animation()
    {
        spriteSheet = NULL;
        firstFrame = -1;
        frameNum = -1;
        fps = 0;
    }
    
    Animation(SpriteSheet const& spriteSheet, int firstFrame, int frameNum, float fps)
    {
        this->spriteSheet = &spriteSheet;
        this->firstFrame = firstFrame;
        this->frameNum = frameNum;
        this->fps = fps;
    }
    
    bool isValid()
    {
        return firstFrame >= 0 /*&& firstFrame + frameNum < spriteSheet->getFrameNum()*/ && fps >= 0;
    }
};

#endif	/* ANIMATION_HPP */

