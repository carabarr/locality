#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include <mem.h>
#include "cputiming.h"

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
void rotate_90(int i, int j, A2 array2, void *elem, void *cl);
void rotate_180(int i, int j, A2 array2, void *elem, void *cl);
// struct Pnm_rgb set_rgb(Pnm_rgb *curr_pixel);


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
        FILE *fp;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default;
        assert(map);

        Pnm_ppm test;
        // printf("WIDTH OF SIZE: %d\n", test->methods->size(test->pixels));
        //

        for (i = 1; i < argc; i++) {

                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major,
				    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major,
				    "column-major");
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
                }

        }

            if (access(argv[argc - 1], R_OK) == 0)
            {

                fp = fopen(argv[argc - 1], "r");
                if(fp != NULL) {
                // printf("file opened!\n" );
                    test = Pnm_ppmread(fp, methods);
                // printf("file opened!222\n" );
            }

            // assert(fp != NULL);
            } else {
                fp = stdin;
                test = Pnm_ppmread(fp, methods);
            }



        CPUTime_T timer;
        timer = CPUTime_New();
        double time_used;


        struct array_methods closure;
        //only on plain

        int size = methods->size(test->pixels);
        int height = methods->height(test->pixels);
        int width = methods->width(test->pixels);



        CPUTime_Start(timer);



        if (rotation == 90) {

            A2 r_90 = methods->new(height, width, size);
            closure.methods = uarray2_methods_plain;
            // A2 r_90 = methods->new_with_blocksize(height, width, size, 2);
            closure.array = r_90;

            map(test->pixels, rotate_90, &closure);
            test->pixels = r_90;

        } else if (rotation == 180) {
            //can we give a generalized version of methods to rotation??
            //can we just do rotations with row major or does that defeat
            //the point of the assingment?
            //
            //when the user calls -block-major -rotate-180 dies that mean
            // that they want the rotation to be performed block major??
            // what do we write in the timing output
            A2 r_180 = methods->new(width, height, size);
            closure.methods = uarray2_methods_plain;

            closure.array = r_180;

            map(test->pixels, rotate_180, &closure);
            test->pixels = r_180;

        } else {
            A2 copy = methods->new(width, height, size);

            closure.array = copy;

            closure.methods = uarray2_methods_blocked;

            map(test->pixels, copy_pixmap, &closure);
            test->pixels = copy;


        }

        time_used = CPUTime_Stop(timer);

        //print to a file,
        //rotation, col/block/row, type
        printf ("Mapping time was computed in %.0f nanoseconds\n", time_used);
        Pnm_ppmwrite(stdout, test);

        return 0;

}


void copy_pixmap(int i, int j, A2 array2, void *elem, void *cl)
{
    (void)array2;

    struct array_methods *cl_data = cl;
    A2 transformed_p = cl_data->array;
    A2Methods_T methods_apply = cl_data->methods;

    *(Pnm_rgb )methods_apply->at(transformed_p, i, j) = *(Pnm_rgb )elem;

}

//WE NEED TO SWITCH HEIGHT WITH WIDTH
void rotate_90(int i, int j, A2 array2, void *elem, void *cl)
{
    (void)array2;

    struct array_methods *cl_data = cl;
    A2 transformed_p = cl_data->array;
    A2Methods_T methods_apply = cl_data->methods;

    int h = methods_apply->height(transformed_p);


    *(Pnm_rgb )methods_apply->at(transformed_p, (h - j - 1), i) = *(Pnm_rgb )elem;

}

void rotate_180(int i, int j, A2 array2, void *elem, void *cl)
{
    (void)array2;

    struct array_methods *cl_data = cl;
    A2 transformed_p = cl_data->array;
    A2Methods_T methods_apply = cl_data->methods;

    int h = methods_apply->height(transformed_p);
    int w = methods_apply->width(transformed_p);


    *(Pnm_rgb )methods_apply->at(transformed_p, (w - i - 1), (h - j - 1)) = *(Pnm_rgb )elem;

}










//if you want ot get something out use the closure (for holding the transformed array for the closure)
void apply_print(int i, int j, A2 array2, void *elem, void *cl)
{
    (void)i;
    (void)j;
    (void)array2;

    int *num = elem;
    // int *num = UArray2_at(array2, i, j);
    print_pixel(elem);
    int *width = cl;
    if(i == *width - 1) {
        printf("\n");
    }
    printf("%d ", *num);
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

// struct Pnm_rgb set_rgb(Pnm_rgb *curr_pixel)
// {
//     Pnm_rgb new_pixel = malloc(sizeof(Pnm_rgb));
//     new_pixel->red = (*curr_pixel)->red;
//     new_pixel->green = (*curr_pixel)->green;
//     new_pixel->blue = (*curr_pixel)->blue;
//
//     return new_pixel;
// }


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


    // void copy_pixmap(int i, int j, A2 array2, void *elem, void *cl)
    // {
    //     (void)array2;
    //
    //     struct array_methods *cl_data = cl;
    //     A2 transformed_p = cl_data->array;
    //     A2Methods_T methods_apply = cl_data->methods;
    //
    //     //HOLY GRAIL DONT TOUCH ANYMORE
    //     Pnm_rgb curr_pixel = elem; //this is copying current element of
    //                                 // original new Pnm_rgb instance?
    //
    //     Pnm_rgb new_pix = malloc(sizeof(struct Pnm_rgb));
    //
    //     new_pix->red = curr_pixel->red;   // assigning values manually works
    //     new_pix->green = curr_pixel->green;
    //     new_pix->blue = curr_pixel->blue;
    //
    //     // printf("printing pixel before adding\n");
    //     // print_pixel(new_pix);
    //     Pnm_rgb *pixel_ptr = &new_pix
    //
    //     Pnm_rgb *pixel_ptr = methods_apply->at(transformed_p, i, j); //are these pointing to the same thing???????
    //     // if(pixel_ptr == methods_apply->at(transformed_p, i, j)) {
    //     //     printf("success?\n");
    //     // }
    //     pixel_ptr = &new_pix;
    //     // printf("printing pixel after adding\n");
    //     // print_pixel(methods_apply->at(transformed_p, i, j));
    //     // printf("did we fuck up new pix??\n");
    //     // print_pixel(new_pix);
    //     printf("what happens when we dereference pixel_ptr??\n");
    //     print_pixel(*pixel_ptr);
    //
    //
    // }


    // (void)array2;
    //
    // struct array_methods *cl_data = cl;
    // A2 transformed_p = cl_data->array;
    // A2Methods_T methods_apply = cl_data->methods;
    //
    // //HOLY GRAIL DONT TOUCH ANYMORE
    // Pnm_rgb curr_pixel = elem; //this is copying current element of
    //
    //
    // Pnm_rgb new_pix = malloc(sizeof(struct Pnm_rgb));
    //
    // new_pix->red = curr_pixel->red;   // assigning values manually works
    // new_pix->green = curr_pixel->green;
    // new_pix->blue = curr_pixel->blue;
    //
    //
    // Pnm_rgb *pixel_ptr = &new_pix;
    // //WTF TWFTFWTEFTWYFTYFA how does this work???
    // *(Pnm_rgb )methods_apply->at(transformed_p, i, j) = **pixel_ptr;
