// increment
BT27 BT27_inc(BT27 a) {
    return (a + 1) | (((a + 1) >> 1) & BT27_PATTERN01); // add 1, then turn 0b10 into 0b11

    // the result of a + 1 (binary):
    /*
      00 --> 01 (T correctly becomes 0)
      01 --> 10 (becomes an invalid trit, supposed to be 0b11 which is a balanced ternary 1)
      11 --> 00 with a carry of 01 (correctly becomes T and carries)
    */
    // what "| (((a + 1) >> 1) & BT27_PATTERN01)" does is it turns the invalid 0b10 trit to 0b11
    // by basically bitwise OR'ing the leading bit with the least significant bit and assigning that to the least significant bit
    // this works because 0b11 is the only trit that has a 1 as the leading bit and it also has a 1 as the least significant bit
    // so 0b10 becomes 0b11 without affecting anything else
}

// decrement
BT27 BT27_dec(BT27 a) {
    a--; // the result from this ends up being  almost correct
    // in binary:
    /*
      00 (T) --> 11 (1) with borrow (correct)
      01 (0) --> 00 (T) (correct)
      11 (1) --> 10 (invalid) (incorrect)
    */
    
    // this last part turns 0b10 to 0b01 (0) 
    return (((a >> 1) | a) & BT27_PATTERN01) | (((a << 1) & a) & BT27_PATTERN10);
    // the first (((a >> 1) | a) & BT27_PATTERN01) part basically turns on the first bit if any bits are on
    // because the only valid trit with the first bit off is T (00) and that also has the leading bit off
    // and then the second part turns on the leading bit only if both bits are on
    // because the only valid trit with the leading bit on is 1 (11) and that also has the least significant bit on
}

BT27 BT27_add(BT27 a, BT27 b) {
    // these were faster to just find and replace instead
    // turn each 0b11 (trit 1) into 01:
    // ((b >> 1) &BT27_PATTERN01); 
    // turn each 0b00 (trit T) into 01:
    // ~b &BT27_PATTERN01;
    
    // this can be seperated into 2 parts:
    a = ((a + ((b >> 1) & BT27_PATTERN01)) | (((a + ((b >> 1) & BT27_PATTERN01)) >> 1) & BT27_PATTERN01)) - (~b & BT27_PATTERN01);
    //  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ~~~~~~~~~~~~~~~~~
    // the first one is very similar to increment but it adds 1 at every trit of b that is 0b11 (trit 1)
    // everything works the same way, including turning 0b10 to 0b11
    
    // the second one is doing regular subtraction but instead of subtracting 1, it subtracts 1 at every trit of b that is T
    // this is subtracted and saved into 'a' without turning 0b10 to 0b01 right away because
    // the logic to do that (in the return statement) uses a 4 times, so it's faster to save it than recalculate it 4 times
    
    // as noted in the decrement function, this turns 0b10 to 0b01 to give the correct result
    return (((a >> 1) | a) & BT27_PATTERN01) | (((a << 1) & a) & BT27_PATTERN10);
}

// subtract
BT27 BT27_sub(BT27 a, BT27 b) {
    // surprisingly similar to addition, but instead of adding 0b01 at every 1 in b, it adds 0b01 at every T (and subtracts 0b01 at every 0b11)
    // so basically just acts as if b is flipped
    // see addition above for more details
    a = ((a + (~b & BT27_PATTERN01)) | (((a + (~b & BT27_PATTERN01)) >> 1) & BT27_PATTERN01)) - ((b >> 1) & BT27_PATTERN01);
    
    // and this is the exact same logic to turn 0b10 into 0b01
    return (((a >> 1) | a) & BT27_PATTERN01) | (((a << 1) & a) & BT27_PATTERN10);
}

// multiply by 2
// really optimized version of a + a
BT27 BT27_mul2(BT27 a) {
    // can also be split into 2 parts
    a += ((a & BT27_PATTERN10) >> 1) - (~a & BT27_PATTERN01);
    //   ~~~~~~~~~~~~~~~~~~~~~~   ~~~~~~~~~~~~~~~~
    // they have the same purpose, the first part turns 1 to 0b01
    // and the second part turns T to 0b01
    // it can just be a lot simpler for obvious reasons
    
    // turn 0b10 to 0b11
    return a | ((a >> 1) & BT27_PATTERN01);
    // it turns the least significant bit to 1 if the leading bit is 1
    // works because 0b11 is the only trit with the leading bit as 1 and it also has the least significant bit as 1
}

// multiply by 3
BT27 BT27_mul3(BT27 a) {
    // very simple,
    // shift by 2 (since 1 trit is 2 bits)
    // and then set the least significant trit to 0, since 0b00 (what it'd be set to normally) is T
    return a << 2 | 0b01;
}

// multiply by 9
BT27 BT27_mul9(BT27 a) {
    // same as above but shift by 2 trits instead of 1
    return a << 4 | 0b0101;
}

// Douglas W. Jones' code was a serious help in this function
// and also minor amounts elsewhere in this project
// shoutout to him
// here's his multiplication
// https://homepage.cs.uiowa.edu/~jones/ternary/multiply.shtml
BT27 BT27_mul(BT27 a, BT27 b) {
    BT27 result = 0; // accumulates the result
    while (a != 0) {
        // multiplying 3 trits at a time
        // it's horrible code (holy tower of switch) but significantly faster
        // because it composes multiplication by using constants with faster multiplication algorithms
        switch (a & 0b111111 /*least significant 3 trits*/) {
            case 0b000000: // -13
                result |= BT27_neg(BT27_add(BT27_mul9(b), BT27_add(BT27_mul3(b), b)));
                // -(((9 * b) + (3 * b) + b)
                break;
            
            case 0b000001: // -12
                result |= BT27_neg(BT27_add(BT27_mul9(b), BT27_mul3(b)));
                // -((9 * b) + (3 * b))
                break;
            
            case 0b000011: // -11
                result |= BT27_neg(BT27_add(BT27_mul3(b), BT27_mul2(b)));
                // -((9 * b) + (2 * b))
                break;
            
            case 0b000100: // -10
                result |= BT27_neg(BT27_add(BT27_mul9(b), b));
                // -((9 * b) + b)
                break;
            
            case 0b000101: // -9
                result |= BT27_neg(BT27_mul9(b));
                // -(9 * b)
                break;
            
            case 0b000111: // -8
                result |= BT27_neg(BT27_sub(BT27_mul9(b), b));
                // -((9 * b) - b)
                break;
            
            case 0b001100: // -7
                result |= BT27_neg(BT27_add(BT27_mul2(BT27_mul3(b)), b));
                // -((2 * 3 * b) + b)
                break;
            
            case 0b001101: // -6
                result |= BT27_neg(BT27_mul2(BT27_mul3(b)));
                // -(2 * 3 * b)
                break;
            
            case 0b001111: // -5
                result |= BT27_neg(BT27_add(BT27_mul3(b), BT27_mul2(b)));
                // -((3 * b) + (2 * b))
                break;
            
            case 0b010000: // -4
                result |= BT27_neg(BT27_mul2(BT27_mul2(b)));
                // -(2 * (2 * b))
                break;
            
            case 0b010001: // -3
                result |= BT27_neg(BT27_mul3(b));
                // -(3 * b)
                break;
            
            case 0b010011: // -2
                result |= BT27_neg(BT27_mul2(b));
                // -(2 * b)
                break;
                
            case 0b010100: // -1
                result |= BT27_neg(b);
                // -b
                break;
                
            /*case 0b010101: // 0
                // do nothing...
                break;*/
                
            case 0b010111: // 1
                result |= b;
                // b
                break;
            
            case 0b011100: // 2
                result |= BT27_mul2(b);
                // 2 * b
                break;
                
             case 0b011101: // 3
                result |= BT27_mul3(b);
                // 3 * b
                break;
                
            case 0b011111: // 4
                result |= BT27_add(BT27_mul3(b), b);
                // (3 * b) + b
                break;
                
            case 0b110000: // 5
                result |= BT27_mul2(BT27_mul2(b));
                // 2 * (2 * b)
                break;
                
            case 0b110001: // 6
                result |= BT27_mul2(BT27_mul3(b));
                // 2 * (3 * b)
                break;
                
            case 0b110011: // 7
                result |= BT27_add(BT27_mul2(BT27_mul3(b)), b);
                // (2 * (3 * b)) + b
                break;
            
            case 0b110100: // 8
                result |= BT27_sub(BT27_mul9(b), b);
                // (9 * b) - b
                break;
                
            case 0b110101: // 9
                result |= BT27_mul9(b);
                // 9 * b
                break;
                
            case 0b110111: // 10
                result |= BT27_add(BT27_mul9(b), b);
                // (9 * b) + b
                break;
                
            case 0b111100: // 11
                result |= BT27_add(BT27_mul9(b), BT27_mul2(b));
                // (9 * b) + (2 * b)
                break;
                
            case 0b111101: // 12
                result |= BT27_add(BT27_mul9(b), BT27_mul3(b));
                // (9 * b) + (3 * b)
                break;
                
            case 0b111111: // 13
                result |= BT27_add(BT27_mul9(b), BT27_add(BT27_mul3(b), b));
                // (9 * b) + ((3 * b) + b)
                break;
        }
        // shift to do next round of trits
        a >>= 6;
        b <<= 6;
    }
    return result;
}

// basically copied from jones
// same link as above
// https://homepage.cs.uiowa.edu/~jones/ternary/multiply.shtml
BT27 BT27_div2(BT27 dividend) {
    // works because 1/2 = 0.111111... in balanced ternary
    // don't have to do each trit one by one since 0.1111... = 0.1010 + (0.1010 >> 1)
    BT27 result = dividend;
    result = BT27_add(result, result >> 2 /*shift by 1 trit*/ | 0x10000000000000 /* fill gaps with 0s after shifting*/ );
    result = BT27_add(result, result >> 4 /*2 trits*/ | 0x14000000000000 /* fill gaps with 0s after shifting*/);
    result = BT27_add(result, result >> 8 /*4 trits*/ | 0x15400000000000 /* fill gaps with 0s after shifting*/);
    result = BT27_add(result, result >> 16 /*8 trits*/ | 0x15554000000000 /* fill gaps with 0s after shifting*/);
    result = BT27_add(result, result >> 32 /*16 trits*/ | 0x15555555400000 /* fill gaps with 0s after shifting*/);
    return result >> 2 | 0x10000000000000 /* fill gaps with 0s after shifting*/; // divide by 3 to move the radix point to the right by 1 (the result would be multiplied by 1.1111 otherwise)
}

BT27 BT27_div3(BT27 dividend) {
    return (dividend >> 2) | 0x10000000000000;
}

BT27 BT27_div(BT27 dividend, BT27 divisor) {
    // turns out it's faster to just convert to int, divide, then convert back
    return intToBT27((BT27_toInt(dividend)) / BT27_toInt(divisor));
}

// used in the pure version of division
BT27 BT27_INTERNAL_closestToZero(BT27 a, BT27 b, BT27 c) {
    if (BT27_abs(a) < BT27_abs(b)) {
        if (BT27_abs(a) < BT27_abs(c)) {
            return a;
        } else {
            return c;
        }
    } else {
        if (BT27_abs(b) < BT27_abs(c)) {
            return b;
        } else {
            return c;
        }
    }
}

// i shall once again direct you to the holy scripture
// https://homepage.cs.uiowa.edu/~jones/ternary/multiply.shtml
BT27 BT27_divPure(BT27 dividend, BT27 divisor) {
    BT27 quotient = dividend;
    BT27 remainder = BT27_0;
    BT27 one = BT27_1;
    
    // take absolute value of divisor
    if (divisor < BT27_0) {
        divisor = BT27_neg(divisor);
        one = BT27_n1; // -1
    }
    
    for (int i = 0; i < 27; i++) {
        // first, shift the remainder and the quotient left
        // and feed the most significant bit of the quotient to the least significant bit of the remainder
        remainder = (remainder << 2) & BT27_USED_BITS;
        remainder |= (quotient >> 52) & 0b11;
        quotient = (quotient << 2 | O) & BT27_USED_BITS;
        
        // not really sure how it works beyond here, i basically copied this from douglas
        if (remainder > BT27_0) {
            BT27 low = BT27_sub(remainder, divisor);
            if ((BT27_neg(low) < remainder) || ((BT27_neg(low) == remainder) && (quotient > BT27_0))) {
                quotient = BT27_add(quotient, one);
                remainder = low;
            }
        } else if (remainder < BT27_0) {
            BT27 high = BT27_add(remainder, divisor);
            if ((BT27_neg(high) < remainder) || ((BT27_neg(high) == remainder) && (quotient > BT27_0))) {
                quotient = BT27_sub(quotient, one);
                remainder = high;
            }
        }
    }
    return quotient;
}

// the "pure" implementation of modulo is actually faster than conversion, as it turns out
// same as above, just return the reminder instead of quotient
BT27 BT27_mod(BT27 dividend, BT27 divisor) {
    BT27 quotient = dividend;
    BT27 remainder = BT27_0;
    BT27 one = BT27_1;
    
    // take absolute value of divisor
    if (divisor < BT27_0) {
        divisor = BT27_neg(divisor);
        one = BT27_n1; // -1
    }
    
    for (int i = 0; i < 27; i++) {
        // first, shift the remainder and the quotient left
        // and feed the most significant bit of the quotient to the least significant bit of the remainder
        remainder = (remainder << 2) & BT27_USED_BITS;
        remainder |= (quotient >> 52) & 0b11;
        quotient = (quotient << 2 | O) & BT27_USED_BITS;
        
        // not really sure how it works beyond here, i basically copied this from douglas
        if (remainder > BT27_0) {
            BT27 low = BT27_sub(remainder, divisor);
            if ((BT27_neg(low) < remainder) || ((BT27_neg(low) == remainder) && (quotient > BT27_0))) {
                quotient = BT27_add(quotient, one);
                remainder = low;
            }
        } else if (remainder < BT27_0) {
            BT27 high = BT27_add(remainder, divisor);
            if ((BT27_neg(high) < remainder) || ((BT27_neg(high) == remainder) && (quotient > BT27_0))) {
                quotient = BT27_sub(quotient, one);
                remainder = high;
            }
        }
    }
    return remainder;
}