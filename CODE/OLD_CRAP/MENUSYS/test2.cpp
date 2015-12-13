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
       
       virtual void draw(BITMAP* buffer, int dx, int dy) = 0;
};

struct MenuItemGroup {
       typedef std::list<MenuItem*> ItemList;
       typedef ItemList::iterator ItemIter;
       int x, y, w, h;
       char const* title;
       
       static const int ITEM_H_SPACING = 5;
       
       enum Positioning {
            UNDER,
            UNDER_CENTERED,
            CENTERED
       };
       
       ItemList itemList;
       ItemIter currItem;
       
       MenuItemGroup(int w, int h, char const* title=NULL)
       {
           this->w = w;
           this->h = h;
           this->title = title;
           
           this->currItem = itemList.end();
       }
       
       ~MenuItemGroup()
       {
           for(ItemIter i = itemList.begin(); i != itemList.end(); ++i)
              delete *i;
       }
       
       void onSelect()
       {
           if(itemList.empty())
              return;
           
           currItem = itemList.begin();
           (*currItem)->onSelect();
       }
       
       void onDeselect()
       {
           if(itemList.empty())
              return;
           
           (*currItem)->onDeselect();
           currItem = itemList.end();
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
              
              if(pos == UNDER_CENTERED)
              {
                 item->x = (w - item->w) / 2;
                 item->y = h / 10;
              }
              
              else
              {
                 item->x = (w - item->w) / 2;
                 item->y = (h - item->h) / 2;
              }
              
              
              item->onSelect();
              itemList.push_back(item);
              currItem = itemList.begin();
           }
           
           else
           {
              MenuItem* last = *(--itemList.end());
              
              if(pos == UNDER_CENTERED)
              {
                 item->x = last->x + (last->w - item->w) / 2;
                 item->y = last->y + item->h + ITEM_H_SPACING;
              }
                 
              else if(pos == UNDER)
              {
                 item->x = last->x;
                 item->y = last->y + item->h + ITEM_H_SPACING;
              }
              
              itemList.push_back(item);
           }
       }
       
       void logic()
       {
           for(ItemIter i = itemList.begin(); i != itemList.end(); ++i)
               (*i)->logic();
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
       
       void onAction()
       {
           (*currItem)->onAction();
       }
       
       void draw(BITMAP* buffer, int dx, int dy)
       {
           rect(buffer, x + dx, y + dy, x + dx + w, y + dy + h, makecol(255, 255, 255));
           
           if(title)
              textout_ex(buffer, font, title, x + dx + 10, y + dy - 4, makecol(255, 255, 255), 0);
           
           for(ItemIter i = itemList.begin(); i != itemList.end(); ++i)
           {
               (*i)->draw(buffer, x + dx, y + dy);
           }
       }
};


struct Menu {
       typedef std::list<MenuItemGroup*> GroupList;
       typedef GroupList::iterator GroupIter;
       
       static const int MENU_W = 800, MENU_H = 600;
       
       Menu* parent;
       GroupList groupList;
       GroupIter currGroup;
       
       Menu(Menu* parent)
       {
           currGroup = groupList.end();
           this->parent = parent;
       }
       
       ~Menu()
       {
           for(GroupIter i = groupList.begin(); i != groupList.end(); ++i)
              delete *i;
       }
       
       void add(MenuItemGroup* group, int x, int y)
       {
           bool isFirst = groupList.empty();
        
           group->x = x;
           group->y = y;
           groupList.push_back(group);
           
           if(isFirst)
           {
             currGroup = groupList.begin();
             group->onSelect();
           }
           
           else
             group->onDeselect();
       }
       
       void add(MenuItem* item, int x, int y)
       {
           if(groupList.empty())
              add(new MenuItemGroup(MENU_W, MENU_H), 0, 0);
           
           (*groupList.begin())->add(item, x, y);
       }
       
       void add(MenuItem* item, MenuItemGroup::Positioning pos=MenuItemGroup::UNDER_CENTERED)
       {
           if(groupList.empty())
              add(new MenuItemGroup(MENU_W, MENU_H), 0, 0);
           
           (*groupList.begin())->add(item, pos);
       }
       
       void logic()
       {
           Keyboard& keyboard = MenuProperties::getInstance().keyboard;
        
           for(GroupIter i = groupList.begin(); i != groupList.end(); ++i)
               (*i)->logic();
               
           if(keyboard.pressed(KEY_LEFT))
              selectPrev();
           if(keyboard.pressed(KEY_RIGHT))
              selectNext();
               
           if(keyboard.pressed(KEY_UP))
              (*currGroup)->selectPrev();
           if(keyboard.pressed(KEY_DOWN))
              (*currGroup)->selectNext();
           if(keyboard.pressed(KEY_ENTER))
              (*currGroup)->onAction();
       }
       
       void selectNext()
       {
           if(groupList.empty())
             return;
           
           (*currGroup)->onDeselect();
           
           if(currGroup == --groupList.end())
              currGroup = groupList.begin();
           else
              ++currGroup;
              
           (*currGroup)->onSelect();
       }
       
       void selectPrev()
       {
           if(groupList.empty())
             return;
           
           (*currGroup)->onDeselect();
           
           if(currGroup == groupList.begin())
              currGroup = --groupList.end();
           else
              --currGroup;
              
           (*currGroup)->onSelect();
       }
       
       void draw(BITMAP* buffer)
       {
           clear_to_color(buffer, 0);
           
           for(GroupIter i = groupList.begin(); i != groupList.end(); ++i)
           {
               (*i)->draw(buffer, 0, 0);
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

struct ActionListener {
       virtual void onAction() = 0;
};

struct Image: public MenuItem {
       BITMAP* bitmap;
       
       Image(BITMAP* bitmap)
       {
          this->bitmap = bitmap;
          this->w = bitmap->w;
          this->h = bitmap->h;
       }
       
       
       void draw(BITMAP* buffer, int dx, int dy)
       {
          draw_sprite(buffer, bitmap, x + dx, y + dy);
       }
};

struct Action: public MenuItem {
       char const* text;
       int selCol, deselCol;
       ActionListener* listener;
       
       Action(ActionListener* listener, char const* text,
                  int selCol=makecol(255, 255, 255), int deselCol=makecol(128, 128, 128))
       {
          this->listener = listener;
          this->text = text;
          this->selCol = selCol;
          this->deselCol = deselCol;
          
          this->w = 8 * strlen(text);
          this->h = 8;
       }
       
       void onAction()
       {  
          if(listener)
             listener->onAction();
       }
       
       void draw(BITMAP* buffer, int dx, int dy)
       {
          textout_ex(buffer, font, text, x + dx, y + dy, selected ? selCol : deselCol, -1);
       }
};


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
       
       void draw(BITMAP* buffer, int dx, int dy)
       {
          textout_ex(buffer, font, text, x + dx, y + dy, selected ? selCol : deselCol, -1);
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
       
       void draw(BITMAP* buffer, int dx, int dy)
       {
          std::string str(desc);
          str += " - ";
          str += scancode_to_name(keyCode);
          
          textout_ex(buffer, font, str.c_str(), x + dx, y + dy, selected ? selCol : deselCol, -1);
       }
};

void initOptionsMenu(Menu& menu, Menu& mainMenu)
{
    int w = 200, h = 100, sp = 10;
 
    MenuItemGroup* group = new MenuItemGroup(w, h, "Player 1");
    MenuItemGroup* group2 = new MenuItemGroup(w, h, "Player 2");
    MenuItemGroup* group3 = new MenuItemGroup(w * 2 + sp, 30);
    
    group->add(new KeyBinder("Up", KEY_UP));
    group->add(new KeyBinder("Down", KEY_DOWN), MenuItemGroup::UNDER);
    group->add(new KeyBinder("Left", KEY_LEFT), MenuItemGroup::UNDER);
    group->add(new KeyBinder("Right", KEY_RIGHT), MenuItemGroup::UNDER);
    group->add(new KeyBinder("Shoot", KEY_SPACE), MenuItemGroup::UNDER);
    
   
    group2->add(new KeyBinder("Up", KEY_UP));
    group2->add(new KeyBinder("Down", KEY_DOWN), MenuItemGroup::UNDER);
    group2->add(new KeyBinder("Left", KEY_LEFT), MenuItemGroup::UNDER);
    group2->add(new KeyBinder("Right", KEY_RIGHT), MenuItemGroup::UNDER);
    group2->add(new KeyBinder("Shoot", KEY_SPACE), MenuItemGroup::UNDER);
    
    group3->add(new Link(&mainMenu, "OK"), MenuItemGroup::CENTERED);
    
    menu.add(group, 100, 100);
    menu.add(group2, 100 + w + sp, 100);
    menu.add(group3, 100, h + 10 + 100);
}

struct ExitAction: public ActionListener {
       void onAction()
       {
          exit(0);
       }
};

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* help = load_bitmap("help.bmp", NULL);
    
    Keyboard keyboard;
    MenuProperties props(keyboard);
    MenuSystem& menuSys = MenuSystem::getInstance();
    Menu& mainMenu = menuSys.mainMenu;
    
    Menu optionsMenu(&mainMenu);
    initOptionsMenu(optionsMenu, mainMenu);
    
    Menu helpMenu(&mainMenu);
    helpMenu.add(new Image(help), MenuItemGroup::CENTERED);
    
    mainMenu.add(new Action(NULL, "Start"));
    mainMenu.add(new Link(&optionsMenu, "Options"));
    mainMenu.add(new Link(&helpMenu, "Help"));
    mainMenu.add(new Action(new ExitAction(), "Exit"));
    
    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
     
     menuSys.logic();
     keyboard.update();

     clear_to_color(buffer, 0);
     
     menuSys.draw(buffer);
     
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    destroy_bitmap(help);
    deinit();
    return 0;
}END_OF_MAIN()
