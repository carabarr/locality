#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include <math.h>


#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"

typedef A2Methods_UArray2 A2;

//

typedef struct Pnm_float{
    float red, blue,green;
}*Pnm_float;

struct closure_data {
    A2Methods_UArray2 array;
    A2Methods_T methods;
    int denominator;
};

void trim_edges(Pnm_ppm image, A2Methods_T methods);
int check_height(Pnm_ppm image);
int check_width(Pnm_ppm image);
void print_apply(int i, int j, A2 array, void *elem, void *cl);
void apply_convers(int i, int j, A2 array, void *elem, void *cl);

float int_to_float(int color, int denominator);
Pnm_float convert_to_float(Pnm_rgb pixel, int denominator);

float root_mean_sqr_diff(Pnm_ppm image_1, Pnm_ppm image_2, A2Methods_T methods);
// ENUMERATE LATER: RED 0 GREEN 1 BLUE 2
float sum_diff(Pnm_ppm image_1, Pnm_ppm image_2, A2Methods_T methods, int w, int h);




// typedef A2Methods_UArray2 A2;



int main(int argc, char *argv[]) {
    // printf("hewwo world uwu\n");
    assert(argc == 3);
    int i;
    A2Methods_T methods = uarray2_methods_plain;


    Pnm_ppm image_1;
    // bool b1 = false;
    Pnm_ppm image_2;
    // bool b2 = false;
    float bad_diff = 1.0;

    for (i = 1; i < argc; i++) {
        assert(strcmp(argv[1], "-") != 0 || strcmp(argv[2], "-") != 0);

        if (strcmp(argv[i], "-") == 0) {
            FILE *fp = stdin;

            if(i == 1) {
                image_1 = Pnm_ppmread(fp, methods);
            } else {
                image_2 = Pnm_ppmread(fp, methods);
            }
            fclose(fp);


        } else {
            FILE *fp = fopen(argv[i], "r");
            assert(fp != NULL);

            if(i == 1) {
                image_1 = Pnm_ppmread(fp, methods);
            } else {
                image_2 = Pnm_ppmread(fp, methods);
            }
            fclose(fp);

        }
    }

    printf("height of image 1? %d\n", image_1->height);
    printf("height of image 2? %d\n", image_2->height);

    int height_diff = image_1->height - image_2->height;
    int width_diff = image_1->width - image_2->width;
    // printf("width diff?? %d\n", width_diff);

    if (abs(width_diff) > 1) {
        fprintf(stderr, "Width difference too big\n");
        printf("%.1f\n", bad_diff);
    }

    if (abs(height_diff) > 1) {
        fprintf(stderr, "Height difference too big\n");
        printf("%.1f\n", bad_diff);
    }

    // methods->map_row_major(image_1->pixels, print_apply, &image_1->width);


    trim_edges(image_1, methods);
    trim_edges(image_2, methods);
    // printf("WIDTH AND HEIGHT: %d %d\n", image_1->width, image_1->height);

    // printf("prints????? lmao\n");
    // methods->map_row_major(image_1->pixels, print_apply, &image_1->width);


    //define closure here
    struct closure_data closure;
    A2 float_array = methods->new(image_1->width, image_1->height, sizeof(struct Pnm_float));
    closure.methods = methods;
    closure.array = float_array;
    closure.denominator = image_1->denominator;

    methods->map_row_major(image_1->pixels, apply_convers, &closure);
    methods->free(&(image_1->pixels)); //we free memory of previous integer vals

    image_1->pixels = closure.array; //floating point supremacy

    methods->map_row_major(image_2->pixels, apply_convers, &closure);
    methods->free(&(image_2->pixels)); //we free memory of previous integer vals

    image_2->pixels = closure.array;

    printf("should be printing in floats\n");
    Pnm_float pixel_test = methods->at(image_1->pixels, 1, 1);
    printf("blue %.2f\n", pixel_test->blue);

    float diff = root_mean_sqr_diff(image_1, image_2, methods);

    printf("E VALUE: %f\n", diff);

    return 0;
}


float int_to_float(int color, int denominator)
{
    float fColor = color;
    float fDenominator = denominator;
    float float_point = (fColor/fDenominator);
    return float_point;
}

Pnm_float convert_to_float(Pnm_rgb pixel, int denominator)
{
  Pnm_float float_pixel = malloc(sizeof(struct Pnm_float));
  float_pixel->red = int_to_float(pixel->red, denominator);
  float_pixel->green =  int_to_float(pixel->green, denominator);
  float_pixel->blue =  int_to_float(pixel->blue, denominator);

  return float_pixel;
}

int check_width(Pnm_ppm image)
{
    int width = image->width;
    if(width % 2 == 1){
        image->width = width - 1;
        return width - 1;
    }else{
        return width;
    }
}

int check_height(Pnm_ppm image)
{
    int height = image->height;
    if(height % 2 == 1){
        image->height = height - 1;
        return height - 1;
    }else{
        return height;
    }
}

//loop through
void trim_edges(Pnm_ppm image, A2Methods_T methods)
{

    if(image->height % 2 != 0 || image->width % 2 != 0) {
        int new_height = check_height(image);
        int new_width = check_width(image);

        A2 trimmedArray = methods->new(new_width, new_height,
                                         sizeof(struct Pnm_rgb));

        for(int row = 0; row < new_height; row++) {
            for(int col = 0; col < new_width; col++) {

                *(Pnm_rgb)methods->at(trimmedArray,col, row) =
                *(Pnm_rgb)methods->at(image->pixels, col, row);

            }
        }

        methods->free(&(image->pixels));
        image->pixels = trimmedArray;

    }
}

void print_apply(int i, int j, A2 array, void *elem, void *cl)
{
    (void)i;
    (void)j;
    (void)array;
    int *width = cl;
    Pnm_rgb pixel = elem; //shouldnt we do something with this? lmao
    printf("%d %d %d  ", pixel->red, pixel->green, pixel->blue);

    if((i + 1) % *width == 0)
    {
        printf("\n");
    }
}

void apply_convers(int i, int j, A2 array, void *elem, void *cl)
{
    (void)i;
    (void)j;
    (void)array;

    struct closure_data *cl_data = cl;

    A2 float_array = cl_data->array;
    int denom = cl_data->denominator;
    A2Methods_T methods = cl_data->methods;

    Pnm_rgb pixel = elem;
    Pnm_float new_pixel = convert_to_float(pixel, denom);

    *(Pnm_float)methods->at(float_array, i, j) = *new_pixel;

}

//WE HAVE TO PASS AN IMAGE WITH FLOATING POINT PIXELS
float root_mean_sqr_diff(Pnm_ppm image_1, Pnm_ppm image_2, A2Methods_T methods)
{
    int w, h;
    //this is how we check which image has the bigger dimensions
    if(image_2->width < image_1->width || image_2->height < image_1->height) {
        w = image_2->width;
        h = image_2->height;
    } else {
        w = image_1->width;
        h = image_1->height;
    }

    float numerator = sum_diff(image_1, image_2, methods, w, h);

    //divide over 3 x w x h
    float fraction = numerator / (3 * w * h);

    //square root the whole thing
    float final = sqrt(fraction);

    return final;

}

float sum_diff(Pnm_ppm image_1, Pnm_ppm image_2, A2Methods_T methods,
                int w, int h)
{
    float sum = 0;
    for (int i = 0; i < w; i++) {
        for (int j = 0; j < h; j++) {
            Pnm_float pixel_1 = methods->at(image_1->pixels, i, j);
            Pnm_float pixel_2 = methods->at(image_2->pixels, i, j);

            float difference_red = pixel_1->red - pixel_2->red;
            float difference_green = pixel_1->green - pixel_2->green;
            float difference_blue = pixel_1->blue - pixel_2->blue;
            float temp = pow(difference_red, 2) + pow(difference_green, 2) + pow(difference_blue, 2);

            sum += temp;
        }
    }
    return sum;
}
