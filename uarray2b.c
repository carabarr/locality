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

extern T UArray2b_new (int width, int height, int size, int blocksize)
 {
    assert(blocksize >= 1);
    T blocked_array;
    NEW(blocked_array);


    int a2_width = ceil(width/(double)blocksize);
    int a2_height = ceil(height/(double)blocksize);

    blocked_array->width = width;
    blocked_array->height = height;
    blocked_array->blocksize = blocksize;
    blocked_array->size = size;
    blocked_array->array = UArray2_new(a2_width, a2_height, sizeof(UArray_T));


    for(int i = 0; i < a2_width; i++)
    {
        for(int j = 0; j < a2_height; j++)
         {
             // printf("THIS IS I %d\n", i);
             // printf("THIS IS J %d\n", j);
            UArray_T *block = UArray2_at(blocked_array->array, i, j);
            *block = UArray_new(blocksize*blocksize, size);
         }
    }


    return blocked_array;


}

//actually returning something now check again with tests.
extern T UArray2b_new_64K_block(int width, int height, int size)
{

    int blocksize;

    if (size > 64 * 1024) {
        return UArray2b_new(width, height, size, 1);
    }

    blocksize = sqrt(64 * 1024 / size);
    if (blocksize >= width || blocksize >= height) {
        if (width > height) {
            return UArray2b_new(width, height, size, width);
        } else {
            return UArray2b_new(width, height, size, height);
        }
    }

    return UArray2b_new(width, height, size, blocksize);

}



extern void UArray2b_free (T *array2b)
{
    float width_d = (*array2b)->width;
    float height_d = (*array2b)->height;
    float blocksize = (*array2b)->blocksize;

    int a2_width = ceil(width_d/blocksize);
    int a2_height = ceil(height_d/blocksize);


        for(int i = 0; i < a2_width; i++)
        {
            for(int j = 0; j < a2_height; j++)
             {
                 UArray_free(UArray2_at((*array2b)->array, i, j));
             }
        }
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

    UArray_T *to_search = UArray2_at(array2b->array, column/blocksize, row/blocksize);


    return UArray_at(*to_search, (blocksize * (column % blocksize) + (row % blocksize)));

}



extern void UArray2b_map(T array2b,
                         void apply(int col, int row, T array2b,
                         void *elem, void *cl),
                         void *cl)
{
    assert(array2b);
    int h = UArray2_height(array2b->array);
    int w = UArray2_width(array2b->array);
    int bs = array2b->blocksize;

    int cl_h = array2b->height;
    int cl_w = array2b->width;

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {

            UArray_T *to_search = UArray2_at(array2b->array, col, row);
            // print_block(*to_search);

            for (int i = 0; i < UArray_length(*to_search); i++) {
                int b_col = i / array2b->blocksize;
                int b_row = i % array2b->blocksize;
                // printf("Client_col %d client_row %d\n", (col * bs + b_col), (row * bs + b_row));
                if ((col * bs + b_col < cl_w) && (row * bs + b_row < cl_h)) {
                    apply(b_col, b_row, array2b, UArray_at(*to_search, i), cl);
                }

            }

        }



    }

}

void print_block(UArray_T block)
{
    for (int i = 0; i < UArray_length(block); i++) {
        printf("%d ", *(int *)UArray_at(block, i));
    }
}

int get_index(T array2b, int col, int row)
{
    int blocksize = array2b->blocksize;
    return blocksize * (row % blocksize) + (col % blocksize);
}
