#include "uarray2b.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main() {
    printf("hello world\n");

    UArray2b_T test_block = UArray2b_new(3, 3, 4, 2);
    int height = UArray2b_height(test_block);
    printf("printing height of 8: %d\n", height);

    *((int *)UArray2b_at(test_block, 0, 0)) = 5;
    *((int *)UArray2b_at(test_block, 2, 0)) = 21;

    printf("testing test_block %d\n", *((int *)UArray2b_at(test_block, 2,0)));

    UArray2b_free(&test_block);


    return 0;
}
