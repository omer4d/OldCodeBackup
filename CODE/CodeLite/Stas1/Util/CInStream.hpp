/* 
 * File:   CInStream.hpp
 * Author: Stas
 *
 * Created on February 21, 2013, 9:25 AM
 */

#ifndef CINSTREAM_HPP
#define	CINSTREAM_HPP

#include <stdio.h>
#include "Readable.hpp"

struct CInStream : public Readable {
    FILE* stream;

    CInStream()
    {
        this->stream = NULL;
    }

    CInStream(FILE * stream)
    {
        this->stream = stream;
    }

    size_t read(void* dest, size_t bytes)
    {
        return fread(dest, 1, bytes, stream);
    }
};

#endif	/* CINSTREAM_HPP */

