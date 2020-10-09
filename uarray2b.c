#include "uarray2b.h"
#include "uarray2.h"
#include <uarray.h>
#include <mem.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#define T UArray2b_T


struct T {
    UArray2_T array;
    int width;
    int height;
    int size;
    int blocksize;
};

extern T UArray2b_new (int width, int height, int size, int blocksize) {
    assert(blocksize >= 1);
    T blocked_array;
    NEW(blocked_array);

    //transforming this into floats so that the ceiling func works
    float width_d = width;
    float height_d = height;

    int true_width = ceil(width_d/blocksize);
    int true_height = ceil(height_d/blocksize);

    UArray2_T uarray2 = UArray2_new(true_width, true_height, sizeof(UArray_T));

    for (int row = 0; row < true_height; row++) {
        for (int col = 0; col < true_width; col++) {
            UArray_T block = UArray_new(blocksize * blocksize, size);
            UArray_T *block_ptr = UArray2_at(uarray2, row, col);
            *block_ptr = block;

        }
    }

    //each cell of the uarray2b is going to be a pointer to a struct (uarray)
    blocked_array->width = width;
    blocked_array->height = height;
    blocked_array->size = size;
    blocked_array->array = uarray2;
    blocked_array->blocksize = blocksize;


    return blocked_array;
}

extern T UArray2b_new_64K_block(int width, int height, int size);


extern void UArray2b_free (T *array2b)
{

    float width_d = (*array2b)->width;
    float height_d = (*array2b)->height;
    float blocksize = (*array2b)->blocksize;

    int true_width = ceil(width_d/blocksize);
    int true_height = ceil(height_d/blocksize);
    /* frees the memory associated with each block inside of the uarray2 */
    for (int row = 0; row < true_height; row++) {
        for (int col = 0; col < true_width; col++) {
            UArray_T *block_ptr = UArray2_at((*array2b)->array, row, col);
            UArray_free(&(*block_ptr));

        }
    }
    /* frees the uarray2 itself */
    UArray2_free(&(*array2b)->array);
    /* frees the actual struct that is uarray2b */
    free(*array2b);

}





extern int UArray2b_width (T array2b)
{
    assert(array2b);
    return array2b->width;
}

extern int UArray2b_height (T array2b)
{
    assert(array2b);
    return array2b->height;
}

extern int UArray2b_size (T array2b)
{
    assert(array2b);
    return array2b->size;

}

extern int UArray2b_blocksize(T array2b)
{
    assert(array2b);
    return array2b->blocksize;

}

extern void *UArray2b_at(T array2b, int column, int row)
{
    assert(array2b->width > column && array2b->height > row);
    assert(-1 < column && -1 < row);
    int blocksize = array2b->blocksize;

    UArray_T *to_search = UArray2_at(array2b->array, row/blocksize, column/blocksize);


    return UArray_at(*to_search, (blocksize * (row % blocksize) + (column % blocksize)));

}


extern void UArray2b_map(T array2b,
                         void apply(int col, int row, T array2b,
                         void *elem, void *cl),
                         void *cl)
{

    float width_d = (array2b)->width;
    float height_d = (array2b)->height;
    int blocksize = (array2b)->blocksize;

    int true_width = ceil(width_d/blocksize);
    int true_height = ceil(height_d/blocksize);

    //change to width_d
    //change to height_d


    for (int col = 0; col < true_width; col++) {
        for (int row = 0; row < true_height; row++) {
            UArray_T *to_search = UArray2_at(array2b->array, row, col);
            // apply(col, row, array2b, UArray2b_at(array2b, col, row), cl);
            // UArray_T *to_search = UArray2_at(array2b->array, row/blocksize, col/blocksize);
            for(int col2 = 0; col2 < width_d; col2++)
            {
                for(int row2 = 0; row2 < height_d; row2++)
                {
                    apply(col2, row2, array2b, UArray_at(*to_search, (blocksize * (row2 % blocksize) + (col2 % blocksize))), cl);
                }
            }
            // apply(col, row, array2b, UArray_at(*to_search, (blocksize * (row % blocksize) + (col % blocksize))), cl);

        }
    }
}

// float width_d = (*array2b)->width;
// float height_d = (*array2b)->height;
// float blocksize = (*array2b)->blocksize;
//
// int true_width = ceil(width_d/blocksize);
// int true_height = ceil(height_d/blocksize);
