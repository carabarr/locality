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
    // UArray2_T test = create_blocked_array(width, height, size, blocksize);

    int true_width = ceil(width/(double)blocksize);
    int true_height = ceil(height/(double)blocksize);
   //
    blocked_array->width = width;
    blocked_array->height = height;
    blocked_array->blocksize = blocksize;
    blocked_array->size = size;
    blocked_array->array = UArray2_new((int)ceil(width/(double)blocksize), (int)ceil(height/(double)blocksize), sizeof(UArray_T));


    for(int i = 0; i < (int)ceil(width/(double)blocksize); i++)
    {
        for(int j = 0; j < (int)ceil(height/(double)blocksize); j++)
         {
             printf("THIS IS I %d\n", i);
             printf("THIS IS J%d\n", j);

            UArray_T *block = UArray2_at(blocked_array->array, i, j);
            *block = UArray_new(blocksize*blocksize, size);
         }
    }


    return blocked_array;


}

UArray2_T create_blocked_array(int width, int height, int size, int blocksize);
// {
//     int true_width = ceil(width/blocksize);
//     int true_height = ceil(height/blocksize);
//
//      UArray2_T block_array = UArray2_new(true_width, true_height, sizeof(UArray_T));
//
//      for(int i = 0; i < true_width; i++)
//      {
//          for(int j = 0; j < true_height; j++)
//          {
//              // printf("THIS IS I %d\n", i);
//              // printf("THIS IS J%d\n", j);
//             // UArray_T block = UArray_new(blocksize * blocksize, size);
//             UArray_T *block = UArray2_at(block_array, i, j);
//             *block = UArray_new(blocksize * blocksize, size);
//             // UArray_T *block_ptr = UArray2_at(block_array, i, j);
//             // *block_ptr = block;
//
//          }
//      }
//
//      return block_array;
//
//
// }




extern T UArray2b_new_64K_block(int width, int height, int size)
{

    int blocksize;

    if (size > 64 * 1024) {
        UArray2b_new(width, height, size, 1);
    } else {
        blocksize = sqrt(64 * 1024 / size);
        if (blocksize >= width || blocksize >= height) {
            if (width > height) {
                UArray2b_new(width, height, size, width);
            } else {
                UArray2b_new(width, height, size, height);
            }
        } else {
            UArray2b_new(width, height, size, blocksize);
        }

    }

}



extern void UArray2b_free (T *array2b)
{
    //
    // float width_d = (*array2b)->width;
    // float height_d = (*array2b)->height;
    // float blocksize = (*array2b)->blocksize;
    //
    // int true_width = ceil(width_d/blocksize);
    // int true_height = ceil(height_d/blocksize);
    /* frees the memory associated with each block inside of the uarray2 */
    // for (int row = 0; row < true_height; row++) {
    //     for (int col = 0; col < true_width; col++) {
    //         UArray_T *block_ptr = UArray2_at((*array2b)->array, row, col);
    //         UArray_free(&(*block_ptr));
    //
    //     }
    // }
    /* frees the uarray2 itself */


        for(int i = 0; i < (int)ceil((*array2b)->width/(double)(*array2b)->blocksize); i++)
        {
            for(int j = 0; j < (int)ceil((*array2b)->height/(double)(*array2b)->blocksize); j++)
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
                printf("Client_col %d client_row %d\n", (col * bs + b_col), (row * bs + b_row));
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
//
//
// assert(blocksize >= 1);
// T blocked_array;
// NEW(blocked_array);
//
// //transforming this into floats so that the ceiling func works
// float width_d = width;
// float height_d = height;
//
// int true_width = ceil(width_d/blocksize);
// int true_height = ceil(height_d/blocksize);
//
// UArray2_T uarray2 = UArray2_new(true_width, true_height, sizeof(UArray_T));
// assert(uarray2);
//
// printf("true width: %d\n", true_width);
// printf("true height: %d\n", true_height);
// printf("Uarray2 width: %d\n",UArray2_width(uarray2) );
// printf("Uarray2 height: %d\n", UArray2_height(uarray2));
//
//
// for (int row = 0; row < true_height; row++) {
//     for (int col = 0; col < true_width; col++) {
//
//         UArray_T block = UArray_new(blocksize * blocksize, size);
//         printf("length of block: %d\n", UArray_length(block));
//
//         printf("hello row %d col %d\n", row, col);
//         UArray_T *block_ptr = UArray2_at(uarray2, col, row);
//         *block_ptr = block;
//
//     }
// }
//
// //each cell of the uarray2b is going to be a pointer to a struct (uarray)
// blocked_array->width = width;
// blocked_array->height = height;
// blocked_array->size = size;
// blocked_array->array = uarray2;
// blocked_array->blocksize = blocksize;
//
//
// return blocked_array;
