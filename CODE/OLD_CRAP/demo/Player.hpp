/* 
 * File:   Player.hpp
 * Author: Stas
 *
 * Created on February 3, 2012, 10:01 PM
 */

#ifndef PLAYER_HPP
#define	PLAYER_HPP

#include <list>

#include "Navigator.hpp"
#include "World.hpp"
#include "SoundEngine/AudioEngine.hpp"
#include "Proxy.hpp"

#include "Container.hpp"
#include "OpenableContainer.hpp"
#include "Takable.hpp"

#include "Shared.hpp"

#include "ResourceMap.hpp"

#include "PlayerCommand.hpp"

#include "SoundComponent.hpp"

#include "Event.hpp"

class Player: public Entity, public EventListener<Audio::SourceEvent>,
              public EventListener<TrackFollowerEvent> {
    
    bool walking, reachingItem;
    float timeSinceLastStep;
    
    SoundComponent* soundComponent;
    Container const* worldContainer;
    Container* inventory;
    TrackFollower* trackFollower;
    Navigator* navigator;
    
    Proxy<Entity> targetItem;
    
    bool busy;
    
    Audio::Signal::Handle footstepSound[4];
    int mySoundId, myVoiceId;
    std::list<Audio::Signal::Handle> voiceBuffer;
    
    std::list<PlayerCommand*> commandBuffer;
    
    void _playVoice(Audio::Signal::Handle h)
    {
        Audio::Item::Handle item = soundComponent->play(myVoiceId, h, Audio::AMBIENT_QUALITY, 0.5,
                                                        false, Vec3f());
        item.addEventListener(getProxy());
    }
    
    void playVoice(std::string const& key)
    {
        SoundHandleVec const& vec = soundComponent->getSoundHandleVec(key);
        
        if(vec.size() > 0)
        {
            int i = vec.size() * (float(rand()) / float(RAND_MAX + 1.0));
            
            if(voiceBuffer.empty())
                _playVoice(vec[i]);
            voiceBuffer.push_front(vec[i]);
        }
    }
    
    void onStop(Audio::Signal::Handle const& h, int id)
    {
    }
    
public:
    Player(World* world)
    {
        walking = false;
        reachingItem = false;
        timeSinceLastStep = 1000.0;
        
        this->trackFollower = new TrackFollower();
        this->trackFollower->setSpeed(1.5);
        this->trackFollower->setRotSpeed(M_PI);
        this->trackFollower->events().addListener(proxy());
        this->addComponent(trackFollower);
        
        this->navigator = new Navigator(world);
        this->addComponent(navigator);

        this->soundComponent = new SoundComponent();
        this->soundComponent->loadSoundResMap("resmaps/player.txt");
        this->addComponent(this->soundComponent);
        
        this->inventory = new Container();
        this->inventory->setPutEnabled(true);
        this->inventory->setMaxRad(1.0);
        this->inventory->setMaxItems(10);
        this->addComponent(this->inventory);
        
        this->worldContainer = world->getContainer();
        this->busy = false;
        
        this->mySoundId = (int)&this->mySoundId;
        this->myVoiceId = (int)&this->myVoiceId;
        
        /*
        ResourceMapHandle resMap = Shared::getResMapMan()->get("resmaps/player.txt");
        
        for(ResourceMap::iterator i = resMap.begin(); i != resMap.end(); ++i)
            for(int j = 0; j < i->second.size(); j++)
                soundResMap[i->first].push_back(Shared::getAudioResMan()->get(i->second[j], false));*/
    }
    
    ~Player()
    {
        abortCommands();
    }
    
    void abortCommands()
    {
        busy = false;
        
        while(!commandBuffer.empty())
        {
            delete commandBuffer.back();
            commandBuffer.pop_back();
        }
    }
    
    void executeTopCommand()
    {
        commandBuffer.back()->execute();
        delete commandBuffer.back();
        commandBuffer.pop_back();
    }
    
    void visitNextTarget()
    {
        while(!commandBuffer.empty())
        {
            PlayerCommand* back = commandBuffer.back();
            
            if(back->validate())
            {
                Vec3f dest = back->getTargetPos();
                if(back->isDistant() && (dest - getAbsPos()).squaredLength() > 1.0f)
                {
                    navigator->goTo(Vec2f(dest.x, dest.z), 0.25);
                    busy = true;
                    break;
                }
                
                else
                    executeTopCommand();
            }
            
            else
            {
                delete back;
                commandBuffer.pop_back();
            }
        }
    }
    
    void setFootstepSounds(Audio::Signal::Handle a,
                           Audio::Signal::Handle b,
                           Audio::Signal::Handle c,
                           Audio::Signal::Handle d)
    {
        footstepSound[0] = a;
        footstepSound[1] = b;
        footstepSound[2] = c;
        footstepSound[3] = d;
    }
    
    void setFootstepSounds(char const* path)
    {
        this->soundComponent->loadSoundResMap(path);
    }
    
    Proxy<Player> proxy()
    {
        return Proxy<Player>(this, this->getAlive());
    }
    
    bool onEvent(TrackFollowerEvent& event)
    {
        switch(event.getType())
        {
            case TrackFollowerEvent::TARGET_SET:
                walking = true;
                break;
                
            case TrackFollowerEvent::TARGET_REACHED:
                walking = false;
                if(reachingItem && !targetItem.expired())
                {
                    addToInventory(targetItem.getObject());
                    reachingItem = false;
                }

                if(busy)
                {
                    executeTopCommand();
                    busy = false;
                }
                
                break;
        }
        
        return true;
    }
    
    bool onEvent(Audio::SourceEvent& event)
    {  
        if(event.type() == Audio::SourceEvent::STOP)
        {
            if(event.id() == myVoiceId)
            {
                voiceBuffer.pop_back();
                if(!voiceBuffer.empty())
                    _playVoice(voiceBuffer.back());
            }
        }
        
        return true;
    }

    void addToInventory(Entity* ent)
    {
        switch(inventory->put(ent))
        {
            case Container::PUT_TOO_BIG:
                playVoice("tooLarge");
                break;
            case Container::PUT_IS_FULL:
                playVoice("inventoryFull");
                break;
            default:
                playVoice("take");
                ent->getComponent<Takable > ()->onTake();
                break;
        }
    }
    
    // ************
    // * Commands *
    // ************
    
    class TakeCmd: public PlayerCommand {
        Player* player;
        Proxy<Entity> entProxy;
        
        std::string name;
        StringList descList;
        
    public:

        TakeCmd(Player* player, std::string const& name, StringList const& descList)
        {
            this->player = player;
            this->name = name;
            this->descList = descList;
            setDistant(true);
        }
        
        bool validate()
        {
            Entity* ent = NULL;
            int finv, fworld;
            
            player->inventory->setSearchEnabled(true);
            finv = player->inventory->findSingle(ent, name, descList);
            
            if(finv == 0)
            {
                fworld = player->worldContainer->findSingle(ent, name, descList);

                if(fworld)
                    setTargetPos(ent->getAbsPos());
            }

            player->inventory->setSearchEnabled(false);
            
            if(finv)
            {
                player->playVoice("alreadyHave");
                return false;
            }
            
            switch(fworld)
            {
                case 0:
                    player->playVoice("cantFind");
                    return false;
                    break;

                case 2:
                    player->playVoice("severalTargets");
                    return false;
                    break;
            }
            
            if(ent->getComponent<Takable>() == NULL)
            {
                player->playVoice("wrongAction");
                return false;
            }
            
            this->entProxy = ent->getProxy();
            return true;
        }

        void execute()
        {
            if(!entProxy.expired())
                player->addToInventory(entProxy.getObject());
        }
    };
    
    class GoCmd : public PlayerCommand {
        Player* player;
        Proxy<Entity> entProxy;
        std::string name;
        StringList descList;

    public:

        GoCmd(Player* player, std::string const& name, StringList const& descList)
        {
            this->player = player;
            this->name = name;
            this->descList = descList;
            
            setDistant(true);
        }

        bool validate()
        {
            Entity* ent = NULL;
            int fworld = player->worldContainer->findSingle(ent, name, descList);

            switch(fworld)
            {
                case 0:
                    player->playVoice("cantFind");
                    return false;
                    break;

                case 1:
                    entProxy = ent->getProxy();
                    setTargetPos(ent->getAbsPos());
                    return true;
                    break;

                case 2:
                    player->playVoice("severalTargets");
                    return false;
                    break;
            }

            return true;
        }

        void execute()
        {
            if(!entProxy.expired())
            {
                Vec3f dest = entProxy.getObject()->getAbsPos();
                player->navigator->goTo(Vec2f(dest.x, dest.z), 0.25);
            }
        }
    };  
    
    class DropCmd: public PlayerCommand {
        Player* player;
        Proxy<Entity> entProxy;
        
        std::string name;
        StringList descList;
        
    public:

        DropCmd(Player* player, std::string const& name, StringList const& descList)
        {
            this->player = player;
            this->name = name;
            this->descList = descList;
            
            setDistant(false);
        }

        bool validate()
        {
            int finv;
            Entity* ent;
            
            player->inventory->setSearchEnabled(true);
            finv = player->inventory->findSingle(ent, name, descList);

            player->inventory->setSearchEnabled(false);
            
            switch(finv)
            {
                case 0:
                    player->playVoice("dontHave");
                    return false;
                    break;

                case 2:
                    player->playVoice("haveSeveral");
                    return false;
                    break;
                default:
                    entProxy = ent->getProxy();
                    return true;
                    break;
            }
        }
        
        void execute()
        {
            if(!entProxy.expired())
            {
                Entity* ent = entProxy.getObject();
                player->navigator->getWorld()->addChild(ent, player->getPos());
                ent->getComponent<Takable > ()->onDrop();
            }
        }
    };
    
    // ****************************
    // * Command Public Interface *
    // ****************************

    void take(std::string const& name, StringList const& descList)
    {
        commandBuffer.push_front(new TakeCmd(this, name, descList));
    }
    
    void go(std::string const& name, StringList const& descList)
    {
        commandBuffer.push_front(new GoCmd(this, name, descList));
    }
    
    void drop(std::string const& name, StringList const& descList)
    {
        commandBuffer.push_front(new DropCmd(this, name, descList));
    }
    
    // ********
    // * Etc. *
    // ********
    
    Proxy<Player> getProxy()
    {
        return Proxy<Player>(this, getAlive());
    }
    
    void logic()
    {
        Entity::logic();
        Audio::EngineManager* audioEngine = Shared::getAudioEngine();
        
        if(audioEngine)
            audioEngine->setHeadState(getAbsPos(), trackFollower->getDir(), 0.0);
        
        timeSinceLastStep += Entity::getTimeDelta();
        if(walking && timeSinceLastStep > 0.7)
        {
            timeSinceLastStep = 0.0;
            soundComponent->playRandom("footsteps", Audio::AMBIENT_QUALITY, 0.5);
            //Shared::getAudioEngine()->play(footstepSound[rand() % 4], mySoundId, Audio::AMBIENT_QUALITY, 0.1);
        }

        if(!busy && !commandBuffer.empty())
            visitNextTarget();
    }
};

#endif	/* PLAYER_HPP */

