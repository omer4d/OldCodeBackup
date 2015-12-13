#ifndef _Prim32L_hpp_
#define _Prim32L_hpp_

typedef char byte8;
typedef long int32;
typedef short int16;

typedef unsigned char ubyte8;
typedef unsigned long uint32;
typedef unsigned short uint16;

// ****************

void swapBytes(ubyte8& a, ubyte8& b)
{
 ubyte8 c = a;
 a = b;
 b = c;
}

uint32 reverse32(uint32 n)
{
 ubyte8* p = (ubyte8*)&n;
 swapBytes(p[0], p[3]);
 swapBytes(p[1], p[2]);
 return n;
}

uint16 reverse16(uint16 n)
{
 ubyte8* p = (ubyte8*)&n;
 swapBytes(p[0], p[1]);
 return n;
}

// ****************

uint32 htol32(uint32 n) { return n; }
uint16 htol16(uint16 n) { return n; }
uint32 ltoh32(uint32 n) { return n; }
uint16 ltoh16(uint16 n) { return n; }

uint32 htob32(uint32 n) { return reverse32(n); }
uint16 htob16(uint16 n) { return reverse16(n); }
uint32 btoh32(uint32 n) { return reverse32(n); }
uint16 btoh16(uint16 n) { return reverse16(n); }

#endif
