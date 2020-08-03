#pragma once

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#include "array.hpp"

template <int N>
class ArrayImaginary : public Array<cpx, cpx, N>
{
protected:
private:
public:
    /////////////////////////////
    ////// Dimension = any //////
    /////////////////////////////

    ArrayImaginary();
    ArrayImaginary(int *shape, alloc_mode_t mode);
    ArrayImaginary(int *shape, cpx *item, bool do_copy, bool do_free);
    ~ArrayImaginary();

    void multiply(cpx *array, bool conj);
    void multiply(cpx *array, bool conj, cpx *result);

    ////// Override Operator //////
    void operator*=(ArrayImaginary<N> &array)
    {
        // assert(array.get_length() == this->length());

        // (a + bj) * (c + dj) = (ac - bd) + (ad + bc) j
        for (size_t i = 0; i < this->get_length(); i++)
        {
            scalar a = this->get_item()[i].r;
            scalar b = this->get_item()[i].i;
            scalar c = array.get_item()[i].r;
            scalar d = array.get_item()[i].i;

            this->get_item()[i].r = a * c - b * d;
            this->get_item()[i].i = a * d + b * c;
        }
    }

    void operator+=(ArrayImaginary<N> &array)
    {
        // assert(array.get_length() == this->length);

        for (size_t i = 0; i < this->get_length(); i++)
        {
            this->get_item()[i].r += (array.get_item()[i].r);
            this->get_item()[i].i += (array.get_item()[i].i);
        }
    }

    ///////////////////////////
    ////// Dimension = 2 //////
    ///////////////////////////

    void print_2d(int y_start, int y_end, int x_start, int x_end, char *name);

    ///////////////////////////
    ////// Dimension = 3 //////
    ///////////////////////////

    void print_3d(int y_start, int y_end, int x_start, int x_end, int c_start, int c_end, char *name, int index_axis);
};

template <int N>
ArrayImaginary<N>::ArrayImaginary()
{
}

template <int N>
ArrayImaginary<N>::ArrayImaginary(int *shape, alloc_mode_t mode)
{
    this->init(shape, mode);
}

template <int N>
ArrayImaginary<N>::ArrayImaginary(int *shape, cpx *item, bool do_copy, bool do_free)
{
    this->init(shape, item, do_copy, do_free);
}

template <int N>
ArrayImaginary<N>::~ArrayImaginary()
{
}

template <int N>
void ArrayImaginary<N>::multiply(cpx *array, bool conj)
{
    if (conj)
    {
        for (size_t i = 0; i < this->get_length(); i++)
        {
            scalar a = this->get_item()[i].r;
            scalar b = this->get_item()[i].i;
            scalar c = array[i].r;
            scalar d = array[i].i;

            this->get_item()[i].r = a * c + b * d;
            this->get_item()[i].i = b * c - a * d;
        }
    }
    else
    {
        for (size_t i = 0; i < this->get_length(); i++)
        {
            scalar a = this->get_item()[i].r;
            scalar b = this->get_item()[i].i;
            scalar c = array[i].r;
            scalar d = array[i].i;

            this->get_item()[i].r = a * c - b * d;
            this->get_item()[i].i = a * d + b * c;
        }
    }
}

template <int N>
void ArrayImaginary<N>::multiply(cpx *array, bool conj, cpx *result)
{
    if (conj)
    {
        for (size_t i = 0; i < this->get_length(); i++)
        {
            // scalar a = this->get_item()[i].r;
            // scalar b = this->get_item()[i].i;
            // scalar c = array[i].r;
            // scalar d = array[i].i;

            result[i].r = this->get_item()[i].r * array[i].r + this->get_item()[i].i * array[i].i;
            result[i].i = this->get_item()[i].i * array[i].r - this->get_item()[i].r * array[i].i;
        }
    }
    else
    {
        for (size_t i = 0; i < this->get_length(); i++)
        {
            // scalar a = this->get_item()[i].r;
            // scalar b = this->get_item()[i].i;
            // scalar c = array[i].r;
            // scalar d = array[i].i;

            result[i].r = this->get_item()[i].r * array[i].r - this->get_item()[i].i * array[i].i;
            result[i].i = this->get_item()[i].r * array[i].i + this->get_item()[i].i * array[i].r;
        }
    }
}

void print_cpx_point(cpx point);
template <int N>
void ArrayImaginary<N>::print_3d(int y_start, int y_end, int x_start, int x_end, int c_start, int c_end, char *name, int index_axis)
{
    Array<cpx, cpx, N>::print_3d(y_start, y_end, x_start, x_end, c_start, c_end, print_cpx_point, name, index_axis);
}

template <int N>
void ArrayImaginary<N>::print_2d(int y_start, int y_end, int x_start, int x_end, char *name)
{
    Array<cpx, cpx, N>::print_2d(y_start, y_end, x_start, x_end, print_cpx_point, name);
}
