// they say that good code does not need comments as it explains itself
// turns out good code is way too slow so prepare to read a whole book's worth of comments
// i hope the comments are understable, at least

// each trit is 2 bits, so 9 trits can fit in 32 bits
typedef uint32_t BT9;

#define BT9_USED_BITS 0x0003FFFF
#define BT9_PATTERN01 0x00015555 // 0b01010101... (first 2 bits aren't set since they aren't used)
#define BT9_PATTERN10 0x0002AAAA // 0b10101010... (first 2 bits aren't set since they aren't used)
#define BT9_n1 0x00015554
#define BT9_0 BT9_PATTERN01 // because 0 is 0b01 in my encoding
#define BT9_1 0x00015557

// used in SHIFT(...) and division
int BT9_toInt(BT9 number) {
    int result = 0;
    for (int i = 0; i < 9; i++) { // loop through each trit
        // int currentTrit = (word >> (i * 2)) & 0b11; // get the i'th trit (inlined version of extractTrit(...))
        // (further inlined to the "result +=" line)
        
        // this is what a normal person would do
        /*if (currentTrit == T) {
            result -= powersOf3[i];
        } else if (currentTrit == I) {
            result += powersOf3[i];
        }*/
        
        // but this is faster
        result += (((number >> (i * 2)) & 0b11) - ((number >> (i * 2 + 1)) & 0b1) - 1) * powersOf3[i];
        // (((word >> (i * 2)) & 0b11) - ((word >> (i * 2 + 1)) & 0b1) - 1) outputs 1 for a balanced ternary 1 and -1 for a balanced ternary T 
        // it just kinda does it, i don't know what i can say
    }
    return result;
}


// used in division
BT9 intToBT9(int a) {
    // if it's a value bigger than what a 9 trit BT number can hold, emulate overflow
    if (a > powersOf3[8]) {
        a = a % powersOf3[8];
    }
    
    BT9 result = 0;
    
    for (int i = 8; i >= 0; i--) { // loop through each trit (most to least significant trit, the order is important)
        result = (result << 2) | O; // shift left by 1 trit, then put 0 in the least significant trit spot
        if (a > powersOf3[i] >> 1 /* floored division by 2 */) { // check if a is bigger than the current power of 3 divided by 2
            result = result | 0b10; // turn least significant trit to 1
            a -= powersOf3[i]; // adjust a for next loop iteration
        } else if (a < -(powersOf3[i] >> 1) /* floored division by 2 */) { // check if a is smaller than the current power of 3 divided by 2 but in negative
            result = result ^ 0b01; // turn least significant trit to T
            a += powersOf3[i]; // adjust a for next loop iteration
        }
    }
    return result;
}


// all the logic functions from here on out are implemented with bitwise operations
// this essentially allows it to process all the trits in parallel
// which means it's very fast 

// NOTE: when you see "& PATTERN01" or "& PATTERN10", a useful way to think about it is that
// it is zeroing out the least/most significant bit of each trit
// what THAT means is that, for example, & PATTERN10 outputs 0b10 wherever there's a 0b11 (balanced ternary 1)
// when it's shifted right by 1, that becomes 0b01 which is useful for a lot of things, as you'll see if you keep reading

// tritwise NOT
// also equal to negation since T (-1) becomes 1 and 1 becomes T (-1)
BT9 BT9_neg(BT9 a) {
    return ((~a & BT9_PATTERN10) >> 1) | ((~a << 1) & BT9_PATTERN10);
    
    // truth table in balanced ternary:
    /*
      T --> 1
      0 --> 0
      1 --> T
    */
    
    // truth table in binary:
    /*
      00 --> 11
      01 --> 01
      11 --> 00
    */
    
    // what it does is essentially taking the bitwise not, and swapping the bits of each trit
    // which kinda just happens to be correct tbh
}

BT9 BT9_and(BT9 a, BT9 b) {
    return a & b; // works because 0b11's set bits completely cover the set bits of 0b01 
    // and the same goes for 0b01 and 0b00
    
    // truth table in balanced ternary:
    /*
          T  0  1
       T  T  T  T
       0  T  0  0
       1  T  0  1
    */
    
    // truth table in binary:
    /*
          00 01 11
      00  00 00 00
      01  00 01 01
      11  00 01 11
    */
}

BT9 BT9_or(BT9 a, BT9 b) {
    return a | b; // works because 0b11's set bits completely cover the set bits of 0b01 
    // and the same goes for 0b01 and 0b00
    
    // truth table in balanced ternary:
    /*
          T  0  1
       T  T  0  1
       0  0  0  1
       1  1  1  1
    */
    
    // truth table in binary:
    /*
          00 01 11
      00  00 01 11
      01  01 01 11
      11  11 11 11
    */
}

// this is just an equation for xor, no special tricks on this one
// a ^ b = (a | b) & ~(a & b) in binary aswell
BT9 BT9_xor(BT9 a, BT9 b) {
    return (a | b) & BT9_neg(a & b);
}

// unlike in binary, the carry from addition is not just equal to a & b
// (neither this function nor the addition with no carry function are actually used in addition)
// (addition is in operations.h if you want to check it out)
BT9 BT9_carry(BT9 a, BT9 b) {
    return (a & b) | ((a | b) & BT9_PATTERN01);
    
    // truth table in balanced ternary:
    /*
          T  0  1
       T  T  0  0
       0  0  0  0
       1  0  0  1
    */
    
    // truth table in binary:
    /*
          00 01 11
      00  00 01 01
      01  01 01 01
      11  01 01 11
    */
    
    // if you look really hard, you might notice that the leading bit of the result is actualy the AND of the leading bits
    // and the least significant bit is the OR of the least significant bits
    // so that's basically what i do, by zeroing out the leading bit after taking the bitwise OR
    // i don't have to zero out the least significant bit after the AND because if a & b = 1 then so does a | b
}

// not equivelant to xor in balanced ternary
// this is not used in the code itself
BT9 BT9_addNoCarry(BT9 a, BT9 b) {
    return (((~a & (~b & BT9_PATTERN01)) << 1) | (~(a ^ (a >> 1)) & (~b & BT9_PATTERN01))) | (((~a & (b & BT9_PATTERN10)) >> 1) | (((a << 1) ^ a) & (b & BT9_PATTERN10))) | (a & (((b ^ (b >> 1)) & BT9_PATTERN01) | (((b ^ (b >> 1)) & BT9_PATTERN01) << 1)));
    
    // truth table in balanced ternary:
      /*    
            T  0  1
         T  1  T  0
         0  T  0  1
         1  0  1  T
      */
      
    // truth table in binary:
      /*
            00 01 11
         00 11 00 01
         01 00 01 11
         11 01 11 00
      */
      
      // i don't remember how i did this one, sorry
}

// shift a by b trits, since this is balanced ternary there can be negative numbers to indicate direction
BT9 BT9_shift(BT9 a, BT9 b) {
    int intB = BT9_toInt(b);
    if (intB > 0) { // shift left by intB (* 2 because each trit is 2 bits)
        return (a << intB * 2) | /* have to fill new trits with zeroes */ (((1 << (2 * intB)) - 1) & BT9_PATTERN01);
    } else { // shift right by intB (* 2 because each trit is 2 bits) (and -intB because it's negative otherwise)
        return (a >> -intB * 2) | /* have to fill new trits with zeroes */ (~((1 << (2 * (intB + 9))) - 1) & BT9_PATTERN01);
    }
}

// this is quite simple i think
BT9 BT9_sign(BT9 a) {
    // because 1 > 0 > T in binary with the encoding too, comparisons like > and < work correctly
    if (a > BT9_0) {
        return I; // balanced ternary 1
    } else if (a < BT9_0) {
        return T; // balanced ternary -1
    } else {
        return O; // balanced ternary 0
    }
}

// absolute value
BT9 BT9_abs(BT9 a) {
    // as you'd expect
    if (a < BT9_0) return BT9_neg(a);
    return a;
}