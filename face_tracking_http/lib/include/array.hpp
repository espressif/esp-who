#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#include <assert.h>

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "freertos/FreeRTOS.h"

#include "kiss_fft.h"

#ifdef __cplusplus
}
#endif

#ifndef max(x, y)
#define max(x, y) (((x) < (y)) ? (y) : (x))
#endif

#ifndef min(x, y)
#define min(x, y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef abs(x)
#define abs(x) (((x) < (0)) ? (-(x)) : (x)) // NOTE: as for float, comparision should be '<'
#endif

#ifndef PI
#define PI 3.1415926535897932384626433832795028841971
#endif

#define MALLOC_IN_SPIRAM 0

typedef enum
{
    DO_MALLOC,
    DO_CALLOC,
    NOT_ALLOC,
} alloc_mode_t;

template <typename T>
struct point_coordinate_t
{
    T x;
    T y;
};

template <typename C, typename V>
struct point_value_t
{
    struct point_coordinate_t<C> coordinate;
    V value;
};

template <typename T>
struct rectangle_shape_t
{
    T height;
    T width;
};

template <typename T>
struct rectangle_side_t
{
    T x;      // left-up x coordinate
    T y;      // left-up y coordinate
    T width;  // rectangle width
    T height; // rectangle height
};

template <typename T>
struct rectangle_coordinate_t
{
    T x1; // left-up x coordinate
    T y1; // left-up y coordinate
    T x2; // right-down x coordinate
    T y2; // right-down y coordinate
};

typedef kiss_fft_scalar scalar;
typedef kiss_fft_cpx cpx;

template <typename I, typename O, int N>
class Array
{
private:
    I *item = NULL;      // all items of array
    int *shape = NULL;   // the shape of array
    int length;          // the length of array
    bool do_free = true; // the flag to handle whether free the item or not

    /**
     * @brief 
     * 
     * @param point 
     */
    inline void is_alloc_success(void *point);

    /**
     * @brief 
     * 
     */
    inline void print_ram_capabilities();

    /**
     * @brief 
     * 
     */
    inline void malloc_item();

    /**
     * @brief 
     * 
     */
    inline void calloc_item();

    /**
     * @brief 
     * 
     */
    inline void malloc_shape();

    /**
     * @brief Set the shape object
     * 
     * @param shape 
     */
    inline void set_shape(int *shape);

    /**
     * @brief 
     * 
     * @param pixel 
     */
    inline void print_pixel(I pixel);

public:
    /////////////////////////////
    ////// Dimension = any //////
    /////////////////////////////

    /**
     * @brief Construct a new Array object
     * 
     */
    Array();

    /**
     * @brief Construct a new Array object
     * 
     * @param shape 
     * @param mode 
     */
    Array(int *shape, alloc_mode_t mode);

    /**
     * @brief Construct a new Array object
     * 
     * @param shape 
     * @param item 
     * @param do_copy 
     * @param do_free 
     */
    Array(int *shape, I *item, bool do_copy, bool do_free);

    /**
     * @brief Destroy the Array object
     * 
     */
    ~Array();

    /**
     * @brief Get the item object
     * 
     * @return I* 
     */
    I *get_item();

    /**
     * @brief Get the item object
     * 
     * @param i 
     * @return I 
     */
    I get_item(int i);

    /**
     * @brief Get the shape object
     * 
     * @return int* 
     */
    int *get_shape();

    /**
     * @brief Get the shape object
     * 
     * @param axis 
     * @return int 
     */
    int get_shape(int axis);

    /**
     * @brief Get the length object
     * 
     * @return int 
     */
    int get_length();

    /**
     * @brief 
     * 
     * @param shape 
     * @param mode 
     */
    void init(int *shape, alloc_mode_t mode);

    /**
     * @brief 
     * 
     * @param shape 
     * @param item 
     * @param do_copy 
     * @param do_free 
     */
    void init(int *shape, I *item, bool do_copy, bool do_free);

    /**
     * @brief Set the item object
     * 
     * @param item 
     * @param do_copy 
     * @param do_free 
     */
    inline void set_item(I *item, bool do_copy, bool do_free);

    /**
     * @brief 
     * 
     * @param cropped_rectangle 
     * @return O* 
     */
    O *crop(struct rectangle_side_t<int> cropped_rectangle);

    /**
     * @brief 
     * 
     * @param name 
     */
    void print_shape(const char *name);
    /**
     * @brief Get the index object
     * 
     * @param indices 
     * @return int 
     */
    inline int get_index(int *indices);

    ///////////////////////////
    ////// Dimension = 1 //////
    ///////////////////////////

    /**
     * @brief 
     * 
     * @param x_start 
     * @param x_end 
     * @param name 
     */
    void print_1d(int x_start, int x_end, char *name);

    ///////////////////////////
    ////// Dimension = 2 //////
    ///////////////////////////

    /**
     * @brief 
     * 
     * @param y_start 
     * @param y_end 
     * @param x_start 
     * @param x_end 
     * @param name 
     */
    void print_2d(int y_start, int y_end, int x_start, int x_end, char *name);

    /**
     * @brief 
     * 
     * @param y_start 
     * @param y_end 
     * @param x_start 
     * @param x_end 
     * @param print_pixel 
     * @param name 
     */
    void print_2d(int y_start, int y_end, int x_start, int x_end, void (*print_pixel)(I), char *name);

    /**
     * @brief Get the index 2d object
     * 
     * @param y 
     * @param x 
     * @return int 
     */
    inline int get_index_2d(int y, int x);

    /**
     * @brief Get the index 2d y object
     * 
     * @param y 
     * @return int 
     */
    inline int get_index_2d_y(int y);

    ///////////////////////////
    ////// Dimension = 3 //////
    ///////////////////////////

    /**
     * @brief 
     * 
     * @param y_start 
     * @param y_end 
     * @param x_start 
     * @param x_end 
     * @param c_start 
     * @param c_end 
     * @param name 
     * @param index_axis 
     */
    void print_3d(int y_start, int y_end, int x_start, int x_end, int c_start, int c_end, char *name, int index_axis);

    /**
     * @brief 
     * 
     * @param y_start 
     * @param y_end 
     * @param x_start 
     * @param x_end 
     * @param c_start 
     * @param c_end 
     * @param print_pixel 
     * @param name 
     * @param index_axis 
     */
    void print_3d(int y_start, int y_end, int x_start, int x_end, int c_start, int c_end, void (*print_pixel)(I), char *name, int index_axis);

    /**
     * @brief Get the index 3d object
     * 
     * @param y 
     * @param x 
     * @param c 
     * @return int 
     */
    inline int get_index_3d(int y, int x, int c);

    /**
     * @brief Get the index 3d y object
     * 
     * @param y 
     * @return int 
     */
    inline int get_index_3d_y(int y);

    /**
     * @brief Get the index 3d x object
     * 
     * @param x 
     * @return int 
     */
    inline int get_index_3d_x(int x);

    /**
     * @brief Get the index 3d y c object
     * 
     * @param y 
     * @param c 
     * @return int 
     */
    inline int get_index_3d_y_c(int y, int c);

    /**
     * @brief Get the index 3d x c object
     * 
     * @param x 
     * @param c 
     * @return int 
     */
    inline int get_index_3d_x_c(int x, int c);

    /**
     * @brief Get the index 3d y x object
     * 
     * @param y 
     * @param x 
     * @return int 
     */
    inline int get_index_3d_y_x(int y, int x);

    ///////////////////////////////
    ////// Override Operator //////
    ///////////////////////////////

    void operator=(Array<I, O, N> &array)
    {
        assert(array.length == this->length);

        set_shape(array.shape);
        this->item = array.item;
        array.do_free = false;
    }

    void operator+=(Array<I, O, N> &array)
    {
        assert(array.length == this->length);

        for (size_t i = 0; i < this->length; i++)
            this->item[i] += array.item[i];
    }

    void operator*=(Array<I, O, N> &array)
    {
        assert(array.length == this->length);

        for (size_t i = 0; i < this->length; i++)
            this->item[i] *= array.item[i];
    }
};

/////////////////////
////// Private //////
/////////////////////
template <typename I, typename O, int N>
inline void Array<I, O, N>::is_alloc_success(void *point)
{
    // print_ram_capabilities();
    assert(point != NULL);
}

template <typename I, typename O, int N>
inline void Array<I, O, N>::print_ram_capabilities()
{
    printf("Cap: RAM %d bytes | SPIRAM %d bytes --- Max Cap: RAM %d bytes | SPIRAM %d bytes\n",
           heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
           heap_caps_get_free_size(MALLOC_CAP_SPIRAM),
           heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
           heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
}

template <typename I, typename O, int N>
inline void Array<I, O, N>::malloc_shape()
{
    if (this->shape == NULL)
    {
        this->shape = (int *)malloc(N * sizeof(int));
        is_alloc_success(this->shape);
    }
}

template <typename I, typename O, int N>
inline void Array<I, O, N>::set_shape(int *shape)
{
    this->length = 1;
    for (size_t i = 0; i < N; i++)
    {
        this->length *= shape[i];
        this->shape[i] = shape[i];
    }
}

template <typename I, typename O, int N>
inline void Array<I, O, N>::malloc_item()
{
    if (this->item == NULL)
    {
#if MALLOC_IN_SPIRAM
        this->item = (I *)heap_caps_malloc(this->length * sizeof(I), MALLOC_CAP_SPIRAM);
#else
        this->item = (I *)malloc(this->length * sizeof(I));
#endif
        is_alloc_success(this->item);
    }
}

template <typename I, typename O, int N>
inline void Array<I, O, N>::calloc_item()
{
    if (this->item)
        free(this->item);

#if MALLOC_IN_SPIRAM
    this->item = (I *)heap_caps_calloc(this->length, sizeof(I), MALLOC_CAP_SPIRAM);
#else
    this->item = (I *)calloc(this->length, sizeof(I));
#endif
    is_alloc_success(this->item);
}

template <typename I, typename O, int N>
Array<I, O, N>::Array()
{
}

template <typename I, typename O, int N>
Array<I, O, N>::Array(int *shape, alloc_mode_t mode)
{
    init(shape, mode);
}

template <typename I, typename O, int N>
Array<I, O, N>::Array(int *shape, I *item, bool do_copy, bool do_free)
{
    init(shape, item, do_copy, do_free);
}

template <typename I, typename O, int N>
Array<I, O, N>::~Array()
{
    if (this->item && this->do_free)
        free(this->item);

    if (this->shape)
        free(this->shape);
}

template <typename I, typename O, int N>
I *Array<I, O, N>::get_item()
{
    return this->item;
}

template <typename I, typename O, int N>
I Array<I, O, N>::get_item(int i)
{
    return this->item[i];
}

template <typename I, typename O, int N>
int *Array<I, O, N>::get_shape()
{
    return this->shape;
}

template <typename I, typename O, int N>
int Array<I, O, N>::get_shape(int axis)
{
    return this->shape[axis];
}

template <typename I, typename O, int N>
int Array<I, O, N>::get_length()
{
    return this->length;
}

template <typename I, typename O, int N>
void Array<I, O, N>::init(int *shape, alloc_mode_t mode)
{
    malloc_shape();
    set_shape(shape);

    switch (mode)
    {
    case DO_MALLOC:
        malloc_item();
        break;

    case DO_CALLOC:
        calloc_item();
        break;

    default:
        break;
    }
}

template <typename I, typename O, int N>
void Array<I, O, N>::init(int *shape, I *item, bool do_copy, bool do_free)
{
    malloc_shape();
    set_shape(shape);
    set_item(item, do_copy, do_free);
}

template <typename I, typename O, int N>
inline void Array<I, O, N>::set_item(I *item, bool do_copy, bool do_free)
{
    if (do_copy)
    {
        malloc_item();
        memcpy(this->item, item, this->length * sizeof(I));
    }
    else
    {
        if (this->do_free && this->item)
            free(this->item);

        this->item = item;
    }

    this->do_free = do_free;
}

template <typename I, typename O, int N>
O *Array<I, O, N>::crop(struct rectangle_side_t<int> cropped_rectangle)
{
    assert(cropped_rectangle.y < this->shape[0]);
    assert(cropped_rectangle.x < this->shape[1]);

    int y2 = cropped_rectangle.y + cropped_rectangle.height - 1;
    assert(y2 >= 0);

    int x2 = cropped_rectangle.x + cropped_rectangle.width - 1;
    assert(x2 >= 0);

#if MALLOC_IN_SPIRAM
    O *item = (O *)heap_caps_calloc(cropped_rectangle.height * cropped_rectangle.width * this->shape[2], sizeof(I), MALLOC_CAP_SPIRAM);
#else
    O *item = (O *)calloc(cropped_rectangle.height * cropped_rectangle.width * this->shape[2], sizeof(I));
#endif
    is_alloc_success(item);

    int dst_y1;
    int dst_x1;
    int src_y_begining;
    int src_x_begining;

    if (cropped_rectangle.y < 0)
    {
        dst_y1 = -cropped_rectangle.y;
        src_y_begining = 0;
    }
    else
    {
        dst_y1 = 0;
        src_y_begining = cropped_rectangle.y;
    }

    if (cropped_rectangle.x < 0)
    {
        dst_x1 = -cropped_rectangle.x;
        src_x_begining = 0;
    }
    else
    {
        dst_x1 = 0;
        src_x_begining = cropped_rectangle.x;
    }

    int dst_y2_1_more = (y2 < this->shape[0]) ? (cropped_rectangle.height) : (this->shape[0] - cropped_rectangle.y); //index + 1
    int dst_x2_1_more = (x2 < this->shape[1]) ? (cropped_rectangle.width) : (this->shape[1] - cropped_rectangle.x);  //index + 1

    int copy_length = (dst_x2_1_more - dst_x1) * this->shape[2] * sizeof(I);

    O *p_dst = item;
    I *p_src = this->item;

    p_dst += ((dst_y1 * cropped_rectangle.width + dst_x1) * this->shape[2]);
    p_src += ((src_y_begining * this->shape[1] + src_x_begining) * this->shape[2]);

    for (size_t dst_y = dst_y1; dst_y < dst_y2_1_more; dst_y++)
    {
        for (size_t i = 0; i < copy_length; i++)
            p_dst[i] = p_src[i];

        p_dst += (cropped_rectangle.width * this->shape[2]);
        p_src += (this->shape[1] * this->shape[2]);
    }

    return item;
}

template <typename I, typename O, int N>
void Array<I, O, N>::print_shape(const char *name)
{
    if (name)
        printf(name);

    printf(":\nshape = (");

    for (size_t i = 0; i < N - 1; i++)
        printf("%d, ", this->shape[i]);

    printf("%d)\n", this->shape[N - 1]);
}

template <typename I, typename O, int N>
inline int Array<I, O, N>::get_index(int *indices)
{
    int index = 0;
    for (size_t i = 0; i < N - 1; i++)
    {
        index += indices[i];
        index *= this->shape[i + 1];
    }
    index += indices[N - 1];
    return index;
}

template <typename I, typename O, int N>
inline void Array<I, O, N>::print_pixel(I pixel)
{
    if (pixel < 0)
        printf("%.2e, ", (double)pixel);
    else
        printf(" %.2e, ", (double)pixel);
}

template <typename I, typename O, int N>
void Array<I, O, N>::print_1d(int x_start, int x_end, char *name)
{
    print_shape(name);

    x_start = max(x_start, 0);
    x_end = min(x_end, this->shape[0]);

    printf("indices = (%d:%d)\n", x_start, x_end);

    for (size_t x = x_start; x < x_end; x++)
        this->print_pixel(this->item[x]);
}

template <typename I, typename O, int N>
void Array<I, O, N>::print_2d(int y_start, int y_end, int x_start, int x_end, char *name)
{
    print_shape(name);

    int height = this->shape[0];
    int width = this->shape[1];

    y_start = max(y_start, 0);
    x_start = max(x_start, 0);

    y_end = min(y_end, height);
    x_end = min(x_end, width);

    printf("indices = (%d:%d, %d:%d)\n", y_start, y_end, x_start, x_end);

    for (size_t y = y_start; y < y_end; y++)
    {
        printf("[%d] ", y);

        for (size_t x = x_start; x < x_end; x++)
            this->print_pixel(this->item[y * width + x]);

        printf("\n");
    }
}

template <typename I, typename O, int N>
void Array<I, O, N>::print_2d(int y_start, int y_end, int x_start, int x_end, void (*print_pixel)(I), char *name)
{
    print_shape(name);

    int height = this->shape[0];
    int width = this->shape[1];

    y_start = max(y_start, 0);
    x_start = max(x_start, 0);

    y_end = min(y_end, height);
    x_end = min(x_end, width);

    printf("indices = (%d:%d, %d:%d)\n", y_start, y_end, x_start, x_end);

    for (size_t y = y_start; y < y_end; y++)
    {
        printf("[%d] ", y);

        for (size_t x = x_start; x < x_end; x++)
            print_pixel(this->item[y * width + x]);

        printf("\n");
    }
}

template <typename I, typename O, int N>
inline int Array<I, O, N>::get_index_2d(int y, int x)
{
    assert(y < this->shape[0]);
    assert(x < this->shape[1]);
    return y * this->shape[1] + x;
}

template <typename I, typename O, int N>
inline int Array<I, O, N>::get_index_2d_y(int y)
{
    assert(y < this->shape[0]);
    return y * this->shape[1];
}

template <typename I, typename O, int N>
void Array<I, O, N>::print_3d(int y_start, int y_end, int x_start, int x_end, int c_start, int c_end, char *name, int index_axis)
{
    print_shape(name);

    int height = this->shape[0];
    int width = this->shape[1];
    int channel = this->shape[2];

    y_start = max(y_start, 0);
    x_start = max(x_start, 0);
    c_start = max(c_start, 0);

    y_end = min(y_end, height);
    x_end = min(x_end, width);
    c_end = min(c_end, channel);

    printf("indices = (%d:%d, %d:%d, %d:%d)\n", y_start, y_end, x_start, x_end, c_start, c_end);

    for (size_t y = y_start; y < y_end; y++)
    {
        if (index_axis == 0)
            printf("[%d] ", y);

        for (size_t x = x_start; x < x_end; x++)
        {
            if (index_axis == 1)
                printf("[%d] ", x);

            for (size_t c = c_start; c < c_end; c++)
                this->print_pixel(this->item[(y * width + x) * channel + c]);

            if (c_end - c_start > 1)
                printf("\n");
        }
        printf("\n");
    }
}

template <typename I, typename O, int N>
void Array<I, O, N>::print_3d(int y_start, int y_end, int x_start, int x_end, int c_start, int c_end, void (*print_pixel)(I), char *name, int index_axis)
{
    print_shape(name);

    int height = this->shape[0];
    int width = this->shape[1];
    int channel = this->shape[2];

    y_start = max(y_start, 0);
    x_start = max(x_start, 0);
    c_start = max(c_start, 0);

    y_end = min(y_end, height);
    x_end = min(x_end, width);
    c_end = min(c_end, channel);

    printf("indices = (%d:%d, %d:%d, %d:%d)\n", y_start, y_end, x_start, x_end, c_start, c_end);

    for (size_t y = y_start; y < y_end; y++)
    {
        if (index_axis == 0)
            printf("[%d] ", y);

        for (size_t x = x_start; x < x_end; x++)
        {
            if (index_axis == 1)
                printf("[%d] ", x);

            for (size_t c = c_start; c < c_end; c++)
                print_pixel(this->item[(y * width + x) * channel + c]);

            if (c_end - c_start > 1)
                printf("\n");
        }
        printf("\n");
    }
}

template <typename I, typename O, int N>
inline int Array<I, O, N>::get_index_3d(int y, int x, int c)
{
    assert(y < this->shape[0]);
    assert(x < this->shape[1]);
    assert(c < this->shape[2]);
    return (y * this->shape[1] + x) * this->shape[2] + c;
}

template <typename I, typename O, int N>
inline int Array<I, O, N>::get_index_3d_y(int y)
{
    assert(y < this->shape[0]);
    return y * this->shape[1] * this->shape[2];
}

template <typename I, typename O, int N>
inline int Array<I, O, N>::get_index_3d_x(int x)
{
    assert(x < this->shape[1]);
    return x * this->shape[2];
}

template <typename I, typename O, int N>
inline int Array<I, O, N>::get_index_3d_y_c(int y, int c)
{
    assert(y < this->shape[0]);
    assert(c < this->shape[2]);
    return y * this->shape[1] * this->shape[2] + c;
}

template <typename I, typename O, int N>
inline int Array<I, O, N>::get_index_3d_x_c(int x, int c)
{
    assert(x < this->shape[1]);
    assert(c < this->shape[2]);
    return x * this->shape[2] + c;
}

template <typename I, typename O, int N>
inline int Array<I, O, N>::get_index_3d_y_x(int y, int x)
{
    assert(y < this->shape[0]);
    assert(x < this->shape[1]);
    return (y * this->shape[1] + x) * this->shape[2];
}
