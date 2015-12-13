/* 
 * File:   Reader.hpp
 * Author: Stas
 *
 * Created on February 21, 2013, 9:28 AM
 */

#ifndef READER_HPP
#define	READER_HPP

#include "ByteOrder.hpp"
#include "Readable.hpp"

class Reader {
    Readable* in;
    bool reverseFlag;

public:

    Reader(void)
    {
        in = NULL;
    }

    Reader(Readable& in, ByteOrder srcOrder = HOST_ORDER)
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

#endif	/* READER_HPP */

