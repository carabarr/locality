#include "uarray2.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <mem.h>
#include <uarray.h>

#define T UArray2_T

struct T {
    UArray_T array;
    int width;
    int height;
    int size;
};

/*
    Will malloc memory for the struct, client must call
    UArray2_free to free memory allocated in the function
*/
T UArray2_new(int width, int height, int size) {
    assert(width >= 0 && height >= 0 && size >= 0);
    T new_struct; 
    NEW(new_struct);
    new_struct->width = width;
    new_struct->height = height;
    new_struct->array = UArray_new(width*height, size);
    new_struct->size = size;
    return new_struct;
}

int UArray2_width(T uarray2) {
    assert(uarray2 != NULL);
    return uarray2->width;
}

int UArray2_height(T uarray2) {
    assert(uarray2 != NULL);
    return uarray2->height;
}

int UArray2_size(T uarray2) {
    assert(uarray2 != NULL);
    return uarray2->size;
}

void *UArray2_at(T uarray2, int col, int row) {
    assert(uarray2 != NULL);
    assert(row < uarray2->height && col < uarray2->width);
    return UArray_at(uarray2->array, (uarray2->width)*row+col);
}

/*
    For each row, this function will loop through each column 
    before indexing to the next row
*/
void UArray2_map_row_major(T uarray2, void apply(int col, 
        int row, T uarray2, void *elem, void *cl), void *cl) {
    assert(uarray2 != NULL);
    for (int r = 0; r < uarray2->height; r++) {
        for (int c = 0; c < uarray2->width; c++) {
            apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
        }
    }
}

/*
    For each column, this function will loop through each row 
    before indexing to the next column
*/
void UArray2_map_col_major(T uarray2, void apply(int col, 
        int row, T uarray2, void *elem, void *cl), void *cl) {
    assert(uarray2 != NULL);
    for (int c = 0; c < uarray2->width; c++) {
        for (int r = 0; r < uarray2->height; r++) {
            apply(c, r, uarray2, UArray2_at(uarray2, c, r), cl);
        }
    }
}

void UArray2_free(T *uarray2) {
    assert(uarray2 && *uarray2);
    UArray_free(&(*uarray2)->array);
    free(*uarray2);
}