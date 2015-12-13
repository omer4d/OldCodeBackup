/* 
 * File:   Shared.hpp
 * Author: Stas
 *
 * Created on February 7, 2012, 1:16 AM
 */

#ifndef SHARED_HPP
#define	SHARED_HPP

#include "SoundEngine/AudioEngine.hpp"
#include "Parser.hpp"
#include "ResourceMap.hpp"

class Shared {
    static Shared* instance;

    Audio::EngineManager* audioEngine;
    Audio::ResourceManager* audioResMan;
    ResourceManager<ResourceMapHandle>* resMapMan;
    
    Parser* parser;
    
    Shared(int buffLen)
    {
        audioEngine = new Audio::EngineManager(buffLen);
        audioResMan = new Audio::ResourceManager();
        resMapMan = new ResourceManager<ResourceMapHandle>("ResourceMapManager");
        parser = new Parser();
    }

public:

    ~Shared()
    {
        delete parser;
        delete resMapMan;
        delete audioResMan;
        delete audioEngine;
        instance = NULL;
    }
    
    static void init(int buffLen)
    {
        delete Shared::instance;
        Shared::instance = new Shared(buffLen);
    }
    
    static ResourceManager<ResourceMapHandle>* getResMapMan()
    {
        return Shared::instance ? Shared::instance->resMapMan : NULL;
    }
    
    static Audio::EngineManager* getAudioEngine()
    {
        return Shared::instance ? Shared::instance->audioEngine : NULL;
    }
    
    static Audio::ResourceManager* getAudioResMan()
    {
        return Shared::instance ? Shared::instance->audioResMan : NULL;
    }
    
    static Parser* getParser()
    {
        return Shared::instance ? Shared::instance->parser : NULL;
    }
    
    static Shared* getInstance()
    {
        return Shared::instance;
    }
};

Shared* Shared::instance = NULL;


#endif	/* SHARED_HPP */

