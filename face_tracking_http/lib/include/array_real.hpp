#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#include "array.hpp"
#include "array_imaginary.hpp"

template <typename T>
struct value_index_t
{
    int index;
    T value;
};

template <typename I, typename O, int N>
class ArrayReal : public Array<I, O, N>
{

private:
public:
    /////////////////////////////
    ////// Dimension = any //////
    /////////////////////////////

    /**
     * @brief Construct a new Array Real object
     * 
     */
    ArrayReal();

    /**
     * @brief Construct a new Array Real object
     * 
     * @param shape 
     * @param mode 
     */
    ArrayReal(int *shape, alloc_mode_t mode);

    /**
     * @brief Construct a new Array Real object
     * 
     * @param shape 
     * @param item 
     * @param do_copy 
     * @param do_free 
     */
    ArrayReal(int *shape, I *item, bool do_copy, bool do_free);

    /**
     * @brief Destroy the Array Real object
     * 
     */
    ~ArrayReal();

    ///////////////////////////
    ////// Dimension = 2 //////
    ///////////////////////////

    /**
     * @brief load hanning window to item. Dimension 1 and 2 are supported only
     * 
     */
    void load_hanning_window();

    ///////////////////////////
    ////// Dimension = 3 //////
    ///////////////////////////

    /**
     * @brief 
     * 
     * @param dst_item 
     * @param cropped_rectangle 
     * @param resized_shape 
     */
    void crop_resize(O *dst_item, int *dst_shape, struct rectangle_side_t<int> cropped_rectangle);

    /**
     * @brief 
     * 
     * @param dst_item 
     * @param cropped_rectangle 
     * @param resized_shape 
     * @param resized_pixel_op 
     */
    void crop_resize_pixel(O *dst_item, int *dst_shape, struct rectangle_side_t<int> cropped_rectangle, void (*resized_pixel_op)(O *, O *));

    /**
     * @brief 
     * 
     * @param dst_item 
     * @param dst_shape 
     * @param cropped_rectangle 
     * @param resized_pixel_op 
     * @param weight 
     * @param weight_channel 
     */
    void crop_resize_pixel_weight(O *dst_item, int *dst_shape, struct rectangle_side_t<int> cropped_rectangle, void (*resized_pixel_op)(O *, O *, O *), O *weight, int weight_channel);

    /**
     * @brief 
     * 
     * @param dst_item 
     * @param dst_shape 
     * @param cropped_rectangle 
     * @param resized_pixel_op 
     * @param weight 
     * @param weight_channel 
     */
    void crop_resize_pixel_weight(cpx *dst_item, int *dst_shape, struct rectangle_side_t<int> cropped_rectangle, void (*resized_pixel_op)(cpx *, O *, O *), O *weight, int weight_channel);
};

template <typename I, typename O, int N>
ArrayReal<I, O, N>::ArrayReal()
{
}

template <typename I, typename O, int N>
ArrayReal<I, O, N>::ArrayReal(int *shape, alloc_mode_t mode)
{
    Array<I, O, N>::init(shape, mode);
}

template <typename I, typename O, int N>
ArrayReal<I, O, N>::ArrayReal(int *shape, I *item, bool do_copy, bool do_free)
{
    Array<I, O, N>::init(shape, item, do_copy, do_free);
}

template <typename I, typename O, int N>
ArrayReal<I, O, N>::~ArrayReal()
{
}

///////////////////////////
////// Dimension = 2 //////
///////////////////////////

template <typename I, typename O, int N>
void ArrayReal<I, O, N>::load_hanning_window()
{
    if (1 == N)
    {
        for (size_t i = 0; i < this->get_length(); i++)
            this->get_item()[i] = 0.5 * (1 - cos(2 * PI * i / (this->get_length() - 1)));
    }
    else if (2 == N)
    {
        I *v = (I *)malloc(sizeof(I) * this->get_shape()[0]);
        I *h = (I *)malloc(sizeof(I) * this->get_shape()[1]);

        if (this->get_shape()[0] == this->get_shape()[1])
        {
            for (size_t i = 0; i < this->get_shape()[0]; i++)
            {
                v[i] = 0.5 * (1 - cos(2 * PI * i / (this->get_shape()[0] - 1)));
                h[i] = 0.5 * (1 - cos(2 * PI * i / (this->get_shape()[0] - 1)));
            }
        }
        else
        {
            for (size_t i = 0; i < this->get_shape()[0]; i++)
                v[i] = 0.5 * (1.0 - cos(2.0 * PI * i / (this->get_shape()[0] - 1.0)));

            for (size_t i = 0; i < this->get_shape()[1]; i++)
                h[i] = 0.5 * (1.0 - cos(2.0 * PI * i / (this->get_shape()[1] - 1.0)));
        }

        I *p_item = this->get_item();
        for (size_t y = 0; y < this->get_shape()[0]; y++)
            for (size_t x = 0; x < this->get_shape()[1]; x++)
                *p_item++ = v[y] * h[x];

        free(v);
        free(h);
    }
}

///////////////////////////
////// Dimension = 3 //////
///////////////////////////

template <typename I, typename O, int N>
/**
 * @brief 
 * 
 * @param dst_item 
 * @param dst_shape 
 * @param cropped_rectangle 
 */
void ArrayReal<I, O, N>::crop_resize(O *dst_item, int *dst_shape, struct rectangle_side_t<int> cropped_rectangle)
{
    int y2 = cropped_rectangle.y + cropped_rectangle.height - 1;
    int x2 = cropped_rectangle.x + cropped_rectangle.width - 1;

    assert(cropped_rectangle.y < this->get_shape()[0]);
    assert(cropped_rectangle.x < this->get_shape()[1]);
    assert(y2 >= 0);
    assert(x2 >= 0);

    float scale_x = (float)cropped_rectangle.width / dst_shape[1];
    float scale_y = (float)cropped_rectangle.height / dst_shape[0];

    O *p_dst_item = dst_item;

    for (size_t dst_y = 0; dst_y < dst_shape[0]; dst_y++)
    {
        struct point_coordinate_t<float> src_point;
        struct point_coordinate_t<int> src_point1;

        src_point.y = ((float)dst_y + 0.5) * scale_y - 0.5;
        src_point1.y = (int)src_point.y;
        // assert(src_point1.y >= 0);

        float weight_down = src_point.y - src_point1.y;
        float weight_up = 1.0 - weight_down;

        src_point1.y = (src_point1.y == cropped_rectangle.height - 1) ? (cropped_rectangle.height - 2) : (src_point1.y);
        src_point1.y += cropped_rectangle.y;

        if (src_point1.y < -1 || src_point1.y > this->get_shape()[0] - 1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    *p_dst_item++ = 255;
            }
        }
        else if (src_point1.y == -1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                        *p_dst_item++ = 255;
                }
                else if (src_point1.x == -1)
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        O pixel = this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        *p_dst_item++ = round(pixel);
                    }
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        O pixel = this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left; // down left
                        *p_dst_item++ = round(pixel);
                    }
                }
                else
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        O pixel = this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;     // down left
                        pixel += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        *p_dst_item++ = round(pixel);
                    }
                }
            }
        }
        else if (src_point1.y == this->get_shape()[0] - 1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                        *p_dst_item++ = 255;
                }
                else if (src_point1.x == -1)
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        O pixel = this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        *p_dst_item++ = round(pixel);
                    }
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        O pixel = this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left; // up left
                        *p_dst_item++ = round(pixel);
                    }
                }
                else
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        O pixel = this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;     // up left
                        pixel += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        *p_dst_item++ = round(pixel);
                    }
                }
            }
        }
        else
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                        *p_dst_item++ = 255;
                }
                else if (src_point1.x == -1)
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        O pixel = this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;     // down left
                        pixel += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        *p_dst_item++ = round(pixel);
                    }
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        O pixel = this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;     // up left
                        pixel += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        *p_dst_item++ = round(pixel);
                    }
                }
                else
                {
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        O pixel = this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;             // up left
                        pixel += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right;         // up right
                        pixel += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;      // down left
                        pixel += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        *p_dst_item++ = round(pixel);
                    }
                }
            }
        }
    }
}

template <typename I, typename O, int N>
void ArrayReal<I, O, N>::crop_resize_pixel(O *dst_item, int *dst_shape, struct rectangle_side_t<int> cropped_rectangle, void (*resized_pixel_op)(O *, O *))
{
    int y2 = cropped_rectangle.y + cropped_rectangle.height - 1;
    int x2 = cropped_rectangle.x + cropped_rectangle.width - 1;

    assert(cropped_rectangle.y < this->get_shape()[0]);
    assert(cropped_rectangle.x < this->get_shape()[1]);
    assert(y2 >= 0);
    assert(x2 >= 0);

    float scale_x = (float)cropped_rectangle.width / dst_shape[1];
    float scale_y = (float)cropped_rectangle.height / dst_shape[0];

    O *p_dst_item = dst_item;

    for (size_t dst_y = 0; dst_y < dst_shape[0]; dst_y++)
    {
        struct point_coordinate_t<float> src_point;
        struct point_coordinate_t<int> src_point1;

        src_point.y = ((float)dst_y + 0.5) * scale_y - 0.5;
        src_point1.y = (int)src_point.y;
        // assert(src_point1.y >= 0);

        float weight_down = src_point.y - src_point1.y;
        float weight_up = 1.0 - weight_down;

        src_point1.y = (src_point1.y == cropped_rectangle.height - 1) ? (cropped_rectangle.height - 2) : (src_point1.y);
        src_point1.y += cropped_rectangle.y;

        if (src_point1.y < -1 || src_point1.y > this->get_shape()[0] - 1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                resized_pixel_op(p_dst_item, NULL);
                p_dst_item += dst_shape[2];
            }
        }
        else if (src_point1.y == -1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    resized_pixel_op(p_dst_item, NULL);
                    p_dst_item += dst_shape[2];
                }
                else if (src_point1.x == -1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel);
                    p_dst_item += dst_shape[2];
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left; // down left
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel);
                    p_dst_item += dst_shape[2];
                }
                else
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;      // down left
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel);
                    p_dst_item += dst_shape[2];
                }
            }
        }
        else if (src_point1.y == this->get_shape()[0] - 1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    resized_pixel_op(p_dst_item, NULL);
                    p_dst_item += dst_shape[2];
                }
                else if (src_point1.x == -1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel);
                    p_dst_item += dst_shape[2];
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left; // up left
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel);
                    p_dst_item += dst_shape[2];
                }
                else
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;      // up left
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel);
                    p_dst_item += dst_shape[2];
                }
            }
        }
        else
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    resized_pixel_op(p_dst_item, NULL);
                    p_dst_item += dst_shape[2];
                }
                else if (src_point1.x == -1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;      // down left
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel);
                    p_dst_item += dst_shape[2];
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;      // up left
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel);
                    p_dst_item += dst_shape[2];
                }
                else
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;              // up left
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right;         // up right
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;      // down left
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel);
                    p_dst_item += dst_shape[2];
                }
            }
        }
    }
}

template <typename I, typename O, int N>
void ArrayReal<I, O, N>::crop_resize_pixel_weight(O *dst_item, int *dst_shape, struct rectangle_side_t<int> cropped_rectangle, void (*resized_pixel_op)(O *, O *, O *), O *weight, int weight_channel)
{
    int y2 = cropped_rectangle.y + cropped_rectangle.height - 1;
    int x2 = cropped_rectangle.x + cropped_rectangle.width - 1;

    assert(cropped_rectangle.y < this->get_shape()[0]);
    assert(cropped_rectangle.x < this->get_shape()[1]);
    assert(y2 >= 0);
    assert(x2 >= 0);

    float scale_x = (float)cropped_rectangle.width / dst_shape[1];
    float scale_y = (float)cropped_rectangle.height / dst_shape[0];

    O *p_dst_item = dst_item;
    O *p_weight_item = weight;

    for (size_t dst_y = 0; dst_y < dst_shape[0]; dst_y++)
    {
        struct point_coordinate_t<float> src_point;
        struct point_coordinate_t<int> src_point1;

        src_point.y = ((float)dst_y + 0.5) * scale_y - 0.5;
        src_point1.y = (int)src_point.y;
        // assert(src_point1.y >= 0);

        float weight_down = src_point.y - src_point1.y;
        float weight_up = 1.0 - weight_down;

        src_point1.y = (src_point1.y == cropped_rectangle.height - 1) ? (cropped_rectangle.height - 2) : (src_point1.y);
        src_point1.y += cropped_rectangle.y;

        if (src_point1.y < -1 || src_point1.y > this->get_shape()[0] - 1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                resized_pixel_op(p_dst_item, NULL, p_weight_item);
                p_dst_item += dst_shape[2];
                p_weight_item += weight_channel;
            }
        }
        else if (src_point1.y == -1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    resized_pixel_op(p_dst_item, NULL, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == -1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left; // down left
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;      // down left
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
            }
        }
        else if (src_point1.y == this->get_shape()[0] - 1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    resized_pixel_op(p_dst_item, NULL, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == -1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left; // up left
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;      // up left
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
            }
        }
        else
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    resized_pixel_op(p_dst_item, NULL, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == -1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;      // down left
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;      // up left
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;              // up left
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right;         // up right
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;      // down left
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
            }
        }
    }
}

template <typename I, typename O, int N>
void ArrayReal<I, O, N>::crop_resize_pixel_weight(cpx *dst_item, int *dst_shape, struct rectangle_side_t<int> cropped_rectangle, void (*resized_pixel_op)(cpx *, O *, O *), O *weight, int weight_channel)
{
    int y2 = cropped_rectangle.y + cropped_rectangle.height - 1;
    int x2 = cropped_rectangle.x + cropped_rectangle.width - 1;

    assert(cropped_rectangle.y < this->get_shape()[0]);
    assert(cropped_rectangle.x < this->get_shape()[1]);
    assert(y2 >= 0);
    assert(x2 >= 0);

    float scale_x = (float)cropped_rectangle.width / dst_shape[1];
    float scale_y = (float)cropped_rectangle.height / dst_shape[0];

    cpx *p_dst_item = dst_item;
    O *p_weight_item = weight;

    for (size_t dst_y = 0; dst_y < dst_shape[0]; dst_y++)
    {
        struct point_coordinate_t<float> src_point;
        struct point_coordinate_t<int> src_point1;

        src_point.y = ((float)dst_y + 0.5) * scale_y - 0.5;
        src_point1.y = (int)src_point.y;
        // assert(src_point1.y >= 0);

        float weight_down = src_point.y - src_point1.y;
        float weight_up = 1.0 - weight_down;

        src_point1.y = (src_point1.y == cropped_rectangle.height - 1) ? (cropped_rectangle.height - 2) : (src_point1.y);
        src_point1.y += cropped_rectangle.y;

        if (src_point1.y < -1 || src_point1.y > this->get_shape()[0] - 1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                resized_pixel_op(p_dst_item, NULL, p_weight_item);
                p_dst_item += dst_shape[2];
                p_weight_item += weight_channel;
            }
        }
        else if (src_point1.y == -1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    resized_pixel_op(p_dst_item, NULL, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == -1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left; // down left
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;      // down left
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
            }
        }
        else if (src_point1.y == this->get_shape()[0] - 1)
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    resized_pixel_op(p_dst_item, NULL, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == -1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left; // up left
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;      // up left
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
            }
        }
        else
        {
            for (size_t dst_x = 0; dst_x < dst_shape[1]; dst_x++)
            {
                src_point.x = ((float)dst_x + 0.5) * scale_x - 0.5;
                src_point1.x = (int)src_point.x;
                // assert(src_point1.x >= 0);

                float weight_right = src_point.x - src_point1.x;
                float weight_left = 1.0 - weight_right;

                src_point1.x = (src_point1.x == cropped_rectangle.width - 1) ? (cropped_rectangle.width - 2) : (src_point1.x);
                src_point1.x += cropped_rectangle.x;

                if (src_point1.x < -1 || src_point1.x > this->get_shape()[1] - 1)
                {
                    resized_pixel_op(p_dst_item, NULL, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == -1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;      // down left
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else if (src_point1.x == this->get_shape()[1] - 1)
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;      // up left
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right; // up right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
                else
                {
                    O pixel[3] = {0};
                    for (size_t dst_c = 0; dst_c < this->get_shape()[2]; dst_c++)
                    {
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_up * weight_left;              // up left
                        pixel[dst_c] += this->get_item()[(src_point1.y * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_up * weight_right;         // up right
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x) * this->get_shape()[2] + dst_c] * weight_down * weight_left;      // down left
                        pixel[dst_c] += this->get_item()[((src_point1.y + 1) * this->get_shape()[1] + src_point1.x + 1) * this->get_shape()[2] + dst_c] * weight_down * weight_right; // down right
                        pixel[dst_c] = round(pixel[dst_c]);
                    }
                    resized_pixel_op(p_dst_item, pixel, p_weight_item);
                    p_dst_item += dst_shape[2];
                    p_weight_item += weight_channel;
                }
            }
        }
    }
}
