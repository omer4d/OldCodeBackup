/* 
 * File:   SoundComponent.hpp
 * Author: Stas
 *
 * Created on February 10, 2012, 11:16 PM
 */

#ifndef SOUNDCOMPONENT_HPP
#define	SOUNDCOMPONENT_HPP

#include "Component.hpp"
#include "Shared.hpp"
#include "SoundEngine/AudioEngine.hpp"
#include "Event.hpp"
#include "Proxy.hpp"

#include <luabind/luabind.hpp>
using namespace luabind;

typedef std::vector<Audio::Signal::Handle> SoundHandleVec;

class SoundComponent : public Component, public EventListener<Audio::SourceEvent> {
    typedef Proxy<EventListener<Audio::SourceEvent> > ListenerProxy;
    
    std::map<std::string, SoundHandleVec> soundResMap;
    int mySoundId;

public:
    SoundComponent()
    {
        this->mySoundId = (int) & this->mySoundId;
    }

    static ComponentId getClassId()
    {
        return SOUND_COMP;
    }

    ComponentId getId()
    {
        return SoundComponent::getClassId();
    }

    SoundHandleVec const& getSoundHandleVec(std::string const& key)
    {
        return soundResMap[key];
    }
    
    void resetSoundResMap()
    {
        soundResMap.clear();
    }

    void loadSoundResMap(char const* path)
    {
        ResourceMapHandle resMap = Shared::getResMapMan()->get(path);

        for(ResourceMap::iterator i = resMap.begin(); i != resMap.end(); ++i)
            for(int j = 0; j < i->second.size(); j++)
                soundResMap[i->first].push_back(Shared::getAudioResMan()->get(i->second[j], false));
    }
    
    // ****************
    // * Play methods *
    // ****************

    Audio::Item::Handle play(int id, Audio::Signal::Handle h, Audio::Quality quality = Audio::FULL_QUALITY,
                             double intens = 0.5, bool looped = false, Vec3f const& pos = Vec3f())
    {
        Audio::Item::Handle item = Shared::getAudioEngine()->play(h, id, quality, intens, looped, pos);
        item.addEventListener(getProxy());
        return item;
    }

    Audio::Item::Handle play(Audio::Signal::Handle h, Audio::Quality quality = Audio::FULL_QUALITY,
                             double intens = 0.5, bool looped = false, Vec3f const& pos = Vec3f())
    {
        Audio::Item::Handle item = Shared::getAudioEngine()->play(h, mySoundId, quality, intens, looped, pos);
        item.addEventListener(getProxy());
        return item;
    }

    Audio::Item::Handle playRandom(int id, std::string const& key, Audio::Quality quality = Audio::FULL_QUALITY,
                                   double intens = 0.5, bool looped = false, Vec3f const& pos = Vec3f())
    {
        SoundHandleVec const& vec = soundResMap[key];

        if(vec.size() > 0)
        {
            int i = vec.size() * (float(rand()) / float(RAND_MAX + 1.0));
            Audio::Item::Handle item = Shared::getAudioEngine()->play(vec[i], id, quality, intens, looped, pos);
            item.addEventListener(getProxy());
            return item;
        }
    }

    Audio::Item::Handle playRandom(std::string const& key, Audio::Quality quality = Audio::FULL_QUALITY,
                                   double intens = 0.5, bool looped = false, Vec3f const& pos = Vec3f())
    {
        SoundHandleVec const& vec = soundResMap[key];

        if(vec.size() > 0)
        {
            int i = vec.size() * (float(rand()) / float(RAND_MAX + 1.0));
            Audio::Item::Handle item = Shared::getAudioEngine()->play(vec[i], mySoundId,
                                                                      quality, intens, looped, pos);
            item.addEventListener(getProxy());
            return item;
        }
    }
    
    void addEventListener(ListenerProxy p, int id)
    {
        Shared::getAudioEngine()->addEventListener(p, id);
    }
    
    void removeEventListener(ListenerProxy p, int id)
    {
        Shared::getAudioEngine()->removeEventListener(p, id);
    }

    bool onEvent(Audio::SourceEvent& event)
    {
        if(event.type() == Audio::SourceEvent::UPDATE)
        {
            event.pos() = getOwner()->getAbsPos();
        }

        return true;
    }
    
    Proxy<SoundComponent> getProxy()
    {
        return Proxy<SoundComponent>(this, getAlive());
    }
    
    // *****************
    // * LUA functions *
    // *****************

    static SoundComponent* create()
    {
        return new SoundComponent();
    }

    static void luaBind(lua_State* luaState)
    {
        luabind::module(luaState)
                [
                class_<SoundComponent, Component > ("SoundComponent")
                .def("loadSoundResMap", &SoundComponent::loadSoundResMap)
                .def("resetSoundResMap", &SoundComponent::resetSoundResMap)
                .scope
                [
                        def("create", &create)
                ]
                ];
    }
};

#endif	/* SOUNDCOMPONENT_HPP */

