#include <stdio.h>
#include <conio.h>
#include <allegro.h>

#include <cstring>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>
#include <fstream>

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

template<int B, typename T>
bool matchBlocks(int aToB[1 << B], BitBlockIter<B> a, T b, int n)
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
template <int B, typename T>
std::vector<Result<B>> search(BitBlockIter<B> src, T patt, int srcLen, int pattLen)
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
  std::vector<int> patt = {'5','5','5','1','1','7','1','7','1','5','1','5'};
  
  BitBlockIter<8> src(str1);
  
  //printf("%d\n\n", sizeof(patt));
  
  auto resVec = search(src, patt.begin(), strlen(str1), patt.size());
  
  for(auto& res: resVec)
  {
      auto pos = res.pos;
      for(int i = 0; i < patt.size(); ++i, ++pos)
         printf("%c", *pos);
      printf("\n");
  }
}*/

struct Data {
   char* buff;
   int size; // In bytes...
   
   Data()
   {
      buff = nullptr;
      size = 0;
   }
};

Data readFile(char const* path, int padding)
{
   std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
   Data data;
   
   if(file.is_open())
   {
       data.size = file.tellg();
       data.buff = new char[data.size + padding];
       file.seekg(0, std::ios::beg);
       file.read(data.buff, data.size);
       file.close();
   }
   
   return data;
}

/*

template<int B>
bool test(std::vector<std::vector<Result<B>>> const& results, int i=0, BitBlockIter<B> lastPos=nullptr, int diff=-1)
{
    if(i >= results.size() - 1)
       return true;
       
    printf("%d", i);
    
    for(auto res : results[i])
    {
        printf("[%d]", BitBlockIter<B>::diff(res.pos, lastPos));
     
        if(res.pos > lastPos && (diff < 0 || BitBlockIter<B>::diff(res.pos, lastPos)) )
           if(test(results, i + 1, res.pos, BitBlockIter<B>::diff(res.pos, lastPos)))
              return true;
    }

    printf("\n");
    
    return false;
}*/

int main()
{   
    bool quit = false;
    
    init();
    
    BITMAP* temp = load_bitmap("patt.bmp", nullptr);
    PalBmp bmp(temp);
    destroy_bitmap(temp);
    
    
    std::vector<std::vector<Result<4>>> results;
    
    Data data = readFile("src.bmp", MAX(bmp.buff.getWidth(), bmp.buff.getHeight()));
    
    /*
    auto row = bmp.getRow(0);
    for(auto i : row)
      printf("%x", i);
   
    printf("\n\n\n");
   
    BitBlockIter<4> iter(data.buff);
    
    
    for(int i = 0; i < data.size * 2; ++i, ++iter)
       printf("%x", *iter);
       
    printf("\n\n");
       
    for(int i = 0; i < data.size; ++i)
      printf("%02x", (uint8_t)data.buff[i]);*/
    
    for(int i = 0; i < MIN(bmp.buff.getHeight(), bmp.buff.getWidth()); ++i)
    {
       auto row = bmp.getRow(i);
       results.push_back(search<4>(data.buff, row.begin(), data.size * 2, row.size()));
       printf("%d ", results[i].size());
    }
    
    printf("\n");
    
    for(int i = 0; i < results.size(); ++i)
    {
      for(auto const& res : results[i])
        printf("%d ", BitBlockIter<4>::diff(res.pos, data.buff));
      
      printf("\n");
    }
    
    //printf("\n\n%d", test(results));
    
    delete[] data.buff;
    
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

