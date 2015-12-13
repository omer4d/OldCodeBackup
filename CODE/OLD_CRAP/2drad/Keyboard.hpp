#ifndef _Keyboard_hpp_
#define _Keyboard_hpp_

#include <allegro.h>

class Keyboard {
      bool prevKeyState[KEY_MAX];
      
      public:
      Keyboard()
      {
        for(int i = 0; i < KEY_MAX; i++)
          prevKeyState[i] = false;
      }
             
      void update()
      {
        for(int i = 0; i < KEY_MAX; i++)
         prevKeyState[i] = key[i];
      }
      
      bool keyPressed(int i)
      {
        return key[i] && !prevKeyState[i];
      }
      
      bool keyReleased(int i)
      {
        return !key[i] && prevKeyState[i];
      }
      
      bool keyHeld(int i)
      {
        return key[i];
      }
};

#endif
