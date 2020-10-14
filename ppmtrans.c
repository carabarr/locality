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
void apply_copy(int i, int j, A2 array2, void *elem, void *cl);
void apply_90r(int i, int j, A2 array2, void *elem, void *cl);
void apply_180r(int i, int j, A2 array2, void *elem, void *cl);

void copy_pixmap(Pnm_ppm image, A2Methods_T methods,
                 void *apply, A2Methods_mapfun *map);

void rotate_90(Pnm_ppm image, A2Methods_T methods, A2Methods_mapfun *map,
                int size, int height, int width);
void rotate_180(Pnm_ppm image, A2Methods_T methods, A2Methods_mapfun *map,
                int size, int height, int width);
void print_time(FILE *fp, int copy, int rot, int rotation,
                double time_used_c, double time_used_r,
                int total_pix, A2Methods_mapfun *map, A2Methods_T methods);
void print_mapping(FILE *fp, A2Methods_mapfun *map, A2Methods_T methods);

struct array_methods set_closure(Pnm_ppm image, A2Methods_T methods);


struct array_methods{
    A2Methods_T methods;
    A2Methods_UArray2 array;
};

int main(int argc, char *argv[])
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        int   i;
        FILE *fp;
        /* checks if we need to copy or rotate */
        int copy             = 0;
        int rot              = 0;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain;
        assert(methods);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default;
        assert(map);

        Pnm_ppm image;

        for (i = 1; i < argc; i++) {

                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major,
				    "row-major");
                    copy = 1;
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major,
				    "column-major");
                    copy = 1;
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                    copy = 1;
                } else if (strcmp(argv[i], "-rotate") == 0) {
                    rot = 1;
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
        /* handles opening the file */
        if (access(argv[argc - 1], R_OK) == 0) {
            fp = fopen(argv[argc - 1], "r");
            assert(fp != NULL);
            image = Pnm_ppmread(fp, methods);
        } else {
            fp = stdin;
            image = Pnm_ppmread(fp, methods);
        }


        CPUTime_T timer_c;
        CPUTime_T timer_r;
        timer_c = CPUTime_New();
        timer_r = CPUTime_New();
        double time_used_c;
        double time_used_r;


        /* holds the values of the array */
        int size = methods->size(image->pixels);
        int height = methods->height(image->pixels);
        int width = methods->width(image->pixels);

        if(copy == 1) {
            CPUTime_Start(timer_c);
            copy_pixmap(image, methods, apply_copy, map);
            time_used_c = CPUTime_Stop(timer_c);

        }

        if(rot == 1) {
            if(rotation == 0) {
                CPUTime_Start(timer_r);
                copy_pixmap(image, methods, apply_copy, map);
                time_used_r = CPUTime_Stop(timer_r);

            } else if(rotation == 90) {
                CPUTime_Start(timer_r);
                rotate_90(image, methods, map, size, height, width);
                time_used_r = CPUTime_Stop(timer_r);

            } else if (rotation == 180) {
                CPUTime_Start(timer_r);
                rotate_180(image, methods, map, size, height, width);
                time_used_r = CPUTime_Stop(timer_r);

            }
        }

        if(time_file_name != NULL) {
            FILE *fp = fopen(time_file_name, "w");
            print_time(fp, copy, rot, rotation, time_used_c, time_used_r, width * height, map, methods);
            fclose(fp);
        }

        CPUTime_Free(&timer_r);
        CPUTime_Free(&timer_c);

        Pnm_ppmwrite(stdout, image);

        fclose(fp);

        Pnm_ppmfree(&image);


        return 0;

}

void print_time(FILE *fp, int copy, int rot, int rotation, double time_used_c, double time_used_r, int total_pix, A2Methods_mapfun *map, A2Methods_T methods)
{
    fprintf(fp, "Time measurements for requested operations\n");
    print_mapping(fp, map, methods);
    if (copy == 1) {
        fprintf(fp, "For the copy operation:\n");
        fprintf(fp, "Total time: %f\n", time_used_c);
        fprintf(fp, "Time per pixel: %f\n", time_used_c / total_pix);

    }

    if (rot == 1) {
        fprintf(fp, "For the rotation operation:\n");
        fprintf(fp, "Total time: %f\n", time_used_r);
        fprintf(fp, "Rotation: %d\n", rotation);
        fprintf(fp, "Time per pixel: %f\n", time_used_r / total_pix);
    }
}

void print_mapping(FILE *fp, A2Methods_mapfun *map, A2Methods_T methods)
{
    if (map == methods->map_row_major) {
        fprintf(fp, "Mapping used: row major\n");
    } else if (map == methods->map_col_major) {
        fprintf(fp, "Mapping used: col major\n");
    } else if (map == methods->map_block_major) {
        fprintf(fp, "Mapping used: block major\n");
    }
}

void apply_copy(int i, int j, A2 array2, void *elem, void *cl)
{
    (void)array2;

    struct array_methods *cl_data = cl;
    A2 transformed_p = cl_data->array;
    A2Methods_T methods_apply = cl_data->methods;

    *(Pnm_rgb )methods_apply->at(transformed_p, i, j) = *(Pnm_rgb )elem;

}

void apply_90r(int i, int j, A2 array2, void *elem, void *cl)
{
    (void)array2;

    struct array_methods *cl_data = cl;
    A2 transformed_p = cl_data->array;
    A2Methods_T methods_apply = cl_data->methods;

    int h = methods_apply->width(transformed_p);

    *(Pnm_rgb )methods_apply->at(transformed_p, (h - j - 1), i) = *(Pnm_rgb )elem;

}

void apply_180r(int i, int j, A2 array2, void *elem, void *cl)
{
    (void)array2;

    struct array_methods *cl_data = cl;
    A2 transformed_p = cl_data->array;
    A2Methods_T methods_apply = cl_data->methods;

    int h = methods_apply->height(transformed_p);
    int w = methods_apply->width(transformed_p);

    *(Pnm_rgb )methods_apply->at(transformed_p, (w - i - 1), (h - j - 1)) = *(Pnm_rgb )elem;

}

void copy_pixmap(Pnm_ppm image, A2Methods_T methods,
                  void *apply, A2Methods_mapfun *map)
{

    struct array_methods closure = set_closure(image, methods);
    map(image->pixels, apply, &closure);
    methods->free(&(image->pixels));

    image->pixels = closure.array;

}

void rotate_90(Pnm_ppm image, A2Methods_T methods, A2Methods_mapfun *map, int size, int height, int width)
{
    struct array_methods closure;
    A2 array = methods->new(height, width, size);
    closure.methods = methods;
    closure.array = array;

    map(image->pixels, apply_90r, &closure);
    methods->free(&(image->pixels));
    image->pixels = closure.array;
    image->width = height;
    image->height = width;


}

void rotate_180(Pnm_ppm image, A2Methods_T methods, A2Methods_mapfun *map, int size, int height, int width)
{
    struct array_methods closure;
    A2 array = methods->new(width, height, size);
    closure.methods = methods;
    closure.array = array;

    map(image->pixels, apply_180r, &closure);
    methods->free(&(image->pixels));

    image->pixels = closure.array;

}

struct array_methods set_closure(Pnm_ppm image, A2Methods_T methods)
{
    struct array_methods closure;

    int size = methods->size(image->pixels);
    int height = methods->height(image->pixels);
    int width = methods->width(image->pixels);
    A2 array = methods->new(width, height, size);

    closure.methods = methods;
    closure.array = array;

    return closure;
}
