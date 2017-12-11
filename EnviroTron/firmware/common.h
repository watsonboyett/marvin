#pragma once

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#ifndef min_val
#define min_val(a,b) ((a)<(b)?(a):(b))
#endif
#ifndef max_val
#define max_val(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef abs_val
#define abs_val(x) ((x)>0?(x):-(x))
#endif
#define constrain(val,low,high) ((val)<(low)?(low):((val)>(high)?(high):(val)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define bit(b) (1UL << (b))
#define bit_read(value, bit) (((value) >> (bit)) & 0x01)
#define bit_set(value, bit) ((value) |= (1UL << (bit)))
#define bit_clear(value, bit) ((value) &= ~(1UL << (bit)))
#define bit_write(value, bit, bitvalue) (bitvalue ? bit_set(value, bit) : bit_clear(value, bit))
#define byte_low(w) ((uint8_t) ((w) & 0xff))
#define byte_high(w) ((uint8_t) ((w) >> 8))


#ifdef	__cplusplus
}
#endif

char * convert_to_binary_string(unsigned int val, char *buff, int sz);

float calc_exponential_avg(float current_value, float new_value, float weight);

