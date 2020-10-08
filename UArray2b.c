#include "UArray2b.h"
#include "UArray2.h"
#include <uarray.h>
#include <mem.h>


#define T UArray2b_T


struct T {
    UArray2_T array;
    int width;
    int height;
    int size;
    int blocksize;
};

extern T UArray2b_new (int width, int height, int size, int blocksize) {

    T blocked_array;
    NEW(blocked_array);

    UArray2_T uarray2;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            UArray_T block = UArray_new(blocksize * blocksize, size);
            *((UArray_T *)UArray2_at(uarray2, (row / blocksize), (col / blocksize)) = block;

        }
    }

    //each cell of the uarray2b is going to be a pointer to a struct (uarray)

    blocked_array->width = width;
    blocked_array->height = height;
    blocked_array->size = size;
    blocked_array->array = block;
    blocked_array->blocksize = blocksize;


    return new_uarray2b;
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

extern void *UArray2b_at(T array2b, int column, int row);

extern void UArray2b_map(T array2b,
void apply(int col, int row, T array2b,
void *elem, void *cl),
void *cl);
