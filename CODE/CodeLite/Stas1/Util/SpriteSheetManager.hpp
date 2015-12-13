/* 
 * File:   SpriteSheetManager.hpp
 * Author: Stas
 *
 * Created on June 19, 2012, 5:38 AM
 */

#ifndef SPRITESHEETMANAGER_HPP
#define	SPRITESHEETMANAGER_HPP

#include "SpriteSheet.hpp"
#include "ResourceManager.hpp"

class SpriteSheetManager {
    static ResourceManager<SpriteSheet>* instance;
    
public:
    static ResourceManager<SpriteSheet>& getInstance()
    {
        if(!SpriteSheetManager::instance)
            SpriteSheetManager::instance = new ResourceManager<SpriteSheet>("SpriteSheetManager");
        return *SpriteSheetManager::instance;
    }
    
    static void destroyInstance()
    {
        delete SpriteSheetManager::instance;
        SpriteSheetManager::instance = NULL;
    }
};

ResourceManager<SpriteSheet>* SpriteSheetManager::instance = NULL;

#endif	/* SPRITESHEETMANAGER_HPP */

