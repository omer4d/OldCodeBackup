#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>

#include <vector>
#include <list>

#include "Vec2f.hpp"
#include "Keyboard.hpp"

#include <inttypes.h>
#include <allegro.h>
#include <string>

#ifndef UINT16_MAX
#define UINT16_MAX 65535
#endif

#define PIXEL(bmp, x, y) ((long*)(bmp)->line[(y)])[(x)]

BITMAP* buffer;

void init()
{
     allegro_init();
     
     install_mouse();
     install_keyboard();
     set_color_depth(32);
     set_gfx_mode(GFX_AUTODETECT_WINDOWED, 800, 600, 0, 0);
     install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL);
     
     buffer = create_bitmap(SCREEN_W, SCREEN_H);
     
     srand(time(NULL));
}

void deinit()
{
     destroy_bitmap(buffer);
}


struct MenuProperties {
       Keyboard& keyboard;
       
       static MenuProperties* instance;
       
       MenuProperties(Keyboard& kb): keyboard(kb)
       {
           instance = this;
       }
       
       static MenuProperties& getInstance()
       {
           return *instance;
       }
};

MenuProperties* MenuProperties::instance = NULL;


struct MenuItem {
       int x, y, w, h;
       bool selected;
       
       MenuItem()
       {
           x = 0;
           y = 0;
           w = 0;
           h = 0;
       }
       
       virtual ~MenuItem() {}
       
       virtual bool isSelectable() { return true; }
       virtual void onSelect() { selected = true; }
       virtual void onDeselect() { selected = false; }
       
       virtual void onAction() {}
       virtual void logic() {}
       
       virtual void draw(BITMAP* buffer) = 0;
};

struct MenuItemGroup {
       typedef std::list<MenuItem*> ItemList;
       typedef ItemList::iterator ItemIter;
       
       static const int ITEM_H_SPACING = 5;
       
       enum Positioning {
            UNDER,
            UNDER_CENTERED
       };
       
       ItemList itemList;
       ItemIter currItem;
       
       MenuItemGroup()
       {
           this->currItem = itemList.end();
       }
       
       ~MenuItemGroup()
       {
           for(ItemIter i = itemList.begin(); i != itemList.end(); ++i)
              delete *i;
       }
       
       void addGroup(int n)
       {
            
       }
       
       void add(MenuItem* item, int x, int y)
       {
            bool isFirst = itemList.empty();
        
           item->x = x;
           item->y = y;
           itemList.push_back(item);
           
           if(isFirst)
           {
             currItem = itemList.begin();
             item->onSelect();
           }
       }
       
       void add(MenuItem* item, Positioning pos = UNDER_CENTERED)
       {
           if(itemList.empty())
           {
              item->x = (MENU_W - item->w) / 2;
              item->y = MENU_H / 10;
              item->onSelect();
              itemList.push_back(item);
              currItem = itemList.begin();
           }
           
           else
           {
              MenuItem* last = *(--itemList.end());
              
              if(pos == UNDER_CENTERED)
                 item->x = last->x + (last->w - item->w) / 2;
              else if(pos == UNDER)
                 item->x = last->x;
                 
              item->y = last->y + item->h + ITEM_H_SPACING;
              
              
              itemList.push_back(item);
           }
       }
       
       void logic()
       {
           Keyboard& keyboard = MenuProperties::getInstance().keyboard;
        
           for(ItemIter i = itemList.begin(); i != itemList.end(); ++i)
               (*i)->logic();
               
           if(keyboard.pressed(KEY_UP))
              selectPrev();
           if(keyboard.pressed(KEY_DOWN))
              selectNext();
           if(keyboard.pressed(KEY_ENTER))
              (*currItem)->onAction();
       }
       
       void selectNext()
       {
           if(itemList.empty())
             return;
           
           (*currItem)->onDeselect();
           
           if(currItem == --itemList.end())
              currItem = itemList.begin();
           else
              ++currItem;
              
           (*currItem)->onSelect();
       }
       
       void selectPrev()
       {
          if(itemList.empty())
             return;
       
         (*currItem)->onDeselect();
           
           if(currItem == itemList.begin())
              currItem = --itemList.end();
           else
              --currItem;
              
           (*currItem)->onSelect();
       }
       
       void draw(BITMAP* buffer)
       {
           clear_to_color(buffer, 0);
           
           for(ItemIter i = itemList.begin(); i != itemList.end(); ++i)
           {
               (*i)->draw(buffer);
           }
       }
};

struct Menu {
       typedef std::list<MenuItem*> ItemList;
       typedef ItemList::iterator ItemIter;
       
       std::vector<int> groupVec;
       
       static const int MENU_W = 800, MENU_H = 600;
       static const int ITEM_H_SPACING = 5;
       
       enum Positioning {
            UNDER,
            UNDER_CENTERED
       };
       
       Menu* parent;
       ItemList itemList;
       ItemIter currItem;
       
       Menu(Menu* parent)
       {
           this->parent = parent;
       }
       
       ~Menu()
       {
           for(ItemIter i = itemList.begin(); i != itemList.end(); ++i)
              delete *i;
       }
       
       void addGroup(int n)
       {
            
       }
       
       void add(MenuItem* item, int x, int y)
       {
            bool isFirst = itemList.empty();
        
           item->x = x;
           item->y = y;
           itemList.push_back(item);
           
           if(isFirst)
           {
             currItem = itemList.begin();
             item->onSelect();
           }
       }
       
       void add(MenuItem* item, Positioning pos = UNDER_CENTERED)
       {
           if(itemList.empty())
           {
              item->x = (MENU_W - item->w) / 2;
              item->y = MENU_H / 10;
              item->onSelect();
              itemList.push_back(item);
              currItem = itemList.begin();
           }
           
           else
           {
              MenuItem* last = *(--itemList.end());
              
              if(pos == UNDER_CENTERED)
                 item->x = last->x + (last->w - item->w) / 2;
              else if(pos == UNDER)
                 item->x = last->x;
                 
              item->y = last->y + item->h + ITEM_H_SPACING;
              
              
              itemList.push_back(item);
           }
       }
       
       void logic()
       {
           Keyboard& keyboard = MenuProperties::getInstance().keyboard;
        
           for(ItemIter i = itemList.begin(); i != itemList.end(); ++i)
               (*i)->logic();
               
           if(keyboard.pressed(KEY_UP))
              selectPrev();
           if(keyboard.pressed(KEY_DOWN))
              selectNext();
           if(keyboard.pressed(KEY_ENTER))
              (*currItem)->onAction();
       }
       
       void selectNext()
       {
           if(itemList.empty())
             return;
           
           (*currItem)->onDeselect();
           
           if(currItem == --itemList.end())
              currItem = itemList.begin();
           else
              ++currItem;
              
           (*currItem)->onSelect();
       }
       
       void selectPrev()
       {
          if(itemList.empty())
             return;
       
         (*currItem)->onDeselect();
           
           if(currItem == itemList.begin())
              currItem = --itemList.end();
           else
              --currItem;
              
           (*currItem)->onSelect();
       }
       
       void draw(BITMAP* buffer)
       {
           clear_to_color(buffer, 0);
           
           for(ItemIter i = itemList.begin(); i != itemList.end(); ++i)
           {
               (*i)->draw(buffer);
           }
       }
};

struct MenuSystem {
       typedef std::list<Menu*> MenuList;
       typedef MenuList::iterator MenuIter;
       
       static MenuSystem* instance;
       
       MenuList menuList;
       Menu mainMenu;
       Menu* current;
       
       static MenuSystem& getInstance()
       {
          if(!instance)
             instance = new MenuSystem();
          return *instance;
       }
       
       MenuSystem(): mainMenu(NULL)
       {
           this->current = &mainMenu;
           menuList.push_back(&mainMenu);
       }
       
       void logic()
       {
           Keyboard& keyboard = MenuProperties::getInstance().keyboard;
           
           current->logic();
           
           if(keyboard.pressed(KEY_BACKSPACE))
              goBack();
       }
       
       void draw(BITMAP* buffer)
       {
            current->draw(buffer);
       }
       
       void goBack()
       {
           if(current->parent != NULL)
              current = current->parent;
       }
};

MenuSystem* MenuSystem::instance = NULL;

struct Link: public MenuItem {
       char const* text;
       int selCol, deselCol;
       Menu* target;
       
       Link(Menu* target, char const* text,
                  int selCol=makecol(255, 255, 255), int deselCol=makecol(128, 128, 128))
       {
          this->target = target;
          this->text = text;
          this->selCol = selCol;
          this->deselCol = deselCol;
          
          this->w = 8 * strlen(text);
          this->h = 8;
       }
       
       void onAction()
       {  
          MenuSystem::getInstance().current = target;
       }
       
       void draw(BITMAP* buffer)
       {
          textout_ex(buffer, font, text, x, y, selected ? selCol : deselCol, -1);
       }
};

struct KeyBinder: public MenuItem {
       int selCol, deselCol;
       char const* desc;
       int keyCode;
       
       KeyBinder(char const* desc, int defKeyCode,
                      int selCol=makecol(255, 255, 255), int deselCol=makecol(128, 128, 128))
       {
           this->desc = desc;
           this->keyCode = defKeyCode;
           this->selCol = selCol;
           this->deselCol = deselCol;
          
          this->w = 8 * (strlen(desc) + 10);
          this->h = 8;
       }
       
       void onAction()
       {  
          bool specialKey = false;
          clear_keybuf();
          
          // Some keys don't modify the keybuffer and need special handing:
          
          while (!keypressed() && !specialKey)
          {
              if(key[KEY_LCONTROL]) keyCode = KEY_LCONTROL, specialKey = true;
              if(key[KEY_RCONTROL]) keyCode = KEY_RCONTROL, specialKey = true;
              if(key[KEY_LSHIFT]) keyCode = KEY_LSHIFT, specialKey = true;
              if(key[KEY_RSHIFT]) keyCode = KEY_RSHIFT, specialKey = true;
              if(key[KEY_ALT]) keyCode = KEY_ALT, specialKey = true;
              if(key[KEY_ALTGR]) keyCode = KEY_ALTGR, specialKey = true;
          }
          
          if(!specialKey)
            keyCode = readkey() >> 8;
          
          key[keyCode] = 0;
       }
       
       void draw(BITMAP* buffer)
       {
          std::string str(desc);
          str += " - ";
          str += scancode_to_name(keyCode);
          
          textout_ex(buffer, font, str.c_str(), x, y, selected ? selCol : deselCol, -1);
       }
};

int main()
{   
    bool exit = false;
    
    init();
    
    
    Keyboard keyboard;
    MenuProperties props(keyboard);
    
    MenuSystem& sys = MenuSystem::getInstance();
    
    Menu tempMenu(&sys.mainMenu);
    tempMenu.add(new Link(&sys.mainMenu, "Crap!"));
    
    Menu optionsMenu(&sys.mainMenu);
    optionsMenu.add(new KeyBinder("Up", KEY_UP), 10, 10);
    optionsMenu.add(new KeyBinder("Down", KEY_DOWN), Menu::UNDER);
    optionsMenu.add(new KeyBinder("Left", KEY_LEFT), Menu::UNDER);
    optionsMenu.add(new KeyBinder("Right", KEY_RIGHT), Menu::UNDER);
    optionsMenu.add(new KeyBinder("Shoot", KEY_SPACE), Menu::UNDER);
    
    optionsMenu.add(new KeyBinder("Up", KEY_UP), 200, 10);
    optionsMenu.add(new KeyBinder("Down", KEY_DOWN), Menu::UNDER);
    optionsMenu.add(new KeyBinder("Left", KEY_LEFT), Menu::UNDER);
    optionsMenu.add(new KeyBinder("Right", KEY_RIGHT), Menu::UNDER);
    optionsMenu.add(new KeyBinder("Shoot", KEY_SPACE), Menu::UNDER);
    
    sys.mainMenu.add(new Link(&tempMenu, "Start"));
    sys.mainMenu.add(new Link(&optionsMenu, "Options"));
    sys.mainMenu.add(new Link(&tempMenu, "Help"));
    sys.mainMenu.add(new Link(&tempMenu, "Exit"));
    
    
    
    
    //printf("%s", scancode_to_name(KEY_W));
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     sys.logic();
     keyboard.update();

     clear_to_color(buffer, 0);
     
     sys.draw(buffer);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    
    deinit();
    return 0;
}END_OF_MAIN()
