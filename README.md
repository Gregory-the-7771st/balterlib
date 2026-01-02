# Balterlib
Balanced ternary types for C, written primarily with speed in mind.

This project was started with the plan of eventually making an emulator, but I'm currently quite burned out so I decided to very quickly convert it into a library, admittedly using some questionable coding practices along the way. I had to make a lot of changes for this to work and wasn't able to test it very much, so there may be some bugs or nonsensical comments. If you find something weird, or have any requests for wat i shoudl change or add, you should email me at hasan.05.7771@proton.me as I basically never check github.

I don't know how licenses work, but you're free to do whatever.

## Documentation:

There are 3 supported types:
- BT9: 9 trit integer. typedef of uint32_t
- BT18: 18 trit integer. typedef of uint64_t
- BT27: 27 trit integer. typedef of uint64_t

All macros and functions are prefixed by their type and an underscore. (i. e: BT9_add(...), BT27_USED_BITS, ...)
Unless specified otherwise, every macro and function listed here does this and will be listed without the type prefix.

### Macros

- USED_BITS: a mask that has every used bit set to 1. as an example, for a 9 trit integer, this mask will have the least significant 18 bits (1 trit is represented with 2 bits) set to 1, and everything else set to 0. This is largely an implementaion thing, but might be useful in some cases.
- n1: -1 in trit form.
- 0: 0 in trit form.
- 1: 1 in trit form.

### Functions

- neg(BTx a): Tritwise NOT operation (also equal to negation due to how balanced ternary works).
- and(BTx a, BTx b): Tritwise AND operation. Only included for consistency as this is precisely equal to bitwise AND(&).
- or(BTx a, BTx b): Tritwise or operation. Only included for consistency as this is precisely equal to bitwise OR(|).
- xor(BTx a, BTx b): Tritwise xor operation. In case you're thinking of a different extension of xor, this one defined as "(a | b) & BTx_neg(a & b)"
- carry(BTx a, BTx b): The carries produced from balanced ternary addition. sets the resulting trit to 1 if both values are 1, T if both values are T, and 0 otherwise.
- addNoCarry(BTx a, BTx b): The result from balanced ternary addition, but without accounting for carries.
- shift(BTx a, BTx b): shifts a by b amount of trits. if b is negative then it shifts to the right, if it is positive then it shifts to the left. note that if you already know the amount you'll be shifting by, it'll be much faster to use one of mul3, mul9, div3 or do (a << b) | 0b0101... (there should be as many 0b01's as the amount you're shifting by) (if you're shifitng right you should put 0b01's starting from the left of the used bits, for example for BT9 do (a >> 1) | 0b010000000000000000). 
- sign(BTx a): returns T (-1 in trit form) if the number is negative, 0 in trit form if the number is 0, and 1 in trit form if the number is positive
- abs(BTx a): returns the absolute value of the number.
- inc(BTx a): returns a + 1.
- dec(BTx a): returns a - 1.
- add(BTx a, BTx b): addition.
- sub(BTx a, BTx b): subtraction.
- mul2(BTx a): returns a * 2
- mul3(BTx a): returns a * 3, this is also equal to shifting left by 1
- mul9(BTx a): returns a * 2, this is also equal to shifting left by 2
- mul(BTx a, BTx b): multiplication.
- div2(BTx a): returns a / 2.
- div3(BTx a): returns a / 3, this is also equal to shifting right by 1
- div(BTx a, BTx b): integer division. Note that since converting to int before dividing and then converting back was faster, this is what the default implementation uses. If you want something that doesn't "cheat" like this, then use divPure.
- divPure(BTx a, BTx b): divison, but it doesn't "cheat" (as explained above)
- mod(BTx a, BTx b): symmetric modulo (what that means is that if a % b is bigger than half of b, then it returns a negative value (specifically b - (a asymmetric mod b))).
