#ifndef _Keyboard_hpp_
#define _Keyboard_hpp_

#include <allegro.h>

class Keyboard {
    int prevKeyState[KEY_MAX];
    int lastPress[KEY_MAX];
    int firstTap[KEY_MAX];
    int tapCounter[KEY_MAX];
    
    int updateCounter;
    int maxTapLen, maxMultitapLen;

public:

    Keyboard()
    {
        updateCounter = 0;
        
        for(int i = 0; i < KEY_MAX; i++)
        {
            prevKeyState[i] = false;
            lastPress[i] = -32000;
            firstTap[i] = -32000;
            tapCounter[i] = 0;
            maxTapLen = 10;
            maxMultitapLen = 15;
        }
    }

    void update()
    {        
        for(int i = 0; i < KEY_MAX; i++)
        {
            if(pressed(i))
                lastPress[i] = updateCounter;
            
            if(tapped(i))
            {
                if(updateCounter - firstTap[i] < maxMultitapLen)
                    tapCounter[i]++;
                else
                {
                    firstTap[i] = updateCounter;
                    tapCounter[i] = 1;
                }
            }
                    
            prevKeyState[i] = key[i];
        }
        
        updateCounter++;
    }

    bool pressed(int i)
    {
        return key[i] && !prevKeyState[i];
    }

    bool released(int i)
    {
        return !key[i] && prevKeyState[i];
    }

    bool tapped(int i)
    {
        return released(i) && updateCounter - lastPress[i] < maxTapLen;
    }
    
    int tapCount(int i)
    {
        return updateCounter - firstTap[i] < maxMultitapLen ? tapCounter[i] : 0;
    }
    
    bool doubleTapped(int i)
    {
        if(updateCounter - firstTap[i] < maxMultitapLen && tapCounter[i] > 1)
        {
            tapCounter[i] = 0;
            return true;
        }
            
        return false;
    }
    
    void resetTapCount(int i)
    {
        tapCounter[i] = 0;
    }
    
    bool held(int i)
    {
        return key[i];
    }
};

#endif
