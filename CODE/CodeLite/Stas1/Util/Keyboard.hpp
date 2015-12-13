#ifndef _Keyboard_hpp_
#define _Keyboard_hpp_

#include <initializer_list>
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

    bool pressed(int i) const
    {
        return key[i] && !prevKeyState[i];
    }
	
	bool pressed(int a, int b) const
	{
		return (pressed(a) || held(a)) && pressed(b);
	}
	
	bool pressed(std::initializer_list<int> const& list) const
	{
		bool ok = false;
		
		for(auto key: list)
		{
			if(pressed(key))
				ok = true;
			else if(!held(key))
				return false;
		}
		
		return ok;
	}

    bool released(int i) const
    {
        return !key[i] && prevKeyState[i];
    }
	
	bool held(int i) const
    {
        return key[i];
    }
	
	bool held(int a, int b) const
	{
		return held(a) && held(b);
	}
	
	bool held(std::initializer_list<int> const& list) const
	{
		for(auto key: list)
			if(!held(key))
				return false;
		return true;
	}

    bool tapped(int i) const
    {
        return released(i) && updateCounter - lastPress[i] < maxTapLen;
    }
    
    int tapCount(int i) const
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
};

#endif
