/* 
 * File:   SpriteManager.hpp
 * Author: Stas
 *
 * Created on July 1, 2012, 3:04 PM
 */

#ifndef SPRITEMANAGER_HPP
#define	SPRITEMANAGER_HPP

#include "SpriteData.hpp"
#include "ResourceManager.hpp"

class SpriteManager {
    static ResourceManager<SpriteData>* instance;
    
public:
    static ResourceManager<SpriteData>& getInstance()
    {
        if(!SpriteManager::instance)
            SpriteManager::instance = new ResourceManager<SpriteData>("SpriteManager");
        return *SpriteManager::instance;
    }
    
    static void destroyInstance()
    {
        delete SpriteManager::instance;
        SpriteManager::instance = NULL;
    }
};

ResourceManager<SpriteData>* SpriteManager::instance = NULL;

#endif	/* SPRITEMANAGER_HPP */

