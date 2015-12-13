#include <stdio.h>
#include <conio.h>
#include <allegro.h>

#include <cstring>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>

#include "Buffer.hpp"
#include "BitBlockIter.hpp"
#include "PalBmp.hpp"

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


template<int B>
struct Result {
       BitBlockIter<B> pos;
       int mapping[1 << B];
};

template<int B>
bool matchBlocks(int aToB[1 << B], BitBlockIter<B> a, BitBlockIter<B> b, int n)
{
    int bToA[1 << B];
    
    for(int i = 0; i < (1 << B); ++i)
    {
       aToB[i] = -1;
       bToA[i] = -1;
    }
    
    for(int i = 0; i < n; ++i, ++a, ++b)
    {
       if(aToB[*a] >= 0) // Is *a already mapped to something?
       {
           if(aToB[*a] != *b)
              return false;
       }
       
       else if(bToA[*b] < 0) // Is *b not mapped to anything yet?
       {
           aToB[*a] = *b;
           bToA[*b] = *a;
       }
       
       else // No one-to-one mapping?
          return false;
    }
    
    return true;
}


// This assumes that src is padded. srcLen doesn't count padding.
template <int B>
std::vector<Result<B>> search(BitBlockIter<B> src, BitBlockIter<B> patt, int srcLen, int pattLen)
{
    std::vector<Result<B>> resVec;
 
    for(int i = 0; i <= srcLen - pattLen; ++i, ++src)
    {
       Result<B> res;
     
       if(matchBlocks<B>(res.mapping, src, patt, pattLen))
       {
           res.pos = src;
           resVec.push_back(res);
       }
    }
    
    return resVec;
}

/*
void testIter()
{
    char const* str = "zrololo!";
    
    BitBlockIter<4>::initLut();
    BitBlockIter<4> iter((uint8_t*)str);
    
    for(int i = 0; i < strlen(str); ++i)
    {
       printf("%x", str[i]);
    }
    
    printf("\n");
    
    for(int i = 0; i < strlen(str) * 2; ++i)
    {
       printf("%x", *iter);
       ++iter;
    }
}*/

/*
void testSearch()
{
  char const* str1 = "zqweqwe111223232121iwpeoirpwoeir555117171515";
  char const* str2 = "555117171515";
  
  BitBlockIter<8> src(str1);
  BitBlockIter<8> patt(str2);
  
  auto resVec = search(src, patt, strlen(str1), strlen(str2));
  
  for(auto& res: resVec)
  {
      auto pos = res.pos;
      for(int i = 0; i < strlen(str2); ++i, ++pos)
         printf("%c", *pos);
      printf("\n");
  }
}*/

int main()
{   
    bool quit = false;
    
    init();
    
    BITMAP* temp = load_bitmap("in.bmp", nullptr);
    PalBmp bmp(temp);
    destroy_bitmap(temp);
    
    //printf("%d", bmp.pal.size());
    
    while(!quit)
    {
        if(key[KEY_ESC]) quit = true;
     
        clear_to_color(buffer, 0);
        bmp.draw(buffer);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()

