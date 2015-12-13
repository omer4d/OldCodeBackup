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

    struct Resource {
        T* data;
        bool isPerm, isDead;

        Resource(T* data, bool isPerm)
        {
            this->data = data;
            this->isPerm = isPerm;
            this->isDead = false;
        }

        Resource()
        {
            this->data = NULL;
            this->isPerm = false;
            this->isDead = false;
        }
    };

    typedef std::map<std::string, Resource> Map;
    Map map;
    Logger logger;

    void erase(typename Map::iterator i)
    {
        delete i->second.data;
        map.erase(i);
    }

public:

    void clear()
    {
        for(typename Map::iterator i = map.begin(); i != map.end();)
            erase(i++);
    }

    ResourceManager(char const* name) : logger(name)
    {
    }

    ~ResourceManager()
    {
        clear();
    }

    Logger& getLogger()
    {
        return logger;
    }

    void beginLoad()
    {
        for(typename Map::iterator i = map.begin(); i != map.end(); ++i)
        {
            Resource& res = i->second;
            res.isDead = !res.isPerm; // Temporary resources die unless reloaded.
        }
    }

    void endLoad()
    {
        for(typename Map::iterator i = map.begin(); i != map.end();)
            if(i->second.isDead)
                erase(i++);
            else
                ++i;
    }

    T const& get(std::string const& path, bool isPerm = false)
    {
        typename Map::iterator i = map.find(path);

        if(i == map.end())
        {
            try
            {
                map[path] = Resource(new T(path.c_str()), isPerm);
                logger.log("Loaded: %s\n", path.c_str());
            }

            catch(std::exception& e)
            {
                logger.log("Warning: %s.\n", e.what());
                map[path] = Resource(new T(), isPerm);
            }
        }

        else if(isPerm) // Same resource needed in another place permanently.
            i->second.isPerm = true;

        map[path].isDead = false;
        return *map[path].data;
    }

    void setPermanence(std::string const& path, bool isPerm)
    {
        typename Map::iterator i = map.find(path);
        if(i != map.end())
            i->second.isPerm = isPerm;
    }

    void erase(std::string const& path)
    {
        typename Map::iterator i = map.find(path);
        if(i != map.end())
            erase(i);
    }

    void printResList()
    {
        for(typename Map::iterator i = map.begin(); i != map.end(); ++i)
            logger.log("%s\n", i->first.c_str());
    }
};

#endif	/* RESOURCEMANAGER_HPP */

