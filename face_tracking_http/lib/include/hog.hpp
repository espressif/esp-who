#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#include "array_real.hpp"
#include "array_imaginary.hpp"

#define CONFIG_HOG_SECTOR_NUMBER 9
#define CONFIG_HOG_DEBUG 0

typedef struct
{
    int sector_index[2];
    scalar value;
} gradient_point_t;

template <typename I, typename O>
class HOG
{
private:
    ArrayReal<O, O, 1> cos_angle;                 // Cos of each angle sectors
    ArrayReal<O, O, 1> sin_angle;                 // Sin of each angle sectors
    int cell_size;                                // HOG cell size
    int sector_number;                            // PI will be divided by the sector number
    ArrayReal<int, int, 1> surround_index_offset; // The surrounding HOG cell offset
    ArrayReal<O, O, 1> weight;                    //
    ArrayReal<O, O, 1> weight_surround;           //
    int image_up_down_offset;                     // the index offset between up and down
    int image_up_left_offset;                     // the index offset between up and left
    int image_up_right_offset;                    // the index offset between up and right

    /**
     * @brief Set the hog1 chw cell object without any HOG size exceeding judgement
     * 
     * @param hog 
     * @param hog_y 
     * @param hog_x 
     * @param image 
     * @param image_up_y 
     * @param image_up_x 
     * @param next_row_stride 
     * @param debug 
     */
    inline void set_hog_1_chw_cell(ArrayImaginary<3> &hog,
                                   int hog_y, int hog_x,
                                   ArrayReal<I, O, 3> &image,
                                   int image_up_y, int image_up_x,
                                   int next_row_stride
#if CONFIG_HOG_DEBUG
                                   ,
                                   ArrayReal<I, O, 3> &debug
#endif
    );

    /**
     * @brief Set the hog1 chw cell object
     * 
     * @param hog 
     * @param hog_y 
     * @param hog_x 
     * @param image 
     * @param image_up_y 
     * @param image_up_x 
     * @param next_row_stride 
     * @param cell_y_start 
     * @param cell_y_end 
     * @param cell_x_start 
     * @param cell_x_end 
     * @param debug 
     */
    inline void set_hog_1_chw_cell(ArrayImaginary<3> &hog,
                                   int hog_y, int hog_x,
                                   ArrayReal<I, O, 3> &image,
                                   int image_up_y, int image_up_x,
                                   int next_row_stride,
                                   int cell_y_start, int cell_y_end,
                                   int cell_x_start, int cell_x_end
#if CONFIG_HOG_DEBUG
                                   ,
                                   ArrayReal<I, O, 3> &debug
#endif
    );

    /**
     * @brief Set the hog1 chw cell object without any HOG size exceeding judgement
     * 
     * @param hog 
     * @param hog_y 
     * @param hog_x 
     * @param image 
     * @param image_up_y 
     * @param image_up_x 
     * @param next_row_stride 
     * @param debug 
     */
    inline void set_hog_1_chw_cell(ArrayReal<O, O, 3> &hog,
                                   int hog_y, int hog_x,
                                   ArrayReal<I, O, 3> &image,
                                   int image_up_y, int image_up_x,
                                   int next_row_stride
#if CONFIG_HOG_DEBUG
                                   ,
                                   ArrayReal<I, O, 3> &debug
#endif
    );

    /**
     * @brief Set the hog1 chw cell object
     * 
     * @param hog 
     * @param hog_y 
     * @param hog_x 
     * @param image 
     * @param image_up_y 
     * @param image_up_x 
     * @param next_row_stride 
     * @param cell_y_start 
     * @param cell_y_end 
     * @param cell_x_start 
     * @param cell_x_end 
     * @param debug 
     */
    inline void set_hog_1_chw_cell(ArrayReal<O, O, 3> &hog,
                                   int hog_y, int hog_x,
                                   ArrayReal<I, O, 3> &image,
                                   int image_up_y, int image_up_x,
                                   int next_row_stride,
                                   int cell_y_start, int cell_y_end,
                                   int cell_x_start, int cell_x_end
#if CONFIG_HOG_DEBUG
                                   ,
                                   ArrayReal<I, O, 3> &debug
#endif
    );

    /**
     * @brief Set the hog1 chw cell object without any HOG size exceeding judgement
     * 
     * @param hog 
     * @param hog_y 
     * @param hog_x 
     * @param image 
     * @param image_up_y 
     * @param image_up_x 
     * @param next_row_stride 
     * @param debug 
     */
    inline void set_hog_1_hwc_cell(ArrayReal<O, O, 3> &hog,
                                   int hog_y, int hog_x,
                                   ArrayReal<I, O, 3> &image,
                                   int image_up_y, int image_up_x,
                                   int next_row_stride
#if CONFIG_HOG_DEBUG
                                   ,
                                   ArrayReal<I, O, 3> &debug
#endif
    );

    /**
     * @brief Set the hog1 chw cell object
     * 
     * @param hog 
     * @param hog_y 
     * @param hog_x 
     * @param image 
     * @param image_up_y 
     * @param image_up_x 
     * @param next_row_stride 
     * @param cell_y_start 
     * @param cell_y_end 
     * @param cell_x_start 
     * @param cell_x_end 
     * @param debug 
     */
    inline void set_hog_1_hwc_cell(ArrayReal<O, O, 3> &hog,
                                   int hog_y, int hog_x,
                                   ArrayReal<I, O, 3> &image,
                                   int image_up_y, int image_up_x,
                                   int next_row_stride,
                                   int cell_y_start, int cell_y_end,
                                   int cell_x_start, int cell_x_end
#if CONFIG_HOG_DEBUG
                                   ,
                                   ArrayReal<I, O, 3> &debug
#endif
    );

    /**
     * @brief Get the max gradient and sector index object
     * 
     * @param image 
     * @return Array<gradient_point_t, gradient_point_t, 2> 
     */
    Array<gradient_point_t, gradient_point_t, 2> get_max_gradient_and_sector_index(ArrayReal<I, O, 3> &image);

public:
    /**
     * @brief Construct a new HOG object
     * 
     */
    HOG();

    /**
     * @brief Construct a new HOG object
     * 
     * @param cell_size 
     * @param sector_number 
     */
    HOG(int cell_size, int sector_number);

    /**
     * @brief Destroy the HOG object
     * 
     */
    ~HOG();

    /**
     * @brief 
     * 
     * @param cell_size 
     * @param sector_number 
     */
    void init(int cell_size, int sector_number);

    /**
     * @brief Set the hog1 chw object
     * 
     * @param image 
     * @param hog 
     */
    void set_hog_1_chw(ArrayReal<I, O, 3> &image, ArrayImaginary<3> &hog);

    /**
     * @brief Set the hog1 chw object
     * 
     * @param image 
     * @param hog 
     */
    void set_hog_1_chw(ArrayReal<I, O, 3> &image, ArrayReal<O, O, 3> &hog);

    /**
     * @brief Set the hog1 chw object
     * 
     * @param image 
     * @param hog 
     */
    void set_hog_1_hwc(ArrayReal<I, O, 3> &image, ArrayReal<O, O, 3> &hog);

    /**
     * @brief 
     * 
     * @param n_t_hog 
     * @param raw_hog 
     * @param threshold 
     */
    void normalize_and_truncate(ArrayImaginary<3> &n_t_hog, ArrayReal<O, O, 3> &raw_hog, O threshold);

    /**
     * @brief Set the hog 1 chw with normalize truncate object
     * 
     * @param hog 
     * @param image 
     * @param threshold 
     */
    void set_hog_1_chw_with_normalize_truncate(ArrayImaginary<3> &hog, ArrayReal<I, O, 3> &image, O threshold);

    /**
     * @brief Get the hog 1 chw with normalize truncate object
     * 
     * @param image 
     * @param threshold 
     * @return ArrayImaginary<3> 
     */
    ArrayImaginary<3> get_hog_1_chw_with_normalize_truncate(ArrayReal<I, O, 3> &image, O threshold);
};

template <typename I, typename O>
HOG<I, O>::HOG(/* args */)
{
}

template <typename I, typename O>
HOG<I, O>::HOG(int cell_size, int sector_number)
{
    init(cell_size, sector_number);
}

template <typename I, typename O>
HOG<I, O>::~HOG()
{
}

template <typename I, typename O>
void HOG<I, O>::init(int cell_size, int sector_number)
{
    this->cell_size = cell_size;
    this->sector_number = sector_number;

    // TODO: Cos(Angle), Sin(Angle)
    int angle_shape[] = {sector_number};
    this->cos_angle.init(angle_shape, DO_MALLOC);
    this->sin_angle.init(angle_shape, DO_MALLOC);
    for (size_t i = 0; i < sector_number; i++)
    {
        O angle = PI / sector_number * i;
        this->cos_angle.get_item()[i] = cos(angle);
        this->sin_angle.get_item()[i] = sin(angle);
    }

    // TODO: Nearest Index offset
    int cell_length[] = {this->cell_size};
    this->surround_index_offset.init(cell_length, DO_MALLOC);

    // TODO: Initialize weight
    this->weight.init(cell_length, DO_MALLOC);
    this->weight_surround.init(cell_length, DO_MALLOC);
    for (size_t i = 0; i < this->cell_size / 2.0; i++)
    {
        this->surround_index_offset.get_item()[i] = -1;
        this->surround_index_offset.get_item()[this->cell_size - 1 - i] = 1;

        O a = this->cell_size / 2.0 - i - 0.5;
        O b = this->cell_size / 2.0 + i + 0.5;

        this->weight.get_item()[i] = b / (a + b);
        this->weight.get_item()[this->cell_size - 1 - i] = this->weight.get_item(i);

        this->weight_surround.get_item()[i] = a / (a + b);
        this->weight_surround.get_item()[this->cell_size - 1 - i] = this->weight_surround.get_item(i);
    }
}

template <typename I, typename O>
Array<gradient_point_t, gradient_point_t, 2> HOG<I, O>::get_max_gradient_and_sector_index(ArrayReal<I, O, 3> &image)
{
    // TODO: init
    Array<gradient_point_t, gradient_point_t, 2> gradient(image.get_shape(), DO_CALLOC);

    gradient_point_t *p_gradient = gradient.get_item() + gradient.get_shape(1) + 1; //(y, x) = (1, 1)
    I *up = image.get_item() + image.get_shape(2);                                  //(y, x) = (0, 1)
    I *down = up + 2 * image.get_shape(1) * image.get_shape(2);                     //(y, x) = (2, 1)
    I *left = image.get_item() + image.get_shape(1) * image.get_shape(2);           //(y, x) = (1, 0)
    I *right = left + image.get_shape(2) + image.get_shape(2);                      //(y, x) = (1, 2)

    for (size_t image_y = 1; image_y < image.get_shape(0) - 1; image_y++)
    {
        for (size_t image_x = 1; image_x < image.get_shape(1) - 1; image_x++)
        {
            // TODO: get the max gradient amplitude among image channels
            O max_gradient_y = *down++ - *up++;
            O max_gradient_x = *right++ - *left++;
            p_gradient->value = sqrt(max_gradient_y * max_gradient_y + max_gradient_x * max_gradient_x);

            for (size_t image_c = 1; image_c < image.get_shape(2); image_c++)
            {
                O gradient_y = *down++ - *up++;
                O gradient_x = *right++ - *left++;
                O gradient = sqrt(gradient_y * gradient_y + gradient_x * gradient_x);
                if (gradient > p_gradient->value)
                {
                    p_gradient->value = gradient;
                    max_gradient_y = gradient_y;
                    max_gradient_x = gradient_x;
                }
            }

            // TODO: get the closet sector
            O max_projection = this->cos_angle.get_item(0) * max_gradient_x + this->sin_angle.get_item(0) * max_gradient_y;
            p_gradient->sector_index[1] = 0;
            for (size_t sector_i = 1; sector_i < this->sector_number; sector_i++)
            {
                O projection = this->cos_angle.get_item(sector_i) * max_gradient_x + this->sin_angle.get_item(sector_i) * max_gradient_y;
                if (projection > max_projection)
                {
                    max_projection = projection;
                    p_gradient->sector_index[1] = sector_i;
                }
                else if (-projection > max_projection)
                {
                    max_projection = -projection;
                    p_gradient->sector_index[1] = sector_i + this->sector_number;
                }
            }
            p_gradient->sector_index[0] = p_gradient->sector_index[1] % this->sector_number;
            p_gradient++;
        }
        up += (image.get_shape(2) + image.get_shape(2));
        down += (image.get_shape(2) + image.get_shape(2));
        left += (image.get_shape(2) + image.get_shape(2));
        right += (image.get_shape(2) + image.get_shape(2));
        p_gradient += 2;
    }

    return gradient;
}

template <typename I, typename O>
inline void HOG<I, O>::set_hog_1_chw_cell(ArrayImaginary<3> &hog,
                                          int hog_y, int hog_x,
                                          ArrayReal<I, O, 3> &image,
                                          int image_up_y, int image_up_x,
                                          int next_row_stride
#if CONFIG_HOG_DEBUG
                                          ,
                                          ArrayReal<I, O, 3> &debug
#endif
)
{
    I *image_up = image.get_item() + image.get_index_3d_y_x(image_up_y, image_up_x);
    I *image_down = image_up + this->image_up_down_offset;
    I *image_left = image_up + this->image_up_left_offset;
    I *image_right = image_up + this->image_up_right_offset;

    for (size_t cell_y = 0; cell_y < this->cell_size; cell_y++)
    {
        for (size_t cell_x = 0; cell_x < this->cell_size; cell_x++)
        {
            // TODO: Find the max gradient among each channel, keep its vertical and horizontal gradient
            O max_gradient_x = *image_right++ - *image_left++;
            O max_gradient_y = *image_down++ - *image_up++;
            O max_gradient = sqrt(max_gradient_x * max_gradient_x + max_gradient_y * max_gradient_y);
            for (size_t image_c = 1; image_c < image.get_shape(2); image_c++)
            {
                O gradient_x = *image_right++ - *image_left++;
                O gradient_y = *image_down++ - *image_up++;
                O gradient = sqrt(gradient_x * gradient_x + gradient_y * gradient_y);

                if (gradient > max_gradient)
                {
                    max_gradient = gradient;
                    max_gradient_x = gradient_x;
                    max_gradient_y = gradient_y;
                }
            }

            // TODO: Find the orientation
            O max_orientation_value = this->cos_angle.get_item(0) * max_gradient_x + this->sin_angle.get_item(0) * max_gradient_y;
            int max_orientation_index = 0;

            for (size_t sector_i = 0; sector_i < this->sector_number; sector_i++)
            {
                O orientation_value = this->cos_angle.get_item(sector_i) * max_gradient_x + this->sin_angle.get_item(sector_i) * max_gradient_y;

                if (orientation_value > max_orientation_value)
                {
                    max_orientation_value = orientation_value;
                    max_orientation_index = sector_i;
                }
                else if (-orientation_value > max_orientation_value)
                {
                    max_orientation_value = -orientation_value;
                    max_orientation_index = sector_i + this->sector_number;
                }
            }

            // TODO: Accumlate
            hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y, hog_x)].r += (max_gradient * this->weight.get_item(cell_y) * this->weight.get_item(cell_x));
            hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y, hog_x)].r += (max_gradient * this->weight.get_item(cell_y) * this->weight.get_item(cell_x));
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y, hog_x)].r += (max_gradient * this->weight.get_item()[cell_y] * this->weight.get_item()[cell_x]);

            hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y + this->surround_index_offset.get_item(cell_y), hog_x)].r += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight.get_item(cell_x));
            hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y + this->surround_index_offset.get_item(cell_y), hog_x)].r += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight.get_item(cell_x));
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y + this->surround_index_offset.get_item()[cell_y], hog_x)].r += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight.get_item()[cell_x]);

            hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y, hog_x + this->surround_index_offset.get_item(cell_x))].r += (max_gradient * this->weight.get_item(cell_y) * this->weight_surround.get_item(cell_x));
            hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y, hog_x + this->surround_index_offset.get_item(cell_x))].r += (max_gradient * this->weight.get_item(cell_y) * this->weight_surround.get_item(cell_x));
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y, hog_x + this->surround_index_offset.get_item()[cell_x])].r += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);

            hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y + this->surround_index_offset.get_item(cell_y), hog_x + this->surround_index_offset.get_item()[cell_x])].r += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight_surround.get_item(cell_x));
            hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y + this->surround_index_offset.get_item(cell_y), hog_x + this->surround_index_offset.get_item()[cell_x])].r += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight_surround.get_item(cell_x));
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y + this->surround_index_offset.get_item()[cell_y], hog_x + this->surround_index_offset.get_item()[cell_x])].r += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);

#if CONFIG_HOG_DEBUG
            // TODO: Debug
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 0)] = max_gradient;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 1)] = max_orientation_index % this->sector_number;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 2)] = max_orientation_index;
#endif
        }
        image_up += next_row_stride;
        image_down += next_row_stride;
        image_left += next_row_stride;
        image_right += next_row_stride;
    }
}

template <typename I, typename O>
inline void HOG<I, O>::set_hog_1_chw_cell(ArrayImaginary<3> &hog,
                                          int hog_y, int hog_x,
                                          ArrayReal<I, O, 3> &image,
                                          int image_up_y, int image_up_x,
                                          int next_row_stride,
                                          int cell_y_start, int cell_y_end,
                                          int cell_x_start, int cell_x_end
#if CONFIG_HOG_DEBUG
                                          ,
                                          ArrayReal<I, O, 3> &debug
#endif
)
{
    I *image_up = image.get_item() + image.get_index_3d_y_x(image_up_y, image_up_x);
    I *image_down = image_up + this->image_up_down_offset;
    I *image_left = image_up + this->image_up_left_offset;
    I *image_right = image_up + this->image_up_right_offset;

    for (size_t cell_y = cell_y_start; cell_y < cell_y_end; cell_y++)
    {
        for (size_t cell_x = cell_x_start; cell_x < cell_x_end; cell_x++)
        {
            // TODO: Find the max gradient among each channel, keep its vertical and horizontal gradient
            O max_gradient_x = *image_right++ - *image_left++;
            O max_gradient_y = *image_down++ - *image_up++;
            O max_gradient = sqrt(max_gradient_x * max_gradient_x + max_gradient_y * max_gradient_y);
            for (size_t image_c = 1; image_c < image.get_shape(2); image_c++)
            {
                O gradient_x = *image_right++ - *image_left++;
                O gradient_y = *image_down++ - *image_up++;
                O gradient = sqrt(gradient_x * gradient_x + gradient_y * gradient_y);

                if (gradient > max_gradient)
                {
                    max_gradient = gradient;
                    max_gradient_x = gradient_x;
                    max_gradient_y = gradient_y;
                }
            }

            // TODO: Find the orientation
            O max_orientation_value = this->cos_angle.get_item(0) * max_gradient_x + this->sin_angle.get_item(0) * max_gradient_y;
            int max_orientation_index = 0;

            for (size_t sector_i = 0; sector_i < this->sector_number; sector_i++)
            {
                O orientation_value = this->cos_angle.get_item(sector_i) * max_gradient_x + this->sin_angle.get_item(sector_i) * max_gradient_y;

                if (orientation_value > max_orientation_value)
                {
                    max_orientation_value = orientation_value;
                    max_orientation_index = sector_i;
                }
                else if (-orientation_value > max_orientation_value)
                {
                    max_orientation_value = -orientation_value;
                    max_orientation_index = sector_i + this->sector_number;
                }
            }

            // TODO: Accumlate
            hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y, hog_x)].r += (max_gradient * this->weight.get_item()[cell_y] * this->weight.get_item()[cell_x]);
            hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y, hog_x)].r += (max_gradient * this->weight.get_item()[cell_y] * this->weight.get_item()[cell_x]);

            int nearest_hog_y = hog_y + this->surround_index_offset.get_item()[cell_y];
            int nearest_hog_x = hog_x + this->surround_index_offset.get_item()[cell_x];

            if (0 <= nearest_hog_y && nearest_hog_y < hog.get_shape(1))
            {
                hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, nearest_hog_y, hog_x)].r += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight.get_item()[cell_x]);
                hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, nearest_hog_y, hog_x)].r += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight.get_item()[cell_x]);

                if (0 <= nearest_hog_x && nearest_hog_x < hog.get_shape(2))
                {
                    hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y, nearest_hog_x)].r += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
                    hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y, nearest_hog_x)].r += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);

                    hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, nearest_hog_y, nearest_hog_x)].r += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
                    hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, nearest_hog_y, nearest_hog_x)].r += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
                }
            }
            else
            {
                if (0 <= nearest_hog_x && nearest_hog_x < hog.get_shape(2))
                {
                    hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y, nearest_hog_x)].r += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
                    hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y, nearest_hog_x)].r += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
                }
            }

#if CONFIG_HOG_DEBUG
            // TODO: Debug
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 0)] = max_gradient;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 1)] = max_orientation_index % this->sector_number;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 2)] = max_orientation_index;
#endif
        }
        image_up += next_row_stride;
        image_down += next_row_stride;
        image_left += next_row_stride;
        image_right += next_row_stride;
    }
}

template <typename I, typename O>
inline void HOG<I, O>::set_hog_1_chw_cell(ArrayReal<O, O, 3> &hog,
                                          int hog_y, int hog_x,
                                          ArrayReal<I, O, 3> &image,
                                          int image_up_y, int image_up_x,
                                          int next_row_stride
#if CONFIG_HOG_DEBUG
                                          ,
                                          ArrayReal<I, O, 3> &debug
#endif
)
{
    I *image_up = image.get_item() + image.get_index_3d_y_x(image_up_y, image_up_x);
    I *image_down = image_up + this->image_up_down_offset;
    I *image_left = image_up + this->image_up_left_offset;
    I *image_right = image_up + this->image_up_right_offset;

    for (size_t cell_y = 0; cell_y < this->cell_size; cell_y++)
    {
        for (size_t cell_x = 0; cell_x < this->cell_size; cell_x++)
        {
            // TODO: Find the max gradient among each channel, keep its vertical and horizontal gradient
            O max_gradient_x = *image_right++ - *image_left++;
            O max_gradient_y = *image_down++ - *image_up++;
            O max_gradient = sqrt(max_gradient_x * max_gradient_x + max_gradient_y * max_gradient_y);
            for (size_t image_c = 1; image_c < image.get_shape(2); image_c++)
            {
                O gradient_x = *image_right++ - *image_left++;
                O gradient_y = *image_down++ - *image_up++;
                O gradient = sqrt(gradient_x * gradient_x + gradient_y * gradient_y);

                if (gradient > max_gradient)
                {
                    max_gradient = gradient;
                    max_gradient_x = gradient_x;
                    max_gradient_y = gradient_y;
                }
            }

            // TODO: Find the orientation
            O max_orientation_value = this->cos_angle.get_item(0) * max_gradient_x + this->sin_angle.get_item(0) * max_gradient_y;
            int max_orientation_index = 0;

            for (size_t sector_i = 0; sector_i < this->sector_number; sector_i++)
            {
                O orientation_value = this->cos_angle.get_item(sector_i) * max_gradient_x + this->sin_angle.get_item(sector_i) * max_gradient_y;

                if (orientation_value > max_orientation_value)
                {
                    max_orientation_value = orientation_value;
                    max_orientation_index = sector_i;
                }
                else if (-orientation_value > max_orientation_value)
                {
                    max_orientation_value = -orientation_value;
                    max_orientation_index = sector_i + this->sector_number;
                }
            }

            // TODO: Accumlate
            hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y, hog_x)] += (max_gradient * this->weight.get_item()[cell_y] * this->weight.get_item()[cell_x]);
            hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y, hog_x)] += (max_gradient * this->weight.get_item()[cell_y] * this->weight.get_item()[cell_x]);
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y, hog_x)] += (max_gradient * this->weight.get_item()[cell_y] * this->weight.get_item()[cell_x]);

            hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y + this->surround_index_offset.get_item()[cell_y], hog_x)] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight.get_item()[cell_x]);
            hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y + this->surround_index_offset.get_item()[cell_y], hog_x)] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight.get_item()[cell_x]);
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y + this->surround_index_offset.get_item()[cell_y], hog_x)] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight.get_item()[cell_x]);

            hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y, hog_x + this->surround_index_offset.get_item()[cell_x])] += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
            hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y, hog_x + this->surround_index_offset.get_item()[cell_x])] += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y, hog_x + this->surround_index_offset.get_item()[cell_x])] += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);

            hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y + this->surround_index_offset.get_item()[cell_y], hog_x + this->surround_index_offset.get_item()[cell_x])] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
            hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y + this->surround_index_offset.get_item()[cell_y], hog_x + this->surround_index_offset.get_item()[cell_x])] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y + this->surround_index_offset.get_item()[cell_y], hog_x + this->surround_index_offset.get_item()[cell_x])] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);

#if CONFIG_HOG_DEBUG
            // TODO: Debug
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 0)] = max_gradient;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 1)] = max_orientation_index % this->sector_number;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 2)] = max_orientation_index;
#endif
        }
        image_up += next_row_stride;
        image_down += next_row_stride;
        image_left += next_row_stride;
        image_right += next_row_stride;
    }
}

template <typename I, typename O>
inline void HOG<I, O>::set_hog_1_chw_cell(ArrayReal<O, O, 3> &hog,
                                          int hog_y, int hog_x,
                                          ArrayReal<I, O, 3> &image,
                                          int image_up_y, int image_up_x,
                                          int next_row_stride,
                                          int cell_y_start, int cell_y_end,
                                          int cell_x_start, int cell_x_end
#if CONFIG_HOG_DEBUG
                                          ,
                                          ArrayReal<I, O, 3> &debug
#endif
)
{
    I *image_up = image.get_item() + image.get_index_3d_y_x(image_up_y, image_up_x);
    I *image_down = image_up + this->image_up_down_offset;
    I *image_left = image_up + this->image_up_left_offset;
    I *image_right = image_up + this->image_up_right_offset;

    for (size_t cell_y = cell_y_start; cell_y < cell_y_end; cell_y++)
    {
        for (size_t cell_x = cell_x_start; cell_x < cell_x_end; cell_x++)
        {
            // TODO: Find the max gradient among each channel, keep its vertical and horizontal gradient
            O max_gradient_x = *image_right++ - *image_left++;
            O max_gradient_y = *image_down++ - *image_up++;
            O max_gradient = sqrt(max_gradient_x * max_gradient_x + max_gradient_y * max_gradient_y);
            for (size_t image_c = 1; image_c < image.get_shape(2); image_c++)
            {
                O gradient_x = *image_right++ - *image_left++;
                O gradient_y = *image_down++ - *image_up++;
                O gradient = sqrt(gradient_x * gradient_x + gradient_y * gradient_y);

                if (gradient > max_gradient)
                {
                    max_gradient = gradient;
                    max_gradient_x = gradient_x;
                    max_gradient_y = gradient_y;
                }
            }

            // TODO: Find the orientation
            O max_orientation_value = this->cos_angle.get_item(0) * max_gradient_x + this->sin_angle.get_item(0) * max_gradient_y;
            int max_orientation_index = 0;

            for (size_t sector_i = 0; sector_i < this->sector_number; sector_i++)
            {
                O orientation_value = this->cos_angle.get_item(sector_i) * max_gradient_x + this->sin_angle.get_item(sector_i) * max_gradient_y;

                if (orientation_value > max_orientation_value)
                {
                    max_orientation_value = orientation_value;
                    max_orientation_index = sector_i;
                }
                else if (-orientation_value > max_orientation_value)
                {
                    max_orientation_value = -orientation_value;
                    max_orientation_index = sector_i + this->sector_number;
                }
            }

            // TODO: Accumlate
            hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y, hog_x)] += (max_gradient * this->weight.get_item()[cell_y] * this->weight.get_item()[cell_x]);
            hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y, hog_x)] += (max_gradient * this->weight.get_item()[cell_y] * this->weight.get_item()[cell_x]);

            int nearest_hog_y = hog_y + this->surround_index_offset.get_item()[cell_y];
            int nearest_hog_x = hog_x + this->surround_index_offset.get_item()[cell_x];

            if (0 <= nearest_hog_y && nearest_hog_y < hog.get_shape(1))
            {
                hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, nearest_hog_y, hog_x)] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight.get_item()[cell_x]);
                hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, nearest_hog_y, hog_x)] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight.get_item()[cell_x]);

                if (0 <= nearest_hog_x && nearest_hog_x < hog.get_shape(2))
                {
                    hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y, nearest_hog_x)] += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
                    hog.get_item()[hog.get_index_cell_y3d(max_orientation_index + this->sector_number, hog_y, nearest_hog_x)] += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);

                    hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, nearest_hog_y, nearest_hog_x)] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
                    hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, nearest_hog_y, nearest_hog_x)] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
                }
            }
            else
            {
                if (0 <= nearest_hog_x && nearest_hog_x < hog.get_shape(2))
                {
                    hog.get_item()[hog.get_index_3d(max_orientation_index % this->sector_number, hog_y, nearest_hog_x)] += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
                    hog.get_item()[hog.get_index_3d(max_orientation_index + this->sector_number, hog_y, nearest_hog_x)] += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);
                }
            }

#if CONFIG_HOG_DEBUG
            // TODO: Debug
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 0)] = max_gradient;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 1)] = max_orientation_index % this->sector_number;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 2)] = max_orientation_index;
#endif
        }
        image_up += next_row_stride;
        image_down += next_row_stride;
        image_left += next_row_stride;
        image_right += next_row_stride;
    }
}

template <typename I, typename O>
inline void HOG<I, O>::set_hog_1_hwc_cell(ArrayReal<O, O, 3> &hog,
                                          int hog_y, int hog_x,
                                          ArrayReal<I, O, 3> &image,
                                          int image_up_y, int image_up_x,
                                          int next_row_stride
#if CONFIG_HOG_DEBUG
                                          ,
                                          ArrayReal<I, O, 3> &debug
#endif
)
{
    I *image_up = image.get_item() + image.get_index_3d_y_x(image_up_y, image_up_x);
    I *image_down = image_up + this->image_up_down_offset;
    I *image_left = image_up + this->image_up_left_offset;
    I *image_right = image_up + this->image_up_right_offset;

    for (size_t cell_y = 0; cell_y < this->cell_size; cell_y++)
    {
        for (size_t cell_x = 0; cell_x < this->cell_size; cell_x++)
        {
            // TODO: Find the max gradient among each channel, keep its vertical and horizontal gradient
            O max_gradient_x = *image_right++ - *image_left++;
            O max_gradient_y = *image_down++ - *image_up++;
            O max_gradient = sqrt(max_gradient_x * max_gradient_x + max_gradient_y * max_gradient_y);
            for (size_t image_c = 1; image_c < image.get_shape(2); image_c++)
            {
                O gradient_x = *image_right++ - *image_left++;
                O gradient_y = *image_down++ - *image_up++;
                O gradient = sqrt(gradient_x * gradient_x + gradient_y * gradient_y);

                if (gradient > max_gradient)
                {
                    max_gradient = gradient;
                    max_gradient_x = gradient_x;
                    max_gradient_y = gradient_y;
                }
            }

            // TODO: Find the orientation
            O max_orientation_value = this->cos_angle.get_item(0) * max_gradient_x + this->sin_angle.get_item(0) * max_gradient_y;
            int max_orientation_index = 0;

            for (size_t sector_i = 0; sector_i < this->sector_number; sector_i++)
            {
                O orientation_value = this->cos_angle.get_item(sector_i) * max_gradient_x + this->sin_angle.get_item(sector_i) * max_gradient_y;

                if (orientation_value > max_orientation_value)
                {
                    max_orientation_value = orientation_value;
                    max_orientation_index = sector_i;
                }
                else if (-orientation_value > max_orientation_value)
                {
                    max_orientation_value = -orientation_value;
                    max_orientation_index = sector_i + this->sector_number;
                }
            }

            // TODO: Accumlate
            hog.get_item()[hog.get_index_3d(hog_y, hog_x, max_orientation_index % this->sector_number)] += (max_gradient * this->weight.get_item(cell_y) * this->weight.get_item(cell_x));
            hog.get_item()[hog.get_index_3d(hog_y, hog_x, max_orientation_index + this->sector_number)] += (max_gradient * this->weight.get_item(cell_y) * this->weight.get_item(cell_x));
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y, hog_x)] += (max_gradient * this->weight.get_item()[cell_y] * this->weight.get_item()[cell_x]);

            hog.get_item()[hog.get_index_3d(hog_y + this->surround_index_offset.get_item(cell_y), hog_x, max_orientation_index % this->sector_number)] += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight.get_item(cell_x));
            hog.get_item()[hog.get_index_3d(hog_y + this->surround_index_offset.get_item(cell_y), hog_x, max_orientation_index + this->sector_number)] += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight.get_item(cell_x));
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y + this->surround_index_offset.get_item()[cell_y], hog_x)] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight.get_item()[cell_x]);

            hog.get_item()[hog.get_index_3d(hog_y, hog_x + this->surround_index_offset.get_item(cell_x), max_orientation_index % this->sector_number)] += (max_gradient * this->weight.get_item(cell_y) * this->weight_surround.get_item(cell_x));
            hog.get_item()[hog.get_index_3d(hog_y, hog_x + this->surround_index_offset.get_item(cell_x), max_orientation_index + this->sector_number)] += (max_gradient * this->weight.get_item(cell_y) * this->weight_surround.get_item(cell_x));
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y, hog_x + this->surround_index_offset.get_item()[cell_x])] += (max_gradient * this->weight.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);

            hog.get_item()[hog.get_index_3d(hog_y + this->surround_index_offset.get_item(cell_y), hog_x + this->surround_index_offset.get_item(cell_x), max_orientation_index % this->sector_number)] += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight_surround.get_item(cell_x));
            hog.get_item()[hog.get_index_3d(hog_y + this->surround_index_offset.get_item(cell_y), hog_x + this->surround_index_offset.get_item(cell_x), max_orientation_index + this->sector_number)] += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight_surround.get_item(cell_x));
            // hog.get_item()[hog.get_index_3d(max_orientation_index, hog_y + this->surround_index_offset.get_item()[cell_y], hog_x + this->surround_index_offset.get_item()[cell_x])] += (max_gradient * this->weight_surround.get_item()[cell_y] * this->weight_surround.get_item()[cell_x]);

#if CONFIG_HOG_DEBUG
            // TODO: Debug
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 0)] = max_gradient;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 1)] = max_orientation_index % this->sector_number;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 2)] = max_orientation_index;
#endif
        }
        image_up += next_row_stride;
        image_down += next_row_stride;
        image_left += next_row_stride;
        image_right += next_row_stride;
    }
}

template <typename I, typename O>
inline void HOG<I, O>::set_hog_1_hwc_cell(ArrayReal<O, O, 3> &hog,
                                          int hog_y, int hog_x,
                                          ArrayReal<I, O, 3> &image,
                                          int image_up_y, int image_up_x,
                                          int next_row_stride,
                                          int cell_y_start, int cell_y_end,
                                          int cell_x_start, int cell_x_end
#if CONFIG_HOG_DEBUG
                                          ,
                                          ArrayReal<I, O, 3> &debug
#endif
)
{
    I *image_up = image.get_item() + image.get_index_3d_y_x(image_up_y, image_up_x);
    I *image_down = image_up + this->image_up_down_offset;
    I *image_left = image_up + this->image_up_left_offset;
    I *image_right = image_up + this->image_up_right_offset;

    for (size_t cell_y = cell_y_start; cell_y < cell_y_end; cell_y++)
    {
        for (size_t cell_x = cell_x_start; cell_x < cell_x_end; cell_x++)
        {
            // TODO: Find the max gradient among each channel, keep its vertical and horizontal gradient
            O max_gradient_x = *image_right++ - *image_left++;
            O max_gradient_y = *image_down++ - *image_up++;
            O max_gradient = sqrt(max_gradient_x * max_gradient_x + max_gradient_y * max_gradient_y);
            for (size_t image_c = 1; image_c < image.get_shape(2); image_c++)
            {
                O gradient_x = *image_right++ - *image_left++;
                O gradient_y = *image_down++ - *image_up++;
                O gradient = sqrt(gradient_x * gradient_x + gradient_y * gradient_y);

                if (gradient > max_gradient)
                {
                    max_gradient = gradient;
                    max_gradient_x = gradient_x;
                    max_gradient_y = gradient_y;
                }
            }

            // TODO: Find the orientation
            O max_orientation_value = this->cos_angle.get_item(0) * max_gradient_x + this->sin_angle.get_item(0) * max_gradient_y;
            int max_orientation_index = 0;

            for (size_t sector_i = 0; sector_i < this->sector_number; sector_i++)
            {
                O orientation_value = this->cos_angle.get_item(sector_i) * max_gradient_x + this->sin_angle.get_item(sector_i) * max_gradient_y;

                if (orientation_value > max_orientation_value)
                {
                    max_orientation_value = orientation_value;
                    max_orientation_index = sector_i;
                }
                else if (-orientation_value > max_orientation_value)
                {
                    max_orientation_value = -orientation_value;
                    max_orientation_index = sector_i + this->sector_number;
                }
            }

            // TODO: Accumlate
            hog.get_item()[hog.get_index_3d(hog_y, hog_x, max_orientation_index % this->sector_number)] += (max_gradient * this->weight.get_item(cell_y) * this->weight.get_item(cell_x));
            hog.get_item()[hog.get_index_3d(hog_y, hog_x, max_orientation_index + this->sector_number)] += (max_gradient * this->weight.get_item(cell_y) * this->weight.get_item(cell_x));

            int nearest_hog_y = hog_y + this->surround_index_offset.get_item(cell_y);
            int nearest_hog_x = hog_x + this->surround_index_offset.get_item(cell_x);

            if (0 <= nearest_hog_y && nearest_hog_y < hog.get_shape(0))
            {
                hog.get_item()[hog.get_index_3d(nearest_hog_y, hog_x, max_orientation_index % this->sector_number)] += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight.get_item(cell_x));
                hog.get_item()[hog.get_index_3d(nearest_hog_y, hog_x, max_orientation_index + this->sector_number)] += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight.get_item(cell_x));

                if (0 <= nearest_hog_x && nearest_hog_x < hog.get_shape(1))
                {
                    hog.get_item()[hog.get_index_3d(hog_y, nearest_hog_x, max_orientation_index % this->sector_number)] += (max_gradient * this->weight.get_item(cell_y) * this->weight_surround.get_item(cell_x));
                    hog.get_item()[hog.get_index_3d(hog_y, nearest_hog_x, max_orientation_index + this->sector_number)] += (max_gradient * this->weight.get_item(cell_y) * this->weight_surround.get_item(cell_x));

                    hog.get_item()[hog.get_index_3d(nearest_hog_y, nearest_hog_x, max_orientation_index % this->sector_number)] += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight_surround.get_item(cell_x));
                    hog.get_item()[hog.get_index_3d(nearest_hog_y, nearest_hog_x, max_orientation_index + this->sector_number)] += (max_gradient * this->weight_surround.get_item(cell_y) * this->weight_surround.get_item(cell_x));
                }
            }
            else
            {
                if (0 <= nearest_hog_x && nearest_hog_x < hog.get_shape(1))
                {
                    hog.get_item()[hog.get_index_3d(hog_y, nearest_hog_x, max_orientation_index % this->sector_number)] += (max_gradient * this->weight.get_item(cell_y) * this->weight_surround.get_item(cell_x));
                    hog.get_item()[hog.get_index_3d(hog_y, nearest_hog_x, max_orientation_index + this->sector_number)] += (max_gradient * this->weight.get_item(cell_y) * this->weight_surround.get_item(cell_x));
                }
            }

#if CONFIG_HOG_DEBUG
            // TODO: Debug
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 0)] = max_gradient;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 1)] = max_orientation_index % this->sector_number;
            debug.get_item()[debug.get_index_3d(hog_y * this->cell_size + cell_y, hog_x * this->cell_size + cell_x, 2)] = max_orientation_index;
#endif
        }
        image_up += next_row_stride;
        image_down += next_row_stride;
        image_left += next_row_stride;
        image_right += next_row_stride;
    }
}

template <typename I, typename O>
void HOG<I, O>::set_hog_1_chw(ArrayReal<I, O, 3> &image, ArrayImaginary<3> &hog)
{
    // TODO: init
    memset(hog.get_item(), 0, hog.get_length() * sizeof(cpx));

    int cell_size_minus_one = this->cell_size - 1;

    int next_row_stride_0 = (image.get_shape(1) - cell_size_minus_one) * image.get_shape(2);
    int next_row_stride_1 = (image.get_shape(1) - this->cell_size) * image.get_shape(2);

    this->image_up_down_offset = image.get_index_3d_y(2);
    this->image_up_left_offset = image.get_index_3d_x(image.get_shape(1) - 1);
    this->image_up_right_offset = image_up_left_offset + image.get_shape(2) + image.get_shape(2);

#if CONFIG_HOG_DEBUG
    ArrayReal<I, O, 3> debug(image.get_shape(), DO_CALLOC);
#endif

    // TODO: hog_y = 0, hog_x = 0
    int hog_y = 0;
    int hog_x = 0;
    set_hog_1_chw_cell(hog, hog_y, hog_x, image, 0, 1, next_row_stride_0, 1, this->cell_size, 1, this->cell_size
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

    // TODO: hog_y = 0, hog_x = [1, hog.get_shape(2) - 2]
    for (hog_x = 1; hog_x < hog.get_shape(2) - 1; hog_x++)
        set_hog_1_chw_cell(hog, hog_y, hog_x, image, 0, hog_x * this->cell_size, next_row_stride_1, 1, this->cell_size, 0, this->cell_size
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );

    // TODO: hog_y = 0, hog_x = hog.get_shape(2) - 1
    set_hog_1_chw_cell(hog, hog_y, hog_x, image, 0, hog_x * this->cell_size, next_row_stride_0, 1, this->cell_size, 0, cell_size_minus_one
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

    // TODO: hog_y = [1, hog.get_shape(1) - 2]
    for (hog_y = 1; hog_y < hog.get_shape(1) - 1; hog_y++)
    {
        // TODO: hog_y = [1, hog.get_shape(1) - 2], hog_x = 0;
        hog_x = 0;
        set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, 1, next_row_stride_0, 0, this->cell_size, 1, this->cell_size
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );

        // TODO: hog_y = [1, hog.get_shape(1) - 2], hog_x = [1, hog.get_shape(2) - 2]
        for (hog_x = 1; hog_x < hog.get_shape(2) - 1; hog_x++)
            set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_1
#if CONFIG_HOG_DEBUG
                               ,
                               debug
#endif
            );

        // TODO: hog_y = [1, hog.get_shape(1) - 2], hog_x = hog.get_shape(2) - 1;
        set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_0, 0, this->cell_size, 0, cell_size_minus_one
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );
    }

    // TODO: hog_y = hog.get_shape(1) - 1, hog_x = 0
    hog_x = 0;
    set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, 1, next_row_stride_0, 0, cell_size_minus_one, 1, this->cell_size
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

    // TODO: hog_y = hog.get_shape(1) - 1, x = [1, hog.get_shape(2) - 2]
    for (hog_x = 1; hog_x < hog.get_shape(2) - 1; hog_x++)
        set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_1, 0, cell_size_minus_one, 0, this->cell_size
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );

    // TODO: hog_y = hog.get_shape(1) - 1, x = hog.get_shape(2) - 1
    set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_0, 0, cell_size_minus_one, 0, cell_size_minus_one
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

#if CONFIG_HOG_DEBUG
    for (size_t i = 0; i < 0; i++)
    {
        debug.print_3d(0, 64, 0, 14, i, i + 1, "\ndebug", 0);
    }

    for (size_t c = 0; c < hog.get_shape(0); c++)
        hog.print_3d(c, c + 1, 0, 10, 0, 7, "\nHOG Feature", 1);

#endif
}

template <typename I, typename O>
void HOG<I, O>::set_hog_1_chw(ArrayReal<I, O, 3> &image, ArrayReal<O, O, 3> &hog)
{
    // TODO: init
    memset(hog.get_item(), 0, hog.get_length() * sizeof(cpx));

    int cell_size_minus_one = this->cell_size - 1;

    int next_row_stride_0 = (image.get_shape(1) - cell_size_minus_one) * image.get_shape(2);
    int next_row_stride_1 = (image.get_shape(1) - this->cell_size) * image.get_shape(2);

    this->image_up_down_offset = image.get_index_3d_y(2);
    this->image_up_left_offset = image.get_index_3d_x(image.get_shape(1) - 1);
    this->image_up_right_offset = image_up_left_offset + image.get_shape(2) + image.get_shape(2);

#if CONFIG_HOG_DEBUG
    ArrayReal<I, O, 3> debug(image.get_shape(), DO_CALLOC);
#endif

    // TODO: hog_y = 0, hog_x = 0
    int hog_y = 0;
    int hog_x = 0;
    set_hog_1_chw_cell(hog, hog_y, hog_x, image, 0, 1, next_row_stride_0, 1, this->cell_size, 1, this->cell_size
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

    // TODO: hog_y = 0, hog_x = [1, hog.get_shape(2) - 2]
    for (hog_x = 1; hog_x < hog.get_shape(2) - 1; hog_x++)
        set_hog_1_chw_cell(hog, hog_y, hog_x, image, 0, hog_x * this->cell_size, next_row_stride_1, 1, this->cell_size, 0, this->cell_size
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );

    // TODO: hog_y = 0, hog_x = hog.get_shape(2) - 1
    set_hog_1_chw_cell(hog, hog_y, hog_x, image, 0, hog_x * this->cell_size, next_row_stride_0, 1, this->cell_size, 0, cell_size_minus_one
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

    // TODO: hog_y = [1, hog.get_shape(1) - 2]
    for (hog_y = 1; hog_y < hog.get_shape(1) - 1; hog_y++)
    {
        // TODO: hog_y = [1, hog.get_shape(1) - 2], hog_x = 0;
        hog_x = 0;
        set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, 1, next_row_stride_0, 0, this->cell_size, 1, this->cell_size
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );

        // TODO: hog_y = [1, hog.get_shape(1) - 2], hog_x = [1, hog.get_shape(2) - 2]
        for (hog_x = 1; hog_x < hog.get_shape(2) - 1; hog_x++)
            set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_1
#if CONFIG_HOG_DEBUG
                               ,
                               debug
#endif
            );

        // TODO: hog_y = [1, hog.get_shape(1) - 2], hog_x = hog.get_shape(2) - 1;
        set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_0, 0, this->cell_size, 0, cell_size_minus_one
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );
    }

    // TODO: hog_y = hog.get_shape(1) - 1, hog_x = 0
    hog_x = 0;
    set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, 1, next_row_stride_0, 0, cell_size_minus_one, 1, this->cell_size
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

    // TODO: hog_y = hog.get_shape(1) - 1, x = [1, hog.get_shape(2) - 2]
    for (hog_x = 1; hog_x < hog.get_shape(2) - 1; hog_x++)
        set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_1, 0, cell_size_minus_one, 0, this->cell_size
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );

    // TODO: hog_y = hog.get_shape(1) - 1, x = hog.get_shape(2) - 1
    set_hog_1_chw_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_0, 0, cell_size_minus_one, 0, cell_size_minus_one
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

#if CONFIG_HOG_DEBUG
    for (size_t i = 0; i < 0; i++)
    {
        debug.print_3d(0, 64, 0, 14, i, i + 1, "\ndebug", 0);
    }

    for (size_t c = 0; c < hog.get_shape(0); c++)
        hog.print_3d(c, c + 1, 0, 10, 0, 7, "\nHOG Feature", 1);

#endif
}

template <typename I, typename O>
void HOG<I, O>::set_hog_1_hwc(ArrayReal<I, O, 3> &image, ArrayReal<O, O, 3> &hog)
{
    // TODO: init
    memset(hog.get_item(), 0, hog.get_length() * sizeof(O));

    int cell_size_minus_one = this->cell_size - 1;

    int next_row_stride_0 = (image.get_shape(1) - cell_size_minus_one) * image.get_shape(2);
    int next_row_stride_1 = (image.get_shape(1) - this->cell_size) * image.get_shape(2);

    this->image_up_down_offset = image.get_index_3d_y(2);
    this->image_up_left_offset = image.get_index_3d_x(image.get_shape(1) - 1);
    this->image_up_right_offset = image_up_left_offset + image.get_shape(2) + image.get_shape(2);

#if CONFIG_HOG_DEBUG
    ArrayReal<I, O, 3> debug(image.get_shape(), DO_CALLOC);
#endif

    // TODO: hog_y = 0, hog_x = 0
    int hog_y = 0;
    int hog_x = 0;
    set_hog_1_hwc_cell(hog, hog_y, hog_x, image, 0, 1, next_row_stride_0, 1, this->cell_size, 1, this->cell_size
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

    // TODO: hog_y = 0, hog_x = [1, hog.get_shape(1) - 2]
    for (hog_x = 1; hog_x < hog.get_shape(1) - 1; hog_x++)
        set_hog_1_hwc_cell(hog, hog_y, hog_x, image, 0, hog_x * this->cell_size, next_row_stride_1, 1, this->cell_size, 0, this->cell_size
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );

    // TODO: hog_y = 0, hog_x = hog.get_shape(1) - 1
    set_hog_1_hwc_cell(hog, hog_y, hog_x, image, 0, hog_x * this->cell_size, next_row_stride_0, 1, this->cell_size, 0, cell_size_minus_one
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

    // TODO: hog_y = [1, hog.get_shape(0) - 2]
    for (hog_y = 1; hog_y < hog.get_shape(0) - 1; hog_y++)
    {
        // TODO: hog_y = [1, hog.get_shape(0) - 2], hog_x = 0;
        hog_x = 0;
        set_hog_1_hwc_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, 1, next_row_stride_0, 0, this->cell_size, 1, this->cell_size
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );

        // TODO: hog_y = [1, hog.get_shape(0) - 2], hog_x = [1, hog.get_shape(1) - 2]
        for (hog_x = 1; hog_x < hog.get_shape(1) - 1; hog_x++)
            set_hog_1_hwc_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_1
#if CONFIG_HOG_DEBUG
                               ,
                               debug
#endif
            );

        // TODO: hog_y = [1, hog.get_shape(0) - 2], hog_x = hog.get_shape(1) - 1;
        set_hog_1_hwc_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_0, 0, this->cell_size, 0, cell_size_minus_one
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );
    }

    // TODO: hog_y = hog.get_shape(0) - 1, hog_x = 0
    hog_x = 0;
    set_hog_1_hwc_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, 1, next_row_stride_0, 0, cell_size_minus_one, 1, this->cell_size
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

    // TODO: hog_y = hog.get_shape(0) - 1, x = [1, hog.get_shape(1) - 2]
    for (hog_x = 1; hog_x < hog.get_shape(1) - 1; hog_x++)
        set_hog_1_hwc_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_1, 0, cell_size_minus_one, 0, this->cell_size
#if CONFIG_HOG_DEBUG
                           ,
                           debug
#endif
        );

    // TODO: hog_y = hog.get_shape(0) - 1, x = hog.get_shape(1) - 1
    set_hog_1_hwc_cell(hog, hog_y, hog_x, image, hog_y * this->cell_size - 1, hog_x * this->cell_size, next_row_stride_0, 0, cell_size_minus_one, 0, cell_size_minus_one
#if CONFIG_HOG_DEBUG
                       ,
                       debug
#endif
    );

#if CONFIG_HOG_DEBUG
    for (size_t i = 0; i < 0; i++)
    {
        debug.print_3d(0, 64, 0, 14, i, i + 1, "\ndebug", 0);
    }

    for (size_t c = 0; c < hog.get_shape(0); c++)
        hog.print_3d(c, c + 1, 0, 10, 0, 7, "\nHOG Feature", 1);

#endif
}

template <typename I, typename O>
void HOG<I, O>::normalize_and_truncate(ArrayImaginary<3> &n_t_hog, ArrayReal<O, O, 3> &raw_hog, O threshold)
{
    ArrayReal<O, O, 2> divisor(raw_hog.get_shape(), DO_MALLOC);
    O *p_raw_hog = raw_hog.get_item();

    for (size_t i = 0; i < raw_hog.get_shape(0) * raw_hog.get_shape(1); i++)
    {
        // TODO: c = 0
        divisor.get_item()[i] = p_raw_hog[0] * p_raw_hog[0];
        // TODO: c = [1, this->sector_number]
        for (size_t raw_hog_c = 1; raw_hog_c < this->sector_number; raw_hog_c++)
            divisor.get_item()[i] += (p_raw_hog[raw_hog_c] * p_raw_hog[raw_hog_c]);

        // TODO: point to next pixel
        p_raw_hog += raw_hog.get_shape(2);
    }

    for (size_t n_t_hog_y = 0; n_t_hog_y < n_t_hog.get_shape(1); n_t_hog_y++)
    {
        for (size_t n_t_hog_x = 0; n_t_hog_x < n_t_hog.get_shape(2); n_t_hog_x++)
        {
            O left_up_divisor = sqrt(divisor.get_item(divisor.get_index_2d(n_t_hog_y, n_t_hog_x)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y, n_t_hog_x + 1)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y + 1, n_t_hog_x)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y + 1, n_t_hog_x + 1))) + 1e-7;
            O right_up_divisor = sqrt(divisor.get_item(divisor.get_index_2d(n_t_hog_y, n_t_hog_x + 1)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y, n_t_hog_x + 2)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y + 1, n_t_hog_x + 1)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y + 1, n_t_hog_x + 2))) + 1e-7;
            O left_down_divisor = sqrt(divisor.get_item(divisor.get_index_2d(n_t_hog_y + 1, n_t_hog_x)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y + 1, n_t_hog_x + 1)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y + 2, n_t_hog_x)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y + 2, n_t_hog_x + 1))) + 1e-7;
            O right_down_divisor = sqrt(divisor.get_item(divisor.get_index_2d(n_t_hog_y + 1, n_t_hog_x + 1)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y + 1, n_t_hog_x + 2)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y + 2, n_t_hog_x + 1)) + divisor.get_item(divisor.get_index_2d(n_t_hog_y + 2, n_t_hog_x + 2))) + 1e-7;

            O *part_1 = raw_hog.get_item() + raw_hog.get_index_3d_y_x(n_t_hog_y + 1, n_t_hog_x + 1); // abs
            O *part_2 = part_1 + this->sector_number;                                                // 0~180
            O *part_3 = part_2 + this->sector_number;                                                // 180~360
            for (size_t i = 0; i < this->sector_number; i++)
            {
                // TODO: part 1
                n_t_hog.get_item()[n_t_hog.get_index_3d(i, n_t_hog_y, n_t_hog_x)].r = min(part_1[i] / left_up_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i, n_t_hog_y, n_t_hog_x)].i = 0;
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number, n_t_hog_y, n_t_hog_x)].r = min(part_1[i] / right_up_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number, n_t_hog_y, n_t_hog_x)].i = 0;
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 2, n_t_hog_y, n_t_hog_x)].r = min(part_1[i] / left_down_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 2, n_t_hog_y, n_t_hog_x)].i = 0;
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 3, n_t_hog_y, n_t_hog_x)].r = min(part_1[i] / right_down_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 3, n_t_hog_y, n_t_hog_x)].i = 0;

                // TODO: part 2
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 4, n_t_hog_y, n_t_hog_x)].r = min(part_2[i] / left_up_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 4, n_t_hog_y, n_t_hog_x)].i = 0;
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 5, n_t_hog_y, n_t_hog_x)].r = min(part_2[i] / right_up_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 5, n_t_hog_y, n_t_hog_x)].i = 0;
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 6, n_t_hog_y, n_t_hog_x)].r = min(part_2[i] / left_down_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 6, n_t_hog_y, n_t_hog_x)].i = 0;
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 7, n_t_hog_y, n_t_hog_x)].r = min(part_2[i] / right_down_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 7, n_t_hog_y, n_t_hog_x)].i = 0;

                // TODO: part 3
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 8, n_t_hog_y, n_t_hog_x)].r = min(part_3[i] / left_up_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 8, n_t_hog_y, n_t_hog_x)].i = 0;
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 9, n_t_hog_y, n_t_hog_x)].r = min(part_3[i] / right_up_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 9, n_t_hog_y, n_t_hog_x)].i = 0;
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 10, n_t_hog_y, n_t_hog_x)].r = min(part_3[i] / left_down_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 10, n_t_hog_y, n_t_hog_x)].i = 0;
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 11, n_t_hog_y, n_t_hog_x)].r = min(part_3[i] / right_down_divisor, threshold);
                n_t_hog.get_item()[n_t_hog.get_index_3d(i + this->sector_number * 11, n_t_hog_y, n_t_hog_x)].i = 0;
            }
        }
    }
}

template <typename I, typename O>
void HOG<I, O>::set_hog_1_chw_with_normalize_truncate(ArrayImaginary<3> &hog, ArrayReal<I, O, 3> &image, O threshold)
{
    int raw_hog_shape[] = {image.get_shape(0) / this->cell_size, image.get_shape(1) / this->cell_size, this->sector_number * 3};
    ArrayReal<O, O, 3> raw_hog(raw_hog_shape, DO_MALLOC);

    set_hog_1_hwc(image, raw_hog);
    normalize_and_truncate(hog, raw_hog, threshold);
}
