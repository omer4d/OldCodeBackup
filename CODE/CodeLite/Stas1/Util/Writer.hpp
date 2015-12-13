/* 
 * File:   Writer.hpp
 * Author: Stas
 *
 * Created on February 21, 2013, 9:29 AM
 */

#ifndef WRITER_HPP
#define	WRITER_HPP

#include "ByteOrder.hpp"
#include "Writable.hpp"

class Writer {
    Writable* out;
    bool reverseFlag;

public:

    Writer(void)
    {
        out = NULL;
    }

    Writer(Writable& out, ByteOrder destOrder = HOST_ORDER)
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

#endif	/* WRITER_HPP */

