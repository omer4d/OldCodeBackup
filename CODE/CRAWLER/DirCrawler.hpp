#include <allegro.h>
#include <functional>
#include <string>

class DirCrawler {
public:
    class Listener {
    public:
           std::function<void(char const*)> onFile;
           std::function<void(char const*)> onDir;
    };
    
private:
     static int callback(const char *path, int attrib, void *param)
     {
        Listener* listener = (Listener*)param;
      
        if(attrib & FA_DIREC)
        {
          std::string newPath(path);
          std::string dirName(get_filename(path));
       
          newPath += "/*";
       
          if(dirName != "." && dirName != "..")
          {
             if(listener->onDir)
                listener->onDir(path);
             
             for_each_file_ex(newPath.c_str(), 0, 0, callback, param);
          }
        }
   
        else if(listener->onFile)
           listener->onFile(path);
   
        return 0;
    }
    
public:
    static void crawlFiles(char const* root, std::function<void(char const*)> onFile)
    {
        std::string fixedRoot = root;
        Listener listener;
        listener.onFile = onFile;
        
        if(fixedRoot.back() == '/' || fixedRoot.back() == '\\')
            fixedRoot += "*";
        else
            fixedRoot += "/*";
        
        for_each_file_ex(fixedRoot.c_str(), 0, 0, callback, &listener);
    }
};
