/* 
 * File:   OpenableContainer.hpp
 * Author: Stas
 *
 * Created on February 8, 2012, 2:03 PM
 */

#ifndef OPENABLECONTAINER_HPP
#define	OPENABLECONTAINER_HPP

#include "Container.hpp"
#include "ResourceMap.hpp"
#include "SoundEngine/AudioEngine.hpp"
#include "Shared.hpp"

class OpenableContainer : public Container {
public:

    enum Status {
        OK, LOCKED, STUCK
    };
    
private:
    Status status;
    std::map<std::string, std::vector<Audio::Signal::Handle> > soundResMap;
    
    void playSound(std::string const& key)
    {
        std::vector<Audio::Signal::Handle>& vec = soundResMap[key];
        
        if(vec.size() > 0)
        {
            int i = vec.size() * (float(rand()) / float(RAND_MAX + 1.0));
            Shared::getAudioEngine()->play(vec[i], Audio::AMBIENT_QUALITY);
        }
    }
    
public:

    OpenableContainer(int maxItems = 10, float maxRad = 0, bool open = false, Status status = OK) : Container(maxItems, maxRad, open, open)
    {
        this->status = status;
    }

    static ComponentId getClassId()
    {
        return OPENABLE_CONTAINER_COMP;
    }

    ComponentId getId()
    {
        return OpenableContainer::getClassId();
    }

    // *******************
    // * Getters\Setters *
    // *******************
    
    void setSoundResMap(ResourceMapHandle resMap)
    {
        soundResMap.clear();
        
        for(ResourceMap::iterator i = resMap.begin(); i != resMap.end(); ++i)
            for(int j = 0; j < i->second.size(); j++)
                soundResMap[i->first].push_back(Shared::getAudioResMan()->get(i->second[j], false));
    }
    
    Status open()
    {
        if(status == OK)
        {
            this->setSearchEnabled(true);
            this->setPutEnabled(true);
            playSound("open");
        }
        
        else if(status == LOCKED)
            playSound("locked");
        else
            playSound("stuck");
        
        return status;
    }
    
    Status close()
    {
        if(status != STUCK)
        {
            this->setSearchEnabled(false);
            this->setPutEnabled(false);
            playSound("close");
        }
        
        else
            playSound("stuck");
        
        return status;
    }

    void setStatus(Status status)
    {
        this->status = status;
    }

    Status getStatus() const
    {
        return status;
    }
};

#endif	/* OPENABLECONTAINER_HPP */
