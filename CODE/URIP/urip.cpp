#include <stdio.h>
#include <conio.h>
#include <cstring>
#include <string>
#include <cstdlib>

#include "Timer.hpp"

typedef unsigned char uint8_t;

template<int B>
struct Lookup {
       static int table[256][8 / B];
       
       static int init(uint8_t c)
       {
          uint8_t mask = (1 << B) - 1;
          uint8_t t = c;
  
          for(int i = 0; i < 8 / B; ++i)
          {
             table[c][i] = (t & mask);
             t >>= B;
          }
       }
       
       static void init()
       {
           for(int i = 0; i < 256; ++i)
             init(i);
       }
};

template<int B>
int Lookup<B>::table[256][8 / B];

template<int B>
class BitBlockIter {
   uint8_t mMask;
   uint8_t const* mPos;
   uint8_t mByte;
   int mBlockIndex;
   
public:
   BitBlockIter(uint8_t const* pos): mMask((1 << B) - 1), mPos(pos), mByte(*pos), mBlockIndex(0)
   {
   }
   
   int operator*() const
   {
       return mByte & mMask;
   }
   
   int get() const
   {
       return Lookup<B>::table[*mPos][mBlockIndex];
   }
   
   void naive()
   {
        ++mBlockIndex;
        
        if(mBlockIndex >= 8 / B)
        {
            mBlockIndex = 0;
            ++mPos;
            mByte = *mPos;
        }
        
        else
           mByte >>= B;
   }
   
   void naive2()
   {
        ++mBlockIndex;
        
        /*
        bool cond = mBlockIndex >= 8 / B;
        mBlockIndex &= -(!cond);
        mPos += cond;*/
        
        
        if(mBlockIndex >= 8 / B)
        {
            mBlockIndex = 0;
            ++mPos;
        }
   }
   
   void smart()
   {
        ++mBlockIndex;
        bool cond = mBlockIndex >= 8 / B;
        mBlockIndex &= -(!cond);
        mPos += cond;
        mByte |= (*mPos & -cond);
        mByte &= (*mPos | -(!cond));
        mByte >>= (B & -(!cond));
   }
};

#define KB 1000
#define MB 1000 * KB

int main()
{
   int size = 50 * MB;
   char const* data = new char[size];
   Timer timer;
   
   Lookup<1>::init();
   Lookup<2>::init();
   Lookup<4>::init();
   Lookup<8>::init();
   
    int sum = 0;
    
   printf("Starting naive!");
   
   timer.reset();
   
   
   for(int j = 0; j < 20; ++j)
   {
      BitBlockIter<1> iter((uint8_t*)data);
      
      for(int i = 0; i < size * 2; ++i)
      {
         iter.naive();
         sum += *iter;
      }
   }
   
   printf("Done! %f\n", timer.elapsed());
   
   
   printf("Starting smart!");
   
   timer.reset();
   
   for(int j = 0; j < 20; ++j)
   {
      BitBlockIter<1> iter((uint8_t*)data);
      
      for(int i = 0; i < size * 2; ++i)
      {
         iter.naive2();
         sum += iter.get();
      }
   }
   
   printf("Done! %f\n", timer.elapsed());
   
   delete[] data;
 
 
 /*
    char const* str = "zrololo!";
    BitBlockIter<4> iter((uint8_t*)str);
 
    for(int i = 0; i < strlen(str); ++i)
    {
       printf("%x", str[i]);
    }
    
    printf("\n");
    
    for(int i = 0; i < strlen(str) * 2; ++i)
    {
       printf("%x", iter.get());
       iter.naive2();
    }*/
 

 
 /*
  int bpb = 8; // Bits per block.
  unsigned char c = 255;
  unsigned char mask = (1 << bpb) - 1;
  
  printf("Mask: %x\nC: %x\n\n", mask, c);
  
  for(int i = 0; i < 8 / bpb; ++i)
  {
     int v = (c & mask);
     printf("%x", v);
     c >>= bpb;
  }*/
  
  getch();
  return 0; 
}
