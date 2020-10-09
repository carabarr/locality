#include "uarray2b.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

void print_bArray(UArray2b_T block);

void
check_and_print(int i, int j, UArray2b_T block, void *p1, void *p2);
void pop_block(int col, int row, UArray2b_T block, void *elem, void *cl);

int main() {
    printf("hello world\n");

    UArray2b_T test_block = UArray2b_new(3, 3, 4, 2);
    int height = UArray2b_height(test_block);
    printf("printing height of 8: %d\n", height);
    //
    *((int *)UArray2b_at(test_block, 0, 0)) = 5;
    // *((int *)UArray2b_at(test_block, 2, 0)) = 21;
    int arr[9] = {5, 10, 39, 59, 8, 1, 3, 5, 100};


    UArray2b_map(test_block, check_and_print, arr);
    printf("\n" );
    print_bArray(test_block);




    printf("testing test_block %d\n", *((int *)UArray2b_at(test_block, 0,0)));

    UArray2b_free(&test_block);


    return 0;
}

//
// void pop_uarray(int col, int row, UArray2_T sudoku, void *elem, void *cl) {
//     (void)row;
//     (void)col;
//     (void)sudoku;
//     Pnmrdr_T *my_pnm = cl;
//     unsigned int data = Pnmrdr_get(*my_pnm);
//     unsigned int *curr_elem = elem;
//     *curr_elem = data;
// }
//

void pop_block(int col, int row, UArray2b_T block, void *elem, void *cl)
{
    (void)row;
    (void)col;
    (void)block;
    int *arr = cl;
    for(int i = 0; i < 9; i++)
    {
        unsigned int data = arr[i];
        unsigned int *curr_elem = elem;
        *curr_elem = data;
        printf("CELL DATA:%d\n",data);

    }

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
