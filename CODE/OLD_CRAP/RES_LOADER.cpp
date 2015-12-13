#include <string>
#include <map>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>

#include <stdio.h>
#include <conio.h>

struct Dummy {
      Dummy()
      {
      }
      
      Dummy(char const* path)
      {
      }
      
      ~Dummy()
      {
        //printf("died!\n");
      }
};

class Logger {
    static char sLogBuff[700];
    std::ostream* mLogStream;
    std::string mName;
    bool mPrintTag;

public:
    void log(char const* fmt, ...)
    {
         int n = 0;
         
         if(mPrintTag)
            n = snprintf(sLogBuff, 200, "[%s]: ", mName.c_str());
         
         va_list argp;
         va_start(argp, fmt);
         vsnprintf(&sLogBuff[n], 500, fmt, argp);
         va_end(argp);

         if(mLogStream) (*mLogStream) << sLogBuff;
    }

    Logger(std::string pName, std::ostream& pLogStream = std::cout)
    {
         mPrintTag = true;
         mName = pName;
         mLogStream = &pLogStream;
    }
    
    void setPrintTag(bool printTag)
    {
         mPrintTag = printTag;
    }

    void setOutStream(std::ostream& s)
    {
         mLogStream = &s;
    }
};

char Logger::sLogBuff[700];

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
    
    ResourceManager(char const* name): logger(name)
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
            res.isDead = !res.isPerm; // Old temporary resrouces are potentially dead.
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
    
    T const& get(std::string const& path, bool isPerm=false)
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

int main()
{
   ResourceManager<Dummy> resMan("DummyManager");
   
   resMan.getLogger().setPrintTag(false);
   
   resMan.beginLoad();
       resMan.get("test1.txt", true);
       resMan.get("test2.txt");
       resMan.get("test3.txt");
       resMan.get("test4.txt");
       resMan.setPermanence("test2.txt", true);
   resMan.endLoad();
   
   resMan.beginLoad();
       resMan.get("test4.txt");
   resMan.endLoad();
   
   resMan.erase("test2.txt");
   
   resMan.printResList();
   
   resMan.clear();
   resMan.printResList();
   
   getch();
   return 0;
}
