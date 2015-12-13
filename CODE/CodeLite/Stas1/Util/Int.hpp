/* 
 * File:   Int.hpp
 * Author: Stas
 *
 * Created on February 27, 2013, 1:04 AM
 */

#ifndef INT_HPP
#define	INT_HPP

#include <cstdint>

// 8-bit

#ifndef INT8_MIN
#define INT8_MIN -128
#endif

#ifndef INT8_MAX
#define INT8_MAX 127
#endif

#ifndef UINT8_MAX
#define UINT8_MAX 255
#endif

// 16-bit

#ifndef INT16_MIN
#define INT16_MIN -32768
#endif

#ifndef INT16_MAX
#define INT16_MAX 32767
#endif

#ifndef UINT16_MAX
#define UINT16_MAX 65535
#endif

// 32-bit

#ifndef INT32_MIN
#define INT32_MIN (-2147483647 - 1)
#endif

#ifndef INT32_MAX
#define INT32_MAX 2147483647
#endif

#ifndef UINT32_MAX
#define UINT32_MAX 4294967295U
#endif

#endif	/* INT_HPP */

