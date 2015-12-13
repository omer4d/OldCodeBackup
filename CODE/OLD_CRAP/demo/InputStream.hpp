/* 
 * File:   InputStream.hpp
 * Author: Stas
 *
 * Created on January 24, 2012, 4:59 PM
 */

#ifndef INPUTSTREAM_HPP
#define	INPUTSTREAM_HPP

#include <istream>

#define BIG_ENDIAN_SYSTEM false

class InputStream {
    bool bigEndian;
    std::istream* stream;

    void swapBytes(char& a, char& b)
    {
        char c = a;
        a = b;
        b = c;
    }

public:

    InputStream(std::istream& stream, bool bigEndian)
    {
        this->stream = &stream;
        this->bigEndian = bigEndian;
    }
    
    std::istream& istream()
    {
        return *stream;
    }

    void read(void* buff, int size)
    {
        char* bytes = (char*) buff;
        stream->read(bytes, size);

        if(bigEndian != BIG_ENDIAN_SYSTEM)
            for(int i = 0; i < size / 2; i++)
                swapBytes(bytes[i], bytes[size - 1 - i]);
    }
    
    void readNaive(void* buff, int size)
    {
        stream->read((char*)buff, size);
    }
};


#endif	/* INPUTSTREAM_HPP */

