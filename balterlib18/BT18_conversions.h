// I didn't feel the need to hyper-optimize these since these aren't part of the calculations themselves

void BT18_toString(BT18 number, char* string) {
    for (int i = 0; i < 18; i++) { // loop through each trit
        trit currentTrit = (number >> ((18 - i - 1) * 2)) & 0b11; // i'm doing 18 - i - 1 because it needs to be reversed in order when in the string
        // match the trit to a character
        if (currentTrit == T) {
            string[i] = 'T';
        } else if (currentTrit == O) {
            string[i] = '0';
        } else if (currentTrit == I) {
            string[i] = '1';
        } else {
            string[i] = 'X'; // not a valid trit
        }
    }
    string[18] = '\0'; // can't forget that one
}

BT18 stringToBT18(char* string) {
    BT18 result = BT18_0; // set every trit to 0 (to make it so that leading zeroes don't have to be specified)
    for (int i = 0; i < BT_INTERNAL_strlen(string); i++) {
        if (string[i] == 'T') {
            // i have to this xor stuff instead of just ORing because of every trit being zero by default
            BT18 mask = (T ^ O) << ((BT_INTERNAL_strlen(string) - i - 1) * 2);
            result = result ^ mask;
            // don't have to check for zero since the result already contains a zero for all the trits by default
        } else if (string[i] == '1') {
            BT18 mask = (I ^ O) << ((BT_INTERNAL_strlen(string) - i - 1) * 2);
            result = result ^ mask;
        }
        // everything that isn't T or 1 gets turned to 0
    }
    return result;
}