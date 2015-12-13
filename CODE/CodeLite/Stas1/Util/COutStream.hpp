/* 
 * File:   COutStream.hpp
 * Author: Stas
 *
 * Created on February 21, 2013, 9:25 AM
 */

#ifndef COUTSTREAM_HPP
#define	COUTSTREAM_HPP

#include <stdio.h>

#include "Writable.hpp"

struct COutStream : public Writable {
    FILE* stream;

    COutStream()
    {
        this->stream = NULL;
    }

    COutStream(FILE * stream)
    {
        this->stream = stream;
    }

    size_t write(void const* src, size_t bytes)
    {
        return fwrite(src, 1, bytes, stream);
    }
};

#endif	/* COUTSTREAM_HPP */

