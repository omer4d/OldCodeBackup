#ifndef _ResourceManager_hpp_
#define _ResourceManager_hpp_

#include <map>
#include "boost/weak_ptr.hpp"
#include "MissingFileException.hpp"
#include "Logger.hpp"

namespace Audio {

    class ResourceManager {
        typedef std::map<std::string, Signal::Handle> Map;
        Map mMap;
        Logger mLogger;
        
    public:
        
        ResourceManager(): mLogger("AudioResourceManager")
        {
        }

        Signal::Handle get(std::string path, bool spatial = true)
        {
            Map::iterator i = mMap.find(path);
            
            if(i == mMap.end())
            {
                Signal* sig;
                
                try
                {
                    sig = new Signal(path.c_str(), spatial);
                }
                
                catch(MissingFileException& e)
                {
                    sig = new Signal();
                    mLogger.log("Warning: %s.\n", e.what());
                }
                
                Signal::Handle h(sig);
                mMap[path] = h;
                return h;
            }
            
            else
                return mMap[path];
        }
        
        void erase(std::string path)
        {
            mMap.erase(path);
        }
        
        void erase(Signal::Handle h)
        {
            for(Map::iterator i = mMap.begin(); i != mMap.end(); ++i)
                if(i->second == h)
                {
                    mMap.erase(i);
                    break;
                }
        }
        
        void clear()
        {
            mMap.clear();
        }
    };

}

#endif
