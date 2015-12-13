/* 
 * File:   ResourceManager.hpp
 * Author: Stas
 *
 * Created on February 7, 2012, 11:38 PM
 */

#ifndef RESOURCEMANAGER_HPP
#define	RESOURCEMANAGER_HPP

#include <string>
#include <map>
#include "Logger.hpp"

template <class T>
class ResourceManager {
    typedef std::map<std::string, T> Map;
    Map map;
    Logger logger;

public:

    ResourceManager(char const* name) : logger(name)
    {
    }

    T get(std::string path)
    {
        typename Map::iterator i = map.find(path);

        if(i == map.end())
        {
            try
            {
                map[path] = T(path.c_str());
            }

            catch(std::exception& e)
            {
                map[path] = T();
                logger.log("Warning: %s.\n", e.what());
            }

            logger.log("Loaded: %s\n", path.c_str());
        }

        return map[path];
    }

    void erase(std::string path)
    {
        map.erase(path);
    }

    void erase(T t)
    {
        for(typename Map::iterator i = map.begin(); i != map.end(); ++i)
            if(i->second == t)
            {
                map.erase(i);
                break;
            }
    }

    void clear()
    {
        map.clear();
    }
};

#endif	/* RESOURCEMANAGER_HPP */

