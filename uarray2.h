#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T *T;

/*
UArray2_new
Purpose: Creates a new instance of UArray2_T with the given width, height, and size parameters.
Parameters: int width - the number of columns stored in UArray2
		 int height - the number of rows stored in UArray2
       int size - the size of each element being stored in the data structure
Expectations: The parameters width, height, and size cannot be < 0 (results in checked runtime error). 
*/
extern T UArray2_new(int width, int height, int size);

/*
UArray2_width
Purpose: Returns an integer representing the number of columns in the 2DArray.
Parameters: T uarray2 - the UArray2_T instance being checked 
Expectations: Checked runtime error will be raised if uarray2 is NULL.
*/
extern int UArray2_width(T uarray2);

/*
UArray2_height
Purpose: To return an integer representing the number of rows in the 2DArray.
Parameters: T uarray2 - the UArray2_T instance being checked
Expectations: Checked runtime error will be raised if uarray2 is NULL.
*/
extern int UArray2_height(T uarray2);

/*
UArray2_size
Purpose: To return the size of the individual elements stored in the 2DArray
Parameters: T uarray2 - the UArray2_T instance being checked
Expectations: Checked runtime error will be raised if uarray2 is NULL.
*/
extern int UArray2_size(T uarray2);

/*
UArray2_at
Purpose: returns a pointer to the element stored at the passed in column and row. 
Parameters: T uarray2 - the UArray2_T instance being checked
 int col - the column of the index being accessed
 Int row - the row of the index being accessed
Expectations: Raises Checked Runtime Error if the col and row elements are out 
			  of the bounds of width and height respectively
*/
extern void *UArray2_at(T uarray2, int col, int row);

/* 
map_row_major 
Purpose: loops through all of the elements in the 2DArray in row-major order, where the row is indexed slower than the column index.
Parameters: T uarray2 - the UArray2_T instance being checked
		 apply(int col, int row, T uarray, void *p1, void *p2) - the function that is being applied to all indices of the 2D array
		 Void *cl - closure, a pointer to a variable or struct that gives the function access to outside data
Expectations: Checked runtime error will be raised if uarray2 is NULL or if the apply function parameter is a null function pointer. 
*/
extern void UArray2_map_row_major(T uarray2, void apply(int col, int row, T uarray2, void *elem, void *cl), void *cl);

/*
map_col_major
Purpose: loops through all of the elements in the 2DArray in col-major order, where the column is indexed slower than the row index.
Parameters: T uarray2 - the UArray2_T instance being checked
		 apply(int col, int row, T uarray, void *p1, void *p2) - the function that is being applied to all indices of the 2D array
		 Void *cl - closure, a pointer to a variable or struct that the function access to outside data
Expectations: Checked runtime error will be raised if uarray2 is NULL or if the apply function parameter is a null function pointer.
*/
extern void UArray2_map_col_major(T uarray2, void apply(int col, int row, T uarray2, void *elem, void *cl), void *cl);

/*
UArray2_free
Purpose: deallocates the memory allocated for UArray2. Releases the memory block pointed to by the parameter pointer.
Parameters: pointer to uarray2
Expectations: Will raise a checked runtime error if uarray2 is NULL when passed into the function.
*/
extern void UArray2_free(T *uarray2);

#undef T
#endif
