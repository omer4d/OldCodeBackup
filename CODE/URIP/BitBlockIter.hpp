#ifndef _BIT_BLOCK_ITER_HPP_
#define _BIT_BLOCK_ITER_HPP_

typedef unsigned char uint8_t;

template<int B>
class BitBlockIter {
   static int sTable[256][8 / B];
   static bool sLutReady;
   
   uint8_t const* mPos;
   int mBlockIndex;
   
   static int initLutAt(uint8_t c)
   {
       uint8_t mask = (1 << B) - 1;
       uint8_t t = c;
  
       for(int i = 0; i < 8 / B; ++i)
       {
          sTable[c][i] = (t & mask);
          t >>= B;
       }
       
       // Reverse it!
       
       for(int i = 0; i < 4 / B; ++i)
       {
           int t = sTable[c][i];
           sTable[c][i] = sTable[c][8 / B - 1 - i];
           sTable[c][8 / B - 1 - i] = t;
       }
   }
   
   static void initLut()
   {
      for(int i = 0; i < 256; ++i)
         initLutAt(i);
      
      sLutReady = true;
   }
   
   void init()
   {
      if(!sLutReady)
          initLut();
   }
   
public:
   BitBlockIter(): mPos(nullptr), mBlockIndex(0)
   {
       init();
   }
   
   BitBlockIter(void const* pos): mPos((uint8_t const*)pos), mBlockIndex(0)
   {
       init();
   }
   
   int operator*() const
   {
       return sTable[*mPos][mBlockIndex];
   }
   
   BitBlockIter& operator++()
   {
        ++mBlockIndex;
        
        if(mBlockIndex >= 8 / B)
        {
            mBlockIndex = 0;
            ++mPos;
        }
        
        return *this;
   }
   
   BitBlockIter operator++(int)
   {
       auto temp = *this;
       ++(*this);
       return temp;
   }
   
   // Relational operators:
   
   static int diff(BitBlockIter const& a, BitBlockIter const& b)
   {
       return int((unsigned int)a.mPos - (unsigned int)b.mPos) * (8 / B) + (a.mBlockIndex - b.mBlockIndex);
   }
   
   bool operator==(BitBlockIter const& other) const
   {
      return this->mPos == other.mPos && this->mBlockIndex == other.mBlockIndex;
   }
   
   bool operator>(BitBlockIter const& other) const
   {
      return diff(*this, other) > 0;
   }
   
   bool operator<(BitBlockIter const& other) const
   {
      return diff(*this, other) < 0;
   }
   
   bool operator>=(BitBlockIter const& other) const
   {
      return diff(*this, other) >= 0;
   }
   
   bool operator<=(BitBlockIter const& other) const
   {
      return diff(*this, other) <= 0;
   }
};

template<int B>
int BitBlockIter<B>::sTable[256][8 / B];

template<int B>
bool BitBlockIter<B>::sLutReady = false;

// ***********************************************
// * Simplified specialization for 8-bit blocks: *
// ***********************************************

template<>
class BitBlockIter<8> {
    uint8_t const* mPos;
      
public:
   BitBlockIter(): mPos(nullptr)
   {
   }
   
   BitBlockIter(void const* pos): mPos((uint8_t const*)pos)
   {
   }
   
   int operator*() const
   {
       return *mPos;
   }
   
   BitBlockIter operator++()
   {
       ++mPos;
       return *this;
   }
   
   BitBlockIter operator++(int)
   {
       auto temp = *this;
       ++(*this);
       return temp;
   }
   
   // Relational operators:
            
   static int diff(BitBlockIter const& a, BitBlockIter const& b)
   {
       return int(a.mPos - b.mPos);
   }
   
   bool operator==(BitBlockIter const& other) const
   {
      return this->mPos == other.mPos;
   }
   
   bool operator>(BitBlockIter const& other) const
   {
      return diff(*this, other) > 0;
   }
   
   bool operator<(BitBlockIter const& other) const
   {
      return diff(*this, other) < 0;
   }
   
   bool operator>=(BitBlockIter const& other) const
   {
      return diff(*this, other) >= 0;
   }
   
   bool operator<=(BitBlockIter const& other) const
   {
      return diff(*this, other) <= 0;
   }
};

#endif
