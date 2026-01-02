#include <stdint.h>

#ifndef BT_COMMON_CODE_IS_INCLUDED

// needed for BTx_conversions.h
// so i don't have to include the entirety of string.h
int BT_INTERNAL_strlen(char* string) {
    int i = 0;
    while (string[i] != '\0') {
        i++;
    }
    return i;
}

// how this library stores trits:
typedef enum {
    T = 0b00, // -1 (T)
    O = 0b01, // 0
    I = 0b11 // 1
} trit;
// storing it as 0b00, 0b01, but 0b11 instead of 0b10 makes logic easier
// these are packed into the BT_int types

int64_t powersOf3[] = {
    // only including up to 3 ^ 26
    1, 3, 9, 27, 81, 243, 729, 2187, 6561,
    19683, 59049, 177147, 531441, 1594323,
    4782969, 14348907, 43046721, 129140163,
    387420489, 1162261467, 3486784401, 
    10460353203ULL, 31381059609ULL,
    94143178827ULL, 282429536481ULL,
    847288609443ULL, 2541865828329ULL
};

#define BT_COMMON_CODE_IS_INCLUDED
#endif

#include "balterlib18/BT18_logic.h"
#include "balterlib18/BT18_operations.h"
#include "balterlib18/BT18_conversions.h"