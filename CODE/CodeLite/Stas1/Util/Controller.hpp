/* 
 * File:   Controller.hpp
 * Author: Stas
 *
 * Created on October 4, 2013, 9:53 AM
 */

#ifndef CONTROLLER_HPP
#define	CONTROLLER_HPP


#include <map>
#include <string>

class Controller {
    typedef std::map<std::string, bool> Map;

    Map state1, state2;
    Map* currState;
    Map* lastState;
    
public:

    Controller()
    {
        currState = &state1;
        lastState = &state2;
    }

    void update()
    {
        Map* temp = currState;

        temp = currState;
        currState = lastState;
        lastState = temp;

        for(Map::iterator i = currState->begin(); i != currState->end(); ++i)
            i->second = false;
    }

    void set(char const* key, bool flag)
    {
        (*currState)[key] = flag;
    }

    bool start(char const* key)
    {
        return(*currState)[key] && !(*lastState)[key];
    }

    bool on(char const* key)
    {
        return(*currState)[key];
    }

    bool end(char const* key)
    {
        return !(*currState)[key] && (*lastState)[key];
    }
};

#endif	/* CONTROLLER_HPP */

