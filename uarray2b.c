#include "uarray2b.h"
#include "uarray2.h"
#include <uarray.h>
#include <mem.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
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


    // printf("true_width %d\n", true_width);
    // printf("true_height %d\n", true_height);

    UArray2_T uarray2 = UArray2_new(true_width, true_height, size);

    for (int row = 0; row < true_height; row++) {
        for (int col = 0; col < true_width; col++) {
            UArray_T block = UArray_new(blocksize * blocksize, size);
            // block = UArray2_at(uarray2, row, col);


          block =  *(UArray_T **)UArray2_at(uarray2, row, col);


            if(UArray_length(block) == 1)
            {
                printf("test silence warning \n");
            }

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


extern void UArray2b_free (T *array2b);

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
    assert  (array2b->width > column || array2b->height > row);
    int blocksize = array2b->blocksize;

    UArray_T to_search = UArray2_at(array2b->array, row/blocksize, column/blocksize);
    return UArray_at(to_search, (blocksize * (row % blocksize) + (column % blocksize)));

}


extern void UArray2b_map(T array2b,
void apply(int col, int row, T array2b,
void *elem, void *cl),
void *cl);
