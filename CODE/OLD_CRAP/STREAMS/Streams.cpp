#include <stdio.h>
#include <conio.h>

#include <inttypes.h>

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

int main()
{
   FILE* stream = fopen("test.bin", "wb+");
   CInStream cInStream(stream);
   COutStream cOutStream(stream);
   
   Reader reader(cInStream, LITTLE_ENDIAN);
   Writer writer(cOutStream, LITTLE_ENDIAN);
   
   writer.write8(13);
   writer.write16(555);
   writer.write16(1234);
   writer.write32(789789);
   writer.write8(77);
   
   int8_t arr1[] = {1, 2, 3, 4, 5};
   int16_t arr2[] = {6, 7, 8, 9, 10};
   int32_t arr3[] = {11, 12, 13, 14, 15, 16};
   
   int8_t arr4[5];
   int16_t arr5[5];
   int32_t arr6[6];
   
   writer.write8(arr1, 5);
   writer.write16(arr2, 5);
   writer.write32(arr3, 6);
   
   rewind(stream);
   
   int a = reader.read8();
   int b = reader.read16();
   int c = reader.read16();
   int d = reader.read32();
   int e = reader.read8();
   
   reader.read8(arr4, 5);
   reader.read16(arr5, 5);
   printf("%d\n", reader.read32(arr6, 10));
   
   printf("%d %d %d %d %d", a, b, c, d, e);

   printf("\n");
   for(int i = 0; i < 5; i++)
     printf("%d ", arr4[i]);
   for(int i = 0; i < 5; i++)
     printf("%d ", arr5[i]);
   for(int i = 0; i < 6; i++)
     printf("%d ", arr6[i]);

   fclose(stream);

   getch();
   return 0;
}
