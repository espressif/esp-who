#pragma once

#include "dl_define.hpp"
#include "dl_tool.hpp"
#include <cmath>

namespace dl {
namespace math {
/**
 * @brief x^a.
 *
 * @param x as a base
 * @param a as an exponent
 * @return x^a
 */
inline float power(float x, int a)
{
    if (a > 0) {
        return x * power(x, a - 1);
    } else if (a < 0) {
        return 1 / (x * power(x, -a - 1));
    } else {
        return 1.f;
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
/**
 * @brief sqrt(x).
 *
 * @param x as a base
 * @return sqrt(x)
 */
inline float sqrt_quick(float x)
{
    const int result = 0x1fbb4000 + (*(int *)&x >> 1);
    return *(float *)&result;
}

/**
 * @brief 1/sqrt(x).
 *
 * @param x as a base
 * @return 1/sqrt(x)
 */
inline float sqrt_reciprocal_quick(float x)
{
    float xhalf = 0.5f * x;
    int i = *(int *)&x;             // get bits for floating value
    i = 0x5f375a86 - (i >> 1);      // gives initial guess y0
    x = *(float *)&i;               // convert bits back to float
    x = x * (1.5f - xhalf * x * x); // Newton step, repeating increases accuracy
    return x;
}
#pragma GCC diagnostic pop

static const float EN = 0.00001f;

/**
 * @brief sqrt(x).
 *
 * @param x as a base
 * @return sqrt(x)
 */
inline float sqrt_newton(float x)
{
    /**
     * Use Newton iteration method to find the square root
     * */
    if (x == 0.f)
        return 0.f;
    float result = x;
    float last_value;
    do {
        last_value = result;
        result = (last_value + x / last_value) * 0.5;
    } while (DL_ABS(result - last_value) > EN);
    return result;
}

/**
 * @brief n-th root of x.
 *
 * @param x as a base
 * @param n root times
 * @return n-th root of x
 */
inline float root_newton(float x, int n)
{
    if (n == 2)
        return sqrt_newton(x);
    if (n == 0)
        return 1.f;
    if (n == 1)
        return x;
    if (x == 0.f)
        return 0.f;
    float result = x;
    float last_value;
    float _n = (float)((n - 1) * n);
    do {
        last_value = result;
        result = _n * last_value + x / (n * power(last_value, n - 1));
    } while (DL_ABS(result - last_value) > EN);
    return result;
}

/**
 * @brief atan(x).
 *
 * @param x as an input
 * @return atan(x) in range [-pi/2, pi/2]
 */
inline float atan(float x)
{
    return x * (0.78539816 - (DL_ABS(x) - 1) * (0.2447 + 0.0663 * DL_ABS(x)));
    // float s = x*x;
    // return ((-0.0464964749 * s + 0.15931422) * s - 0.327622764) * s * x + x;
}

// TODO:@yuanjiong
/**
 * @brief
 *
 * @param x
 * @param y
 * @return in range [-pi, pi]
 */
inline float atan2(float x, float y)
{
    float ax = DL_ABS(x);
    float ay = DL_ABS(y);
    float eps = 1e-8;
    float a = DL_MIN(ax, ay) / (DL_MAX(ax, ay) + eps);
    float r = atan(a); //[0, pi/2]
    if (ay > ax)
        r = 1.57079633 - r;
    if (x < 0)
        r = 3.14159265 - r;
    if (y < 0)
        r = -r;

    return r;
}

/**
 * @brief acos(x).
 *
 * @param x as an input
 * @return acos(x) in range [-pi/2, pi/2]
 */
inline float acos(float x)
{
    return atan2(x, sqrt_newton(1.0 - x * x));
}

/**
 * @brief asin(x).
 *
 * @param x as an input
 * @return asin(x) in range [0, pi]
 */
inline float asin(float x)
{
    return atan2(sqrt_newton(1.0 - x * x), x);
}

/**
 * @brief e^x
 *
 * @param x     exponent
 * @param steps iteration steps
 * @return e^x
 */
inline float exp_fast(float x, int steps = 8)
{
    x = 1.0 + x / (1 << steps);
    for (int i = 0; i < steps; i++) x *= x;
    return x;
}

inline float sigmoid(float x)
{
    return 1.0 / (1.0 + expf(-x));
}

inline float tanh(float x)
{
    return 2 * sigmoid(2 * x) - 1;
}

inline float inverse_sigmoid(float x)
{
    return -logf(1.0 / x - 1);
}

inline void softmax(float *x, int num)
{
    float max_input = x[0];
    for (int i = 1; i < num; i++) max_input = DL_MAX(max_input, x[i]);

    float sum = 0;
    for (int i = 0; i < num; i++) {
        x[i] = expf(x[i] - max_input);
        sum += x[i];
    }

    for (int i = 0; i < num; i++) x[i] /= sum;
}

inline float dfl_integral(float *data, int reg_max = 7)
{
    softmax(data, reg_max + 1);
    float sum = 0;
    for (int i = 0; i <= reg_max; i++) {
        sum += data[i] * i;
    }
    return sum;
}
} // namespace math
} // namespace dl
