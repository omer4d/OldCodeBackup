#include "ImageLoader.hpp"

#include "Reader.hpp"
#include "CInStream.hpp"

struct BmpHeader {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
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
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;

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

Buffer<int32_t>* loadBmp(char const* path)
{
    BmpHeader header;
    BmpInfo info;
    int32_t pal[256] = {0};
    int w, h, bpp, size;

    FILE* in = fopen(path, "rb");
    CInStream inStream(in);
    Reader reader(inStream, LITTLE_ENDIAN);

    if(!in)
        return NULL;
        
    header.read(reader);

    if(header.bfType != 19778)
        return NULL;
    
    info.read(reader);
    w = info.biWidth;
    h = info.biHeight;
    bpp = info.biBitCount;
    size = header.bfSize - header.bfOffBits;

    if(bpp != 1 && bpp != 4 && bpp != 8 && bpp != 24 && bpp != 32)
        return NULL;

    if(bpp < 16)
    {
        int colNum = 1 << bpp;
        reader.read32(pal, colNum);
    }

    else
        fseek(in, info.biClrUsed * 4, SEEK_CUR);

    uint8_t* tempData = new uint8_t[size];
    fread(tempData, 1, size, in);

    int bpr = (w * bpp + 7) / 8; // Bytes per row.
    int bpr4 = ((bpr + 3) / 4) * 4; // Round up to nearest number divisible by 4.
    int padding = bpr4 - bpr; // Padding bytes per row.
    uint8_t* offs = tempData;

    Buffer<int32_t>* buffer = new Buffer<int32_t> (w, h);

    switch(bpp)
    {
        case 32:
            for(int j = h - 1; j >= 0; j--, offs += padding)
                for(int i = 0; i < w; i++, offs += 4)
                {
                    int c = *(int*) offs;
                    buffer->at(i, j) = c;
                }
            break;

        case 24:
            for(int j = h - 1; j >= 0; j--, offs += padding)
                for(int i = 0; i < w; i++, offs += 3)
                {
                    int c = (*(int*) offs) & 0x00FFFFFF;
                    buffer->at(i, j) = c;
                }
            break;

        case 8:
            for(int j = h - 1; j >= 0; j--, offs += padding)
                for(int i = 0; i < w; i++, offs++)
                    buffer->at(i, j) = pal[*offs];
            break;

        case 4:
            for(int j = h - 1; j >= 0; j--, offs += padding)
                for(int i = 0; i < bpr; i++, offs++)
                {
                    int p1 = *offs >> 4;
                    int p2 = *offs & 0xF;

                    buffer->at(i * 2, j) = pal[p1];
                    buffer->at(i * 2 + 1, j) = pal[p2];
                }
            break;

        case 1:
            for(int j = h - 1; j >= 0; j--, offs += padding)
                for(int i = 0; i < bpr; i++, offs++)
                    for(int k = 0; k < 8; k++)
                        buffer->at(i * 8 + k, j) = pal[isBitSet(*offs, 7 - k)];
            break;

        default:
            break;
    }

    delete[] tempData;
    fclose(in);

    return buffer;
}

Buffer<int32_t>* ImageLoader::load(char const* path)
{
    return loadBmp(path);
}