#include "uarray2b.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void print_bArray(UArray2b_T block);

void
check_and_print(int i, int j, UArray2b_T block, void *p1, void *p2);
void pop_block(int col, int row, UArray2b_T block, void *elem, void *cl);
void print_pos(int col, int row, UArray2b_T block, void *elem, void *cl);

int main() {
    printf("remote sync?\n");

    UArray2b_T test_block = UArray2b_new(3, 3, 4, 2);
    int height = UArray2b_height(test_block);
    printf("printing blocksize of 128: %d\n", UArray2b_blocksize(test_block));
    //
    *((int *)UArray2b_at(test_block, 0, 0)) = 1;
    *((int *)UArray2b_at(test_block, 0, 1)) = 2;
    *((int *)UArray2b_at(test_block, 0, 2)) = 3;
    // *((int *)UArray2b_at(test_block, 0, 3)) = 4;
    *((int *)UArray2b_at(test_block, 1, 0)) = 5;
    *((int *)UArray2b_at(test_block, 1, 1)) = 6;
    *((int *)UArray2b_at(test_block, 1, 2)) = 7;
    // *((int *)UArray2b_at(test_block, 1, 3)) = 8;
    *((int *)UArray2b_at(test_block, 2, 0)) = 9;
    *((int *)UArray2b_at(test_block, 2, 1)) = 10;
    *((int *)UArray2b_at(test_block, 2, 2)) = 11;
    // *((int *)UArray2b_at(test_block, 2, 3)) = 12;
    // *((int *)UArray2b_at(test_block, 3, 0)) = 13;
    // *((int *)UArray2b_at(test_block, 3, 1)) = 14;
    // *((int *)UArray2b_at(test_block, 3, 2)) = 15;
    // *((int *)UArray2b_at(test_block, 3, 3)) = 16;
    // *((int *)UArray2b_at(test_block, 2, 0)) = 17;
    // int arr[9] = {5, 10, 39, 59, 8, 1, 3, 5, 100}
    int num = 0;


    UArray2b_map(test_block, print_pos, &num);
    printf("\n" );
    print_bArray(test_block);




    // printf("testing test_block %d\n", *((int *)UArray2b_at(test_block, 0,0)));

    UArray2b_free(&test_block);


    return 0;
}


void pop_block(int col, int row, UArray2b_T block, void *elem, void *cl)
{
    //PRINT COL ROW VISITED INSTEAD OF TRYING TO FUCK AROUND WITH CL
    (void)row;
    (void)col;
    (void)block;
    // int *num_ptr = cl;
    int num = *(int *)cl++;
    printf("data updating on curr elem? %ls, after pp %ls\n", (int *)cl, (int *)cl+1);

    unsigned int *curr_elem = elem;
    *curr_elem = num;

    printf("data updating on curr elem? %d num %d\n", *curr_elem, num);

}

void print_pos(int col, int row, UArray2b_T block, void *elem, void *cl)
{
    (void)cl;
    // (void)elem;
    (void)block;
    // (void)col;
    // (void)row;

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



void
check_and_print(int i, int j, UArray2b_T block, void *p1, void *p2)
{
        int *entry_p = p1;

        *((bool *)p2) &= UArray2b_at(block, i, j) == entry_p;

        if ( (i == (3 - 1) ) && (j == (3 - 1) ) ) {
                /* we got the corner */
                *((bool *)p2) &= (*entry_p == 5);
        }

        printf("ar[%d,%d]\n", i, j);
}
