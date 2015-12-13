/* 
 * File:   ByteOrder.hpp
 * Author: Stas
 *
 * Created on February 21, 2013, 9:26 AM
 */

#ifndef BYTEORDER_HPP
#define	BYTEORDER_HPP

#include <inttypes.h>

enum ByteOrder {
    BIG_ENDIAN = 0, LITTLE_ENDIAN = 1, NETWORK_ORDER = 0, HOST_ORDER = 2
};

ByteOrder getHostByteOrder()
{
    int n = 1;
    return *(char *) &n == 1 ? LITTLE_ENDIAN : BIG_ENDIAN;
}

int16_t rev16(int16_t n)
{
    char* byte = (char*) &n;
    int8_t tmp = byte[0];

    byte[0] = byte[1];
    byte[1] = tmp;

    return n;
}

int32_t rev32(int32_t n)
{
    char* byte = (char*) &n;
    int8_t tmp = byte[0];

    byte[0] = byte[3];
    byte[3] = tmp;
    tmp = byte[1];
    byte[1] = byte[2];
    byte[2] = tmp;
    
    return n;
}

#endif	/* BYTEORDER_HPP */

