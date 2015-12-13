#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <vector>

#include <string>
#include <map>

#include "Vec2f.hpp"

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}

class Mouse {
      Vec2f pos, delta;
      static Mouse* instance;
      int lastMb;
      
      Mouse()
      {
        pos.init(mouse_x, mouse_y);
      }
      
      public:
      static Mouse& getInstance()
      {
         if(!Mouse::instance)
           Mouse::instance = new Mouse();
           
         return *Mouse::instance;
      }
      
      void update()
      {
        int br = int(MIN(SCREEN_W, SCREEN_H) * 0.3);
     
        if(mouse_x < br || mouse_x > SCREEN_W - br || mouse_y < br || mouse_y > SCREEN_H - br)
           position_mouse(SCREEN_W / 2, SCREEN_H / 2);
           
        int dx, dy;
        get_mouse_mickeys(&dx, &dy);
        delta.init(dx, dy);
        
        pos += delta;
        
        if(pos.x < 0)            pos.x = 0;
        if(pos.x > SCREEN_W - 1) pos.x = SCREEN_W - 1;
        if(pos.y < 0)            pos.y = 0;
        if(pos.y > SCREEN_H - 1) pos.y = SCREEN_H - 1;
        
        lastMb = mouse_b;
      }
      
      Vec2f const& getPos() const
      {
         return pos;
      }
      
      Vec2f const& getDelta() const
      {
         return delta;
      }
      
      int held() const
      {
         return mouse_b;
      }
      
      int pressed() const
      {
         return (mouse_b && !lastMb) ? mouse_b : 0;
      }
      
      int released() const
      {
         return (!mouse_b && lastMb) ? lastMb : 0;
      }
      
      bool held(int b) const
      {
         b = 1 << (b - 1);
         return mouse_b & b;
      }
      
      bool pressed(int b) const
      {
         b = 1 << (b - 1);
         return (mouse_b & b) && !(lastMb & b);
      }
      
      bool released(int b) const
      {
         b = 1 << (b - 1);
         return !(mouse_b & b) && (lastMb & b);
      }
};

Mouse* Mouse::instance = NULL;


class Commands {
  typedef std::map<std::string, bool> Map;
  
  Map state1, state2;
  Map* currState;
  Map* lastState;
  
public:
  Commands()
  {
     currState = &state1;
     lastState = &state2;
  }
       
  void update(char const* key, bool flag)
  {
     (*currState)[key] = flag;
  }
  
  void finishUpdate()
  {
     Map* temp = currState;
     
     temp = currState;
     currState = lastState;
     lastState = temp;
     
     for(Map::iterator i = currState->begin(); i != currState->end(); ++i)
       i->second = false;
  }
  
  bool start(char const* key)
  {
     return (*currState)[key] && !(*lastState)[key];
  }
  
  bool on(char const* key)
  {
     return (*currState)[key];
  }
  
  bool end(char const* key)
  {
     return !(*currState)[key] && (*lastState)[key];
  }
};

int main()
{   
    bool exit = false;
    
    init();
    Mouse& mouse = Mouse::getInstance();

    Commands cmd;

    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     cmd.update("shoot", key[KEY_SPACE] && mouse_b == 1);
     
     if(cmd.start("shoot"))
       printf("CLICK");
     if(cmd.end("shoot"))
       printf("BANG!");
     if(cmd.on("shoot"))
       printf(".");
     
     cmd.finishUpdate();
     
     Vec2f p = mouse.getPos();
     mouse.update();
       
    
     clear_to_color(buffer, 0);
     draw_sprite(buffer, mouse_sprite, int(p.x), int(p.y));
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
