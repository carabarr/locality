#include "uarray2b.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void print_bArray(UArray2b_T block);

void
check_and_print(int i, int j, UArray2b_T block, void *p1, void *p2);
void check_map(int col, int row, UArray2b_T block, void *elem, void *cl);

int main() {

    // UArray2b_T test_block = UArray2b_new(4, 3, 4, 3);

    UArray2b_T test_block = UArray2b_new(4, 3, 4, 2);

    // *((int *)UArray2b_at(test_block, 0, 0)) = 1;
    // *((int *)UArray2b_at(test_block, 0, 1)) = 2;
    // *((int *)UArray2b_at(test_block, 0, 2)) = 3;
    //
    // *((int *)UArray2b_at(test_block, 1, 0)) = 5;
    // *((int *)UArray2b_at(test_block, 1, 1)) = 6;
    // *((int *)UArray2b_at(test_block, 1, 2)) = 7;
    //
    // *((int *)UArray2b_at(test_block, 2, 0)) = 9;
    // *((int *)UArray2b_at(test_block, 2, 1)) = 10;
    // *((int *)UArray2b_at(test_block, 2, 2)) = 11;
    //
    // *((int *)UArray2b_at(test_block, 3, 0)) = 12;
    // *((int *)UArray2b_at(test_block, 3, 1)) = 13;
    // *((int *)UArray2b_at(test_block, 3, 2)) = 14;
    // int num = 0;
    //
    // UArray2b_map(test_block, check_map, &num);
    // printf("\n" );
    // print_bArray(test_block);

    UArray2b_free(&test_block);


    return 0;
}



void check_map(int col, int row, UArray2b_T block, void *elem, void *cl)
{
    (void)cl;
    // (void)elem;
    (void)block;
    (void)col;
    (void)row;

    // printf("\n");
    printf("elem %d\n", *(int *)elem);

}

void print_bArray(UArray2b_T block)
{
    for(int col = 0; col < UArray2b_width(block); col++)
    {
        for(int row = 0; row < UArray2b_height(block); row++)
        {
            printf("%d ",  *((int *)UArray2b_at(block, col,row)));

        }
        printf("\n");
    }
}
