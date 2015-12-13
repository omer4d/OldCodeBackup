/* 
 * File:   SpriteData.hpp
 * Author: Stas
 *
 * Created on June 19, 2012, 5:40 AM
 */

#ifndef SPRITEDATA_HPP
#define	SPRITEDATA_HPP

#include <allegro.h>

#include <map>
#include <json/json.h>
#include <stdexcept>
#include <fstream>
#include <string>
#include <SpriteSheetManager.hpp>

#include "Animation.hpp"
#include "SpriteSheet.hpp"
#include "SpriteClip.hpp"

class SpriteData {
    typedef std::map<std::string, Animation> Map;
    Map map;
    
    // Full info + correct types?
    bool isFullAnim(Json::Value const& element)
    {
        return element.isMember("name") && element["name"].isString() &&
               element.isMember("spriteSheet") && element["spriteSheet"].isString() &&
               element.isMember("firstFrame") && element["firstFrame"].isInt() &&
               element.isMember("frameNum") && element["frameNum"].isInt() &&
               element.isMember("fps") && element["fps"].isNumeric();
    }
    
    // All necessary fields + correct types?
    bool isValidAnim(Json::Value const& element)
    {
        return element.isMember("name") && element["name"].isString() &&
               (!element.isMember("spriteSheet") || element["spriteSheet"].isString()) &&
               (!element.isMember("firstFrame") || element["firstFrame"].isInt()) &&
               (!element.isMember("frameNum") || element["frameNum"].isInt()) &&
               (!element.isMember("fps") || element["fps"].isNumeric());
    }

    void throwParseException(char const* path, char const* reason)
    {
        throw std::runtime_error(std::string("Could not parse sprite file: '") +
                                 path + "(" + reason + ")");
    }
    
public:
    
    SpriteData()
    {
        ResourceManager<SpriteSheet>& resMan = SpriteSheetManager::getInstance();
        
        Animation temp;
        temp.firstFrame = 0;
        temp.fps = 0;
        temp.frameNum = 1;
        temp.spriteSheet = &resMan.get("");
        map[""] = temp;
    }
    
    SpriteData(char const* path)
    {
        ResourceManager<SpriteSheet>& resMan = SpriteSheetManager::getInstance();
        
        std::ifstream file;
        file.open(path);

        if(!file.is_open())
        {
            throw std::runtime_error(std::string("Sprite not found: '") + path + "'");
        }

        Json::Value root;
        Json::Reader reader;

        if(!reader.parse(file, root))
        {
            throwParseException(path, reader.getFormattedErrorMessages().c_str());
        }
        
        file.close();
        
        if(!root.isMember("animations"))
        {
            throwParseException(path, "'animations' key is missing.");
        }
        
        const Json::Value animArr = root["animations"];
        Animation const* lastAnim = NULL;
        std::string lastSpriteSheetPath;
        char buffer[500];
        
        for(int i = 0; i < animArr.size(); i++)
        {
            const Json::Value element = animArr[i];
            Animation temp;
            std::string spriteSheetPath;
            bool appearsValid = false;
            
            // Full info.
            if(isFullAnim(element))
            {
                spriteSheetPath = element["spriteSheet"].asString();
                temp.firstFrame = element["firstFrame"].asInt();
                temp.frameNum = element["frameNum"].asInt();
                temp.fps = element["fps"].asFloat();
                appearsValid = true;
            }
            
            // Partial info but correct types. Take missing bits from previous valid animation.
            else if(lastAnim && isValidAnim(element))
            {
                spriteSheetPath = element.get("spriteSheet", lastSpriteSheetPath).asString();
                temp.firstFrame = element.get("firstFrame", lastAnim->firstFrame + lastAnim->frameNum).asInt();
                temp.frameNum = element.get("frameNum", lastAnim->frameNum).asInt();
                temp.fps = element.get("fps", lastAnim->fps).asFloat();
                appearsValid = true;
            }
            
            else
                appearsValid = false;
            
            if(appearsValid)
            {
                replace_filename(buffer, path, spriteSheetPath.c_str(), 500);
                temp.spriteSheet = &resMan.get(buffer);
                
                if(temp.isValid())
                {
                    map[element["name"].asString()] = temp;
                    lastAnim = &map[element["name"].asString()];
                    lastSpriteSheetPath = spriteSheetPath;
                }
                
                else
                    printf("Bad info in animation %d\n", i);
            }
            
            else
                printf("Wrong types or missing necessary fields in animation %d\n", i);
        }
        
        if(!lastAnim) // Not a single valid animations???
            throwParseException(path, "No valid animations!");
        
        // Create a default animation:
        Animation temp;
        
        replace_filename(buffer, path, lastSpriteSheetPath.c_str(), 500);
        temp.spriteSheet = &resMan.get(buffer);
        temp.firstFrame = 0;
        temp.fps = 0;
        temp.frameNum = 1;
        map[""] = temp;
    }
    
    SpriteClip getClip(std::string name) const
    {
        Map::const_iterator i = map.find(name);
        if(i != map.end())
            return SpriteClip(&i->second);
        else
        {
            i = map.find("");
            return SpriteClip(&i->second);
        }
    }
};

#endif	/* SPRITEDATA_HPP */

