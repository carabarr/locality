#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods != NULL);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (0)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] [filename]\n",
                        progname);
        exit(1);
}

typedef A2Methods_UArray2 A2;


void print_array(A2Methods_T methods, A2 array);
void check_map(int col, int row, A2 array, void *elem, void *cl);
void apply_print(int i, int j, A2 array2, void *elem, void *cl);
void print_pixel(Pnm_rgb pixel);
void copy_pixmap(int i, int j, A2 array2, void *elem, void *cl);


struct array_methods{
    A2Methods_T methods;
    A2Methods_UArray2 array;
};


static inline void copy_unsigned(A2Methods_T methods, A2 a,
                                 int i, int j, int n);

int main(int argc, char *argv[])
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default;
        assert(map);

        Pnm_ppm test;

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major,
				    "row-major");
                    printf("good?? we should be here\n");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major,
				    "column-major");
                    printf("now we should be here\n");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr,
					"Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                } else if (strcmp(argv[i], "-time") == 0) {
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
				argv[i]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else if (access(argv[i], R_OK) == 0) {
                    //that means we can read??
                    printf("success?? question mark??\n");
                    FILE *fp = fopen(argv[i], "r");
                    if(fp != NULL) {
                        test = Pnm_ppmread(fp, methods);
                    }
                    // assert(fp != NULL);
                } else {
                        break;
                }
        }


        printf("%d\n", test->methods->height(test));
        if(map == methods->map_col_major){
            printf("this should be set to col major \n");
        } else {
            printf("is default row major\n");
        }

        // Pnm_ppm image;
        Pnm_ppm transformed = malloc(sizeof(Pnm_ppm));


        int size = methods->size(test);
        int width = methods->width(test);
        int height = methods->height(test);

        A2 transformed_p = methods->new(width, height, size);

        struct array_methods closure;
        closure.array = transformed_p;
        closure.methods = uarray2_methods_plain; //only on plain


        // //cpu timer outside of map
        map(test->pixels, copy_pixmap, &closure);
        test->pixels = transformed_p;
        FILE *fpw = fopen("test_write.ppm", "w");
        Pnm_ppmwrite(fpw, test);
        // map(test->pixels, apply_print, &height);



        return 0;
        // assert(0);  // the rest of this function is not yet implemented
}


void copy_pixmap(int i, int j, A2 array2, void *elem, void *cl)
{
    // (void)i;
    // (void)j;
    (void)array2;

    struct array_methods *cl_data = cl;


    A2 transformed_p = cl_data->array;
    A2Methods_T methods_apply = cl_data->methods;


    Pnm_rgb *curr_pixel = elem; //this is copying current element of original
                                //new Pnm_rgb instance?
    Pnm_rgb new_pix = *curr_pixel;
    // Pnm_rgb *copy_pixel;
    // *copy_pixel->red = (*curr_pixel)->red;
    // *copy_pixel->green = (*curr_pixel)->green;
    // *copy_pixel->blue = (*curr_pixel)->blue;

    Pnm_rgb *pixel_ptr = methods_apply->at(transformed_p, i, j);
    *pixel_ptr = new_pix;
    //freeA2
    // A2 *tran
    // Pnm_rgb *pixel_ptr = (*transformed_pm)->methods->at((*transformed_pm)->pixels, i, j);
    // *pixel_ptr = elem;
    //set rgb values individually!!!!!!
}










//if you want ot get something out use the closure (for holding the transformed array for the closure)
void apply_print(int i, int j, A2 array2, void *elem, void *cl)
{
    (void)i;
    (void)j;
    (void)array2;

    // int *num = elem;
    // int *num = UArray2_at(array2, i, j);
    print_pixel(elem);
    int *width = cl;
    // if(i == *width - 1) {
    //     printf("\n");
    // }
    // printf("%d ", *num);
}



static inline void copy_unsigned(A2Methods_T methods, A2 a,
                                 int i, int j, int n)
{
        int *p = methods->at(a, i, j);
        *p = n;
}
////////////////////////////////////////
//MAKE AN APPLY FUNCTION WHOSE ONLY JOB IS PRINTING AN ARRAY PIXEL BY PIXEL
//AND CALL THAT APPLY FUNCTION ON WHATEVER MAPPING WE NEED FOR -COL-MAJOR
//-ROW-MAJOR FLAGS OR WHATEVER
///////////////////////////////////////
void print_array(A2Methods_T methods, A2 array)
{
    for (int i = 0; i < methods->height(array); i++) {
        for (int j = 0; j < methods->width(array); j++) {
            int *n = methods->at(array, i, j);
            printf("%d ", *n);
        }
        printf("\n");
    }
}

//rather than working with an A2 we can work with the pixels

void print_pixel(Pnm_rgb pixel)
{
    printf("%d %d %d   ", pixel->red, pixel->green, pixel->blue);

}



//
// printf("hello\n");
// A2 test = methods->new(2, 2, 4);
//
// // test = methods->
// copy_unsigned(methods, test, 0, 0, 1);
// copy_unsigned(methods, test, 0, 1, 2);
// copy_unsigned(methods, test, 1, 0, 3);
// copy_unsigned(methods, test, 1, 1, 4);
// int *n = methods->at(test, 0, 0);
// printf("first thing in array? %d\n", *n);
// printf("worked?\n");
//
// print_array(methods, test);

// int size = methods->size(array);
// Pnm_rgb pixel = test->methods->at(test, 0, 0); segfaults lmao
// printf("%d\n", pixel->red);



    // int *num = elem;
    // int *num = UArray2_at(array2, i, j);
    //
    // print_pixel(elem);
    // int *width = cl;
    // if(i == *width - 1) {
    //     printf("\n");
    // }
    // printf("%d ", *num);
