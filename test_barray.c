#include "UArray2b.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main() {
    printf("hello world\n");

    UArray2b_T test_block = UArray2b_new(8, 8, 4, 4);
    int height = UArray2b_height(test_block);
    printf("printing height of 8: %d\n", height);

    return 0;
}
