#include <stdio.h>
#include <conio.h>
#include <math.h>

#include <allegro.h>
#include <inttypes.h>

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

struct Readable {
       virtual size_t read(void* dest, size_t bytes);
};

struct Writable {
       virtual size_t write(void const* src, size_t bytes);
};

struct CInStream: public Readable {
       FILE* stream;
       
       CInStream()
       {
           this->stream = NULL;
       }
       
       CInStream(FILE* stream)
       {
           this->stream = stream;
       }
       
       size_t read(void* dest, size_t bytes)
       {
            return fread(dest, 1, bytes, stream);
       }
};

struct COutStream: public Writable {
       FILE* stream;
       
       COutStream()
       {
           this->stream = NULL;
       }
       
       COutStream(FILE* stream)
       {
           this->stream = stream;
       }
       
       size_t write(void const* src, size_t bytes)
       {
            return fwrite(src, 1, bytes, stream);
       }
};

enum ByteOrder {
     BIG_ENDIAN=0, LITTLE_ENDIAN=1, NETWORK_ORDER=0, HOST_ORDER=2
};

ByteOrder getHostByteOrder()
{
   int n = 1;
   return *(char *)&n == 1 ? LITTLE_ENDIAN : BIG_ENDIAN;
}

static int16_t rev16(int16_t n)
{
       char* byte = (char*)&n;
       int8_t tmp = byte[0];
           
       byte[0] = byte[1];
       byte[1] = tmp;
       
       return n;
}
       
static int32_t rev32(int32_t n)
{
       char* byte = (char*)&n;
       int8_t tmp = byte[0];
           
       byte[0] = byte[3];
       byte[3] = tmp;
       tmp = byte[1];
       byte[1] = byte[2];
       byte[2] = tmp;
       
       return n;
}

class Reader {
       Readable* in;
       bool reverseFlag;
       
public:
       Reader(void)
       {
           in = NULL;
       }
       
       Reader(Readable& in, ByteOrder srcOrder=HOST_ORDER)
       {
           this->in = &in;
           reverseFlag = srcOrder != HOST_ORDER && srcOrder != getHostByteOrder();
       }
       
       int8_t read8() const
       {
           int8_t res;
           in->read(&res, 1);
           return res;
       }
       
       int read8(int8_t* dest, int n) const
       {
           int c = in->read(dest, n);
           return c;
       }
       
       int16_t read16() const
       {
           int16_t res;
           in->read(&res, 2);
           
           return reverseFlag ? rev16(res) : res;
       }
       
       int read16(int16_t* dest, int n) const
       {
           int c = in->read(dest, 2 * n) / 2;
           
           if(reverseFlag)
             for(int i = 0; i < c; i++)
                dest[i] = rev16(dest[i]);
           
           return c;
       }
       
       int32_t read32() const
       {
           int32_t res;
           in->read(&res, 4);
           
           return reverseFlag ? rev32(res) : res;
       }
       
       int read32(int32_t* dest, int n) const
       {
           int c = in->read(dest, 4 * n) / 4;
           
           if(reverseFlag)
             for(int i = 0; i < c; i++)
                dest[i] = rev32(dest[i]);
           
           return c;
       }
};


class Writer {
       Writable* out;
       bool reverseFlag;
       
public:
       Writer(void)
       {
           out = NULL;
       }
       
       Writer(Writable& out, ByteOrder destOrder=HOST_ORDER)
       {
           this->out = &out;
           reverseFlag = destOrder != HOST_ORDER && destOrder != getHostByteOrder();
       }
       
       void write8(int8_t src) const
       {
           out->write(&src, 1);
       }
       
       int write8(int8_t const* src, int n) const
       {
           int c = out->write(src, n);
           return c;
       }
       
       void write16(int16_t src) const
       {
           src = reverseFlag ? rev16(src) : src;
           out->write(&src, 2);
       }
       
       int write16(int16_t const* src, int n) const
       {
           int c;
           
           if(reverseFlag)
             for(c = 0; c < n; c++)
             {
                int16_t tmp = rev16(src[c]);
                if(out->write(&tmp, 2) != 2)
                   break;
             }
           
           else
             c = out->write(src, 2 * n) / 2;
           
           return c;
       }
       
       void write32(int32_t src) const
       {
           src = reverseFlag ? rev32(src) : src;
           out->write(&src, 4);
       }
       
       int write32(int32_t const* src, int n) const
       {
           int c;
           
           if(reverseFlag)
             for(c = 0; c < n; c++)
             {
                int32_t tmp = rev32(src[c]);
                if(out->write(&tmp, 4) != 4)
                   break;
             }
           
           else
             c = out->write(src, 4 * n) / 4;
           
           return c;
       }
};


struct BmpHeader {
  uint16_t	bfType;
  uint32_t bfSize;
  uint16_t	bfReserved1;
  uint16_t	bfReserved2;
  uint32_t bfOffBits;
  
  void read(Reader const& reader)
  {
      bfType = reader.read16();
      bfSize = reader.read32();
      bfReserved1 = reader.read16();
     	bfReserved2 = reader.read16();
      bfOffBits = reader.read32();
  }
};

struct BmpInfo {
  uint32_t  biSize;
  int32_t   biWidth;
  int32_t   biHeight;
  uint16_t   biPlanes;
  uint16_t   biBitCount;
  uint32_t  biCompression;
  uint32_t  biSizeImage;
  int32_t   biXPelsPerMeter;
  int32_t   biYPelsPerMeter;
  uint32_t  biClrUsed;
  uint32_t  biClrImportant;
  
  void read(Reader const& reader)
  {
     biSize = reader.read32();
     biWidth = reader.read32();
     biHeight = reader.read32();
     biPlanes = reader.read16();
     biBitCount = reader.read16();
     biCompression = reader.read32();
     biSizeImage = reader.read32();
     biXPelsPerMeter = reader.read32();
     biYPelsPerMeter = reader.read32();
     biClrUsed = reader.read32();
     biClrImportant = reader.read32();
  }
};

bool isBitSet(int num, int bit)
{
    return num & (1 << bit);
}

BITMAP* loadBmp(char const* path)
{
    BmpHeader header;
    BmpInfo info;
    int32_t pal[256] = {0};
    int w, h, bpp, colNum, size;

    FILE* in = fopen(path, "rb");
    CInStream inStream(in);
    Reader reader(inStream, LITTLE_ENDIAN);
    
    header.read(reader);
    info.read(reader);
    
    w = info.biWidth;
    h = info.biHeight;
    bpp = info.biBitCount;
    size = header.bfSize - header.bfOffBits;
    
    if(bpp < 16)
    {
        int colNum = 1 << bpp;
        reader.read32(pal, colNum);
    }
    
    else
       fseek(in, info.biClrUsed, SEEK_CUR);
    
    uint8_t* tempData = new uint8_t[size];
    fread(tempData, 1, size, in);
    
    int bpr = (w * bpp + 7) / 8;             // Bytes per row.
    int bpr4 = ((bpr + 3) / 4) * 4;          // Round up to nearest number divisible by 4.
    int padding = bpr4 - bpr;                // Padding bytes per row.
    
    BITMAP* bitmap = create_bitmap(w, h);
    uint8_t* offs = tempData;
    
    switch(bpp)
    {
       case 32:
       for(int j = h - 1; j >= 0; j--, offs += padding)
           for(int i = 0; i < w; i++, offs += 4)
           {
             int c = *(int*)offs;
             putpixel(bitmap, i, j, c);
           }
       break;
     
       case 24:
         for(int j = h - 1; j >= 0; j--, offs += padding)
           for(int i = 0; i < w; i++, offs += 3)
           {
             int c = (*(int*)offs) & 0xFFFFFF00;
             putpixel(bitmap, i, j, c);
           }
       break;
    
      case 8:
        for(int j = h - 1; j >= 0; j--, offs += padding)
          for(int i = 0; i < w; i++, offs++)
           putpixel(bitmap, i, j, pal[*offs]);
      break;
    
      case 4:
      for(int j = h - 1; j >= 0; j--, offs += padding)
        for(int i = 0; i < bpr; i++, offs++)
        { 
          int p1 = *offs >> 4;
          int p2 = *offs & 0xF;
          
          putpixel(bitmap, i * 2, j, pal[p1]);
          putpixel(bitmap, i * 2 + 1, j, pal[p2]);
        }
      break;

      case 1:
      for(int j = h - 1; j >= 0; j--, offs += padding)
        for(int i = 0; i < bpr; i++, offs++)
          for(int k = 0; k < 8; k++)
            putpixel(bitmap, i * 8 + k, j, pal[isBitSet(*offs, 7 - k)]);
      break;
      
      default:
      break;
    }

    
    delete[] tempData;
    fclose(in);
    
    return bitmap;
}

int main()
{   
    bool exit = false;
    
    init();
    
    BITMAP* bitmap = loadBmp("test.bmp");

    while(!exit)
    {
     if(key[KEY_ESC]) exit = true;
    
     clear_to_color(buffer, makecol(255, 255, 255));
   
     draw_sprite(buffer, bitmap, 0, 0);
   
     draw_sprite(buffer, mouse_sprite, mouse_x, mouse_y);
     blit(buffer, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
    }
    
    deinit();
    return 0;
}END_OF_MAIN()
