/* 
 * File:   main.cpp
 * Author: Stas
 *
 * Created on January 23, 2012, 4:05 PM
 */

#include <cstdlib>
#include <allegro.h>

#include "Vec2f.hpp"
#include "Vec3f.hpp"

#include "World.hpp"
#include "Navigator.hpp"

#include "SoundEngine/AudioEngine.hpp"

#include "SafePtr.hpp"

#include "AABB.hpp"
#include "InputField.hpp"

#include "Parser.hpp"
#include "Player.hpp"

#include "Container.hpp"
#include "Identity.hpp"

#include "SoundEngine/ResourceManager.hpp"

#include "Shared.hpp"

#include "SoundComponent.hpp"

#include "LuaVec2f.hpp"
#include "LuaVec3f.hpp"

#define BUFFLEN ((2 << 10) + 1)

BITMAP* buffer;

void init()
{
    allegro_init();

    set_color_depth(32);
    if(set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0))
        allegro_message("Error: %s.", allegro_error);

    if(install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL))
        allegro_message("Error: %s.", allegro_error);

    install_keyboard();
    install_mouse();

    buffer = create_bitmap(SCREEN_W, SCREEN_H);
    srand(time(NULL));
}



Container* worldContainer;

Container* getWorldContainer()
{
    return worldContainer;
}

class Engine {
    Audio::EngineManager* audioEngine;
    Audio::ResourceManager* audioResMan;
    
    World* world;
    Proxy<Player> player;
    
    Parser* parser;
    
    lua_State *luaState;
    
public:
    Engine()
    {
        Shared::init(BUFFLEN);
        
        audioEngine = Shared::getAudioEngine();
        audioResMan = Shared::getAudioResMan();
        parser = Shared::getParser();

        world = NULL;
        
        luaState = lua_open();
        luaL_openlibs(luaState);
        luabind::open(luaState);
        luaBind();
        
        parser->pushAction("go");
        parser->pushAction("take");
        parser->pushAction("drop");
        
        parser->pushDesc("red");
        parser->pushDesc("green");
        parser->pushDesc("blue");
        
        parser->pushKeyword("and");
        parser->pushKeyword("or");
    }
    
    ~Engine()
    {
        lua_close(luaState);
        delete world;
        delete Shared::getInstance();
    }
    
    void loadWorld(const char* path, float worldScale)
    {
        Entity::setTimeDelta(BUFFLEN / 44100.0);

        world = new World("maps/test.map", worldScale);
        worldContainer = world->getContainer();
        
        audioResMan->get("sounds/clock_tick.wav");
        audioResMan->get("sounds/storm.wav", false);
        audioResMan->get("sounds/creep.wav", false);
        
        
        Player* p = new Player(world);
        p->setFootstepSounds("resmaps/footsteps_wood.txt");
        
        world->addChild(p);

        Vec2f start = world->findMark("start"), end = world->findMark("end");
        p->setPos(Vec3f(end.x, 0.0, end.y));
        p->getComponent<Navigator>()->goTo(start, 0.25);
        player = p->proxy();

        Audio::Signal::Handle h1 = audioResMan->get("sounds/clock_tick.wav");
        Audio::Signal::Handle h2 = audioResMan->get("sounds/pz_breathe_loop2.wav");
        Audio::Signal::Handle h3 = audioResMan->get("sounds/chargeloop.wav");
        Audio::Signal::Handle storm = audioResMan->get("sounds/storm.wav", false);
        Audio::Signal::Handle music = audioResMan->get("sounds/creep.wav", false);
        
        Audio::Env::Handle env(new Audio::Env(1.0 / 100.0, 340.0, 0.1));
        audioEngine->setDefaultEnv(env);
        
        audioEngine->setHeadState(p->getAbsPos(), 0.0, 0.0);
        //audioEngine->play(h1, 0, Audio::FULL_QUALITY, true, 1.0, Vec3f(start.x, 0.0, start.y));
        audioEngine->play(music, 0, Audio::AMBIENT_QUALITY, true);
        audioEngine->play(storm, 0, Audio::AMBIENT_QUALITY, true);
        
        Entity* car = new Entity();
        world->addChild(car, Vec3f(5.0, 0.0, 5.0));
        
        std::list<Vec2f> track;
        TrackFollower* trackFollower = new TrackFollower();
        trackFollower->setSpeed(3.0);
        trackFollower->setRotSpeed(M_PI);
        car->addComponent(trackFollower);
        
        for(int i = 0; i < 11; i++)
        {
            Vec2f p;
            p.x = 5.0 + 5.0 * cos(i / 10.0 * 2.0 * M_PI);
            p.y = 5.0 + 5.0 * sin(i / 10.0 * 2.0 * M_PI);
            track.push_back(p);
        }
        
        trackFollower->setTrack(track, 0.25);
        trackFollower->setLoop(true);
        
        SoundComponent* soundComponent = new SoundComponent();
        car->addComponent(soundComponent);
        soundComponent->play(0, h2, Audio::FULL_QUALITY, 0.5, true, car->getAbsPos());
        
        
        
        if(luaL_loadfile(luaState, "maps/level.lua") || lua_pcall(luaState, 0, 0, 0))
            printf("Cannot run script: %s\n", lua_tostring(luaState, -1));
    }
    
    void execute(std::string const& cmd)
    {
        std::list<ParsedCommand> parsedList;
        parser->parse(parsedList, cmd);

        while(!parsedList.empty())
        {
            ParsedCommand* pc = &parsedList.front();
            
            for(int i = 0; i < pc->length(); i++)
                printf("%s ", pc->word(i).c_str());

            printf(" | ");
            printf("%s", pc->getPattern().c_str());
            //for(int i = 0; i < pc->length(); i++)
                //printf("%c-", pc->type(i));
            printf("\n");

            // ****************
            // * Actual work: *
            // ****************

            if(pc->matchesPattern("an"))
            {
                StringList descList;

                if(pc->word(0) == "go")
                    player.getObject()->go(pc->word(1), descList);
                else if(pc->word(0) == "take")
                    player.getObject()->take(pc->word(1), descList);
                else if(pc->word(0) == "drop")
                    player.getObject()->drop(pc->word(1), descList);
            }

            else if(pc->matchesPattern("adn"))
            {
                StringList descList;
                descList.push_back(pc->word(1));

                if(pc->word(0) == "go")
                    player.getObject()->go(pc->word(2), descList);
                else if(pc->word(0) == "take")
                    player.getObject()->take(pc->word(2), descList);
            }

            parsedList.pop_front();
        }
    }
    
    void goTo(int px, int py, float tx, float ty, float scale)
    {
        Player* p = player.getObject();
        p->getComponent<Navigator>()->goTo(Vec2f(px / scale - tx, py / scale - ty), 0.25);
    }
    
    void logic()
    {
        if(audioEngine->logic() && world)
            world->logic();
    }
    
    void render(BITMAP* buffer, float x, float y, float scale) const
    {
        if(world)
            world->render(buffer, x, y, scale);
    }
    
    // ************
    // * LUA bind *
    // ************
    
    void luaBind()
    {
        Entity::luaBind(luaState);
        Component::luaBind(luaState);
        Container::luaBind(luaState);
        Identity::luaBind(luaState);
        SoundComponent::luaBind(luaState);
        Takable::luaBind(luaState);
        
        bindLuaVec2f(luaState);
        bindLuaVec3f(luaState);

        luabind::module(luaState)
                [
                def("getWorldContainer", &getWorldContainer)
                ];
    }
};

int main(int argc, char** argv)
{   
    init();
    
    Engine* engine = new Engine();
    engine->loadWorld("maps/test.map", 0.5);
    
    int lastMouseB = 0;
    
    InputField cmdField(80);
    
    std::map<std::string, std::vector<Audio::Signal::Handle> > soundResMap;
    printf("SIZE: %d %d\n", sizeof(soundResMap), sizeof(Audio::Signal::Handle) * 2);
    
    while(!key[KEY_ESC])
    {
        if(key[KEY_ENTER])
        {
            engine->execute(cmdField.getText());
            cmdField.clear();
            key[KEY_ENTER] = 0;
        }
        
        while(keypressed())
        {
            int key = readkey();
            cmdField.keyPressed(key & 0xff, key >> 8);
        }
        
        if(mouse_b && !lastMouseB)
        {
            engine->goTo(mouse_x, mouse_y, 0.0, 0.0, SCREEN_H / 30.0);
        }
        
        //lastMouseB = mouse_b;
            
            
        engine->logic();
        
        clear_to_color(buffer, makecol32(64, 64, 64));
        
        engine->render(buffer, 0.0, 0.0, SCREEN_H / 30.0);
        
        cmdField.render(buffer, 30, SCREEN_H - 30);
        
        draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    delete engine;
    destroy_bitmap(buffer);
    
    return 0;
}END_OF_MAIN()
