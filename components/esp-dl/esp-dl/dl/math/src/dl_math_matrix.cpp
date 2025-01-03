#include "dl_math_matrix.hpp"
#include "dl_math.hpp"

namespace dl {
namespace math {
template <typename T>
Matrix<T> Matrix<T>::matmul(const Matrix<T> &input) const
{
    assert(input.h == this->w);
    Matrix<T> output(this->h, input.w);
    for (int i = 0; i < output.h; i++) {
        for (int j = 0; j < output.w; j++) {
            for (int k = 0; k < this->w; k++) {
                output.array[i][j] += this->array[i][k] * input.array[k][j];
            }
        }
    }
    return output;
}
template Matrix<int> Matrix<int>::matmul(const Matrix<int> &input) const;
template Matrix<int16_t> Matrix<int16_t>::matmul(const Matrix<int16_t> &input) const;
template Matrix<int8_t> Matrix<int8_t>::matmul(const Matrix<int8_t> &input) const;
template Matrix<uint16_t> Matrix<uint16_t>::matmul(const Matrix<uint16_t> &input) const;
template Matrix<uint8_t> Matrix<uint8_t>::matmul(const Matrix<uint8_t> &input) const;
template Matrix<float> Matrix<float>::matmul(const Matrix<float> &input) const;
template Matrix<double> Matrix<double>::matmul(const Matrix<double> &input) const;

template <typename T>
Matrix<T> Matrix<T>::transpose() const
{
    Matrix<T> trans(this->w, this->h);
    for (int i = 0; i < this->h; i++) {
        for (int j = 0; j < this->w; j++) {
            trans.array[j][i] = this->array[i][j];
        }
    }
    return trans;
}
template Matrix<int> Matrix<int>::transpose() const;
template Matrix<int16_t> Matrix<int16_t>::transpose() const;
template Matrix<int8_t> Matrix<int8_t>::transpose() const;
template Matrix<uint16_t> Matrix<uint16_t>::transpose() const;
template Matrix<uint8_t> Matrix<uint8_t>::transpose() const;
template Matrix<float> Matrix<float>::transpose() const;
template Matrix<double> Matrix<double>::transpose() const;

template <typename T>
Matrix<T> Matrix<T>::inverse() const
{
    Matrix<T> inv(this->h, this->w);
    if (this->w != this->h) {
        printf("this matrix is not a square matrix !\n");
        return inv;
    }

    Matrix<T> matw(this->h, 2 * (this->w));
    T **w = matw.array;
    float eps = 1e-6;

    for (int i = 0; i < matw.h; i++) {
        for (int j = 0; j < this->w; j++) {
            w[i][j] = this->array[i][j];
        }
        w[i][(this->w) + i] = 1;
    }

    for (int i = 0; i < matw.h; i++) {
        if (DL_ABS(w[i][i]) < eps) {
            int j;
            for (j = i + 1; j < matw.h; j++) {
                if (DL_ABS(w[j][i]) > eps)
                    break;
            }
            if (j == matw.h) {
                printf("This matrix is irreversible!\n");
                return inv;
            }
            for (int k = i; k < matw.w; k++) {
                w[i][k] += w[j][k];
            }
        }
        float factor = w[i][i];
        for (int k = i; k < matw.w; k++) {
            w[i][k] /= factor;
        }
        for (int k = i + 1; k < matw.h; k++) {
            factor = -w[k][i];
            for (int l = i; l < matw.w; l++) {
                w[k][l] += (factor * w[i][l]);
            }
        }
    }
    for (int i = (matw.h) - 1; i > 0; i--) {
        for (int j = i - 1; j >= 0; j--) {
            float factor = -w[j][i];
            for (int k = i; k < matw.w; k++) {
                w[j][k] += (factor * w[i][k]);
            }
        }
    }
    for (int i = 0; i < this->h; i++) {
        for (int j = 0; j < this->w; j++) {
            inv.array[i][j] = w[i][(this->h) + j];
        }
    }
    return inv;
}
template Matrix<int> Matrix<int>::inverse() const;
template Matrix<int16_t> Matrix<int16_t>::inverse() const;
template Matrix<int8_t> Matrix<int8_t>::inverse() const;
template Matrix<uint16_t> Matrix<uint16_t>::inverse() const;
template Matrix<uint8_t> Matrix<uint8_t>::inverse() const;
template Matrix<float> Matrix<float>::inverse() const;
template Matrix<double> Matrix<double>::inverse() const;

template <typename T>
Matrix<T> Matrix<T>::diagonal() const
{
    int rank = DL_MIN(this->h, this->w);
    Matrix<T> diag_m(1, rank);
    for (int i = 0; i < rank; ++i) {
        diag_m.array[0][i] = this->array[i][i];
    }
    return diag_m;
}
template Matrix<int> Matrix<int>::diagonal() const;
template Matrix<int16_t> Matrix<int16_t>::diagonal() const;
template Matrix<int8_t> Matrix<int8_t>::diagonal() const;
template Matrix<uint16_t> Matrix<uint16_t>::diagonal() const;
template Matrix<uint8_t> Matrix<uint8_t>::diagonal() const;
template Matrix<float> Matrix<float>::diagonal() const;
template Matrix<double> Matrix<double>::diagonal() const;

template <typename T>
Matrix<T> Matrix<T>::slice(int h_start, int h_end, int w_start, int w_end) const
{
    assert((h_end >= h_start) && (w_end >= w_start) && (h_start >= 0) && (w_start >= 0));
    int slice_h = DL_MIN((h_end - h_start), (this->h - h_start));
    int slice_w = DL_MIN((w_end - w_start), (this->w - w_start));
    Matrix<T> slice_m(slice_h, slice_w);
    for (int i = 0; i < slice_h; ++i) {
        for (int j = 0; j < slice_w; ++j) {
            slice_m.array[i][j] = this->array[h_start + i][w_start + j];
        }
    }
    return slice_m;
}
template Matrix<int> Matrix<int>::slice(int h_start, int h_end, int w_start, int w_end) const;
template Matrix<int16_t> Matrix<int16_t>::slice(int h_start, int h_end, int w_start, int w_end) const;
template Matrix<int8_t> Matrix<int8_t>::slice(int h_start, int h_end, int w_start, int w_end) const;
template Matrix<uint16_t> Matrix<uint16_t>::slice(int h_start, int h_end, int w_start, int w_end) const;
template Matrix<uint8_t> Matrix<uint8_t>::slice(int h_start, int h_end, int w_start, int w_end) const;
template Matrix<float> Matrix<float>::slice(int h_start, int h_end, int w_start, int w_end) const;
template Matrix<double> Matrix<double>::slice(int h_start, int h_end, int w_start, int w_end) const;

Matrix<float> get_affine_transform(Matrix<float> &source_coord, Matrix<float> &dest_coord)
{
    Matrix<float> m(3, 3);
    float Ainv[3][3] = {0};
    float Adet =
        (source_coord.array[0][0] * source_coord.array[1][1] + source_coord.array[0][1] * source_coord.array[2][0] +
         source_coord.array[1][0] * source_coord.array[2][1]) -
        (source_coord.array[2][0] * source_coord.array[1][1] + source_coord.array[1][0] * source_coord.array[0][1] +
         source_coord.array[0][0] * source_coord.array[2][1]);
    if (Adet == 0) {
        printf("the src is linearly dependent\n");
        return m;
    }
    Ainv[0][0] = (source_coord.array[1][1] - source_coord.array[2][1]) / Adet;
    Ainv[0][1] = (source_coord.array[2][1] - source_coord.array[0][1]) / Adet;
    Ainv[0][2] = (source_coord.array[0][1] - source_coord.array[1][1]) / Adet;
    Ainv[1][0] = (source_coord.array[2][0] - source_coord.array[1][0]) / Adet;
    Ainv[1][1] = (source_coord.array[0][0] - source_coord.array[2][0]) / Adet;
    Ainv[1][2] = (source_coord.array[1][0] - source_coord.array[0][0]) / Adet;
    Ainv[2][0] =
        (source_coord.array[1][0] * source_coord.array[2][1] - source_coord.array[2][0] * source_coord.array[1][1]) /
        Adet;
    Ainv[2][1] =
        (source_coord.array[2][0] * source_coord.array[0][1] - source_coord.array[0][0] * source_coord.array[2][1]) /
        Adet;
    Ainv[2][2] =
        (source_coord.array[0][0] * source_coord.array[1][1] - source_coord.array[0][1] * source_coord.array[1][0]) /
        Adet;

    for (int i = 0; i < 3; i++) {
        m.array[0][i] = Ainv[i][0] * dest_coord.array[0][0] + Ainv[i][1] * dest_coord.array[1][0] +
            Ainv[i][2] * dest_coord.array[2][0];
        m.array[1][i] = Ainv[i][0] * dest_coord.array[0][1] + Ainv[i][1] * dest_coord.array[1][1] +
            Ainv[i][2] * dest_coord.array[2][1];
    }
    m.array[2][0] = 0;
    m.array[2][1] = 0;
    m.array[2][2] = 1;
    return m;
}

Matrix<float> get_similarity_transform(Matrix<float> &source_coord, Matrix<float> &dest_coord)
{
    int num = source_coord.h;
    int dim = 2;
    double src_mean_x = 0.0;
    double src_mean_y = 0.0;
    double dst_mean_x = 0.0;
    double dst_mean_y = 0.0;
    Matrix<float> T(3, 3);

    for (int i = 0; i < num; i++) {
        src_mean_x += source_coord.array[i][0];
        src_mean_y += source_coord.array[i][1];
        dst_mean_x += dest_coord.array[i][0];
        dst_mean_y += dest_coord.array[i][1];
    }
    src_mean_x /= num;
    src_mean_y /= num;
    dst_mean_x /= num;
    dst_mean_y /= num;

    Matrix<float> src_demean(num, 2);
    Matrix<float> dst_demean(num, 2);
    for (int i = 0; i < num; i++) {
        src_demean.array[i][0] = source_coord.array[i][0] - src_mean_x;
        src_demean.array[i][1] = source_coord.array[i][1] - src_mean_y;
        dst_demean.array[i][0] = dest_coord.array[i][0] - dst_mean_x;
        dst_demean.array[i][1] = dest_coord.array[i][1] - dst_mean_y;
    }
    double A[2][2] = {0};
    for (int i = 0; i < num; i++) {
        A[0][0] += (dst_demean.array[i][0] * src_demean.array[i][0] / num);
        A[0][1] += (dst_demean.array[i][0] * src_demean.array[i][1] / num);
        A[1][0] += (dst_demean.array[i][1] * src_demean.array[i][0] / num);
        A[1][1] += (dst_demean.array[i][1] * src_demean.array[i][1] / num);
    }
    if ((A[0][0] == 0) && (A[0][1] == 0) && (A[1][0] == 0) && (A[1][1] == 0)) {
        return T;
    }

    double d[2] = {1, 1};
    if (((A[0][0] * A[1][1]) - A[0][1] * A[1][0]) < 0) {
        d[1] = -1;
    }

    //======================================================================SVD=====================================================================
    double U[2][2] = {0};
    double V[2][2] = {0};
    double S[2] = {0};

    double divide_temp = 0;

    double AAT[2][2] = {0};
    AAT[0][0] = A[0][0] * A[0][0] + A[0][1] * A[0][1];
    AAT[0][1] = A[0][0] * A[1][0] + A[0][1] * A[1][1];
    AAT[1][0] = A[1][0] * A[0][0] + A[1][1] * A[0][1];
    AAT[1][1] = A[1][0] * A[1][0] + A[1][1] * A[1][1];

    double l1 = (AAT[0][0] + AAT[1][1] +
                 sqrt_newton((AAT[0][0] + AAT[1][1]) * (AAT[0][0] + AAT[1][1]) -
                             4 * ((AAT[0][0] * AAT[1][1]) - (AAT[0][1] * AAT[1][0])))) /
        2.0;
    double l2 = (AAT[0][0] + AAT[1][1] -
                 sqrt_newton((AAT[0][0] + AAT[1][1]) * (AAT[0][0] + AAT[1][1]) -
                             4 * ((AAT[0][0] * AAT[1][1]) - (AAT[0][1] * AAT[1][0])))) /
        2.0;
    S[0] = sqrt_newton(l1);
    S[1] = sqrt_newton(l2);

    U[0][0] = 1.0;
    divide_temp = l1 - AAT[1][1];
    if (divide_temp == 0) {
        return T;
    }
    U[1][0] = AAT[1][0] / divide_temp;
    double norm = sqrt_newton((U[0][0] * U[0][0]) + (U[1][0] * U[1][0]));
    U[0][0] /= norm;
    U[1][0] /= norm;

    U[0][1] = 1.0;
    divide_temp = l2 - AAT[1][1];
    if (divide_temp == 0) {
        return T;
    }
    U[1][1] = AAT[1][0] / divide_temp;
    norm = sqrt_newton((U[0][1] * U[0][1]) + (U[1][1] * U[1][1]));
    U[0][1] /= norm;
    U[1][1] /= norm;

    if (U[0][1] * U[1][0] < 0) {
        U[0][0] = -U[0][0];
        U[1][0] = -U[1][0];
    }

    double ATA[2][2] = {0};
    ATA[0][0] = A[0][0] * A[0][0] + A[1][0] * A[1][0];
    ATA[0][1] = A[0][0] * A[0][1] + A[1][0] * A[1][1];
    ATA[1][0] = A[0][1] * A[0][0] + A[1][1] * A[1][0];
    ATA[1][1] = A[0][1] * A[0][1] + A[1][1] * A[1][1];

    V[0][0] = 1.0;
    divide_temp = l1 - ATA[1][1];
    if (divide_temp == 0) {
        return T;
    }
    V[0][1] = ATA[1][0] / divide_temp;
    norm = sqrt_newton((V[0][0] * V[0][0]) + (V[0][1] * V[0][1]));
    V[0][0] /= norm;
    V[0][1] /= norm;

    V[1][0] = 1.0;
    divide_temp = l2 - ATA[1][1];
    if (divide_temp == 0) {
        return T;
    }
    V[1][1] = ATA[1][0] / divide_temp;
    norm = sqrt_newton((V[1][0] * V[1][0]) + (V[1][1] * V[1][1]));
    V[1][0] /= norm;
    V[1][1] /= norm;

    if (V[0][1] * V[1][0] < 0) {
        V[0][0] = -V[0][0];
        V[0][1] = -V[0][1];
    }
    if ((S[0] * U[0][0] * V[0][0] + S[1] * U[0][1] * V[1][0]) * A[0][0] < 0) {
        U[0][0] = -U[0][0];
        U[0][1] = -U[0][1];
        U[1][0] = -U[1][0];
        U[1][1] = -U[1][1];
    }
    //============================================================================================================================================

    if (DL_ABS((A[0][0] * A[1][1]) - A[0][1] * A[1][0]) < 1e-8) {
        if ((((U[0][0] * U[1][1]) - U[0][1] * U[1][0]) * ((V[0][0] * V[1][1]) - V[0][1] * V[1][0])) > 0) {
            T.array[0][0] = U[0][0] * V[0][0] + U[0][1] * V[1][0];
            T.array[0][1] = U[0][0] * V[0][1] + U[0][1] * V[1][1];
            T.array[1][0] = U[1][0] * V[0][0] + U[1][1] * V[1][0];
            T.array[1][1] = U[1][0] * V[0][1] + U[1][1] * V[1][1];
        } else {
            double s = d[dim - 1];
            d[dim - 1] = -1;
            T.array[0][0] = d[0] * U[0][0] * V[0][0] + d[1] * U[0][1] * V[1][0];
            T.array[0][1] = d[0] * U[0][0] * V[0][1] + d[1] * U[0][1] * V[1][1];
            T.array[1][0] = d[0] * U[1][0] * V[0][0] + d[1] * U[1][1] * V[1][0];
            T.array[1][1] = d[0] * U[1][0] * V[0][1] + d[1] * U[1][1] * V[1][1];
            d[dim - 1] = s;
        }
    } else {
        T.array[0][0] = d[0] * U[0][0] * V[0][0] + d[1] * U[0][1] * V[1][0];
        T.array[0][1] = d[0] * U[0][0] * V[0][1] + d[1] * U[0][1] * V[1][1];
        T.array[1][0] = d[0] * U[1][0] * V[0][0] + d[1] * U[1][1] * V[1][0];
        T.array[1][1] = d[0] * U[1][0] * V[0][1] + d[1] * U[1][1] * V[1][1];
    }

    double Ex = 0.0;
    double Ex2 = 0.0;
    double Ey = 0.0;
    double Ey2 = 0.0;
    for (int i = 0; i < num; i++) {
        Ex += src_demean.array[i][0];
        Ex2 += (src_demean.array[i][0] * src_demean.array[i][0]);
        Ey += src_demean.array[i][1];
        Ey2 += (src_demean.array[i][1] * src_demean.array[i][1]);
    }
    Ex /= num;
    Ex2 /= num;
    Ey /= num;
    Ey2 /= num;
    double var_sum = (Ex2 - Ex * Ex) + (Ey2 - Ey * Ey);
    double scale = (S[0] * d[0] + S[1] * d[1]) / var_sum;

    T.array[0][2] = dst_mean_x - scale * (T.array[0][0] * src_mean_x + T.array[0][1] * src_mean_y);
    T.array[1][2] = dst_mean_y - scale * (T.array[1][0] * src_mean_x + T.array[1][1] * src_mean_y);

    T.array[0][0] *= scale;
    T.array[0][1] *= scale;
    T.array[1][0] *= scale;
    T.array[1][1] *= scale;
    T.array[2][0] = 0;
    T.array[2][1] = 0;
    T.array[2][2] = 1;

    return T;
}

Matrix<float> get_perspective_transform(Matrix<float> &source_coord, Matrix<float> &dest_coord)
{
    Matrix<float> m(3, 3);
    Matrix<float> A(8, 8);

    for (int i = 0; i < 4; i++) {
        A.array[i][0] = source_coord.array[i][0];
        A.array[i][1] = source_coord.array[i][1];
        A.array[i][2] = 1;
        A.array[i][3] = 0;
        A.array[i][4] = 0;
        A.array[i][5] = 0;
        A.array[i][6] = -dest_coord.array[i][0] * source_coord.array[i][0];
        A.array[i][7] = -dest_coord.array[i][0] * source_coord.array[i][1];
    }
    for (int i = 4; i < 8; i++) {
        A.array[i][0] = 0;
        A.array[i][1] = 0;
        A.array[i][2] = 0;
        A.array[i][3] = source_coord.array[i - 4][0];
        A.array[i][4] = source_coord.array[i - 4][1];
        A.array[i][5] = 1;
        A.array[i][6] = -dest_coord.array[i - 4][1] * source_coord.array[i - 4][0];
        A.array[i][7] = -dest_coord.array[i - 4][1] * source_coord.array[i - 4][1];
    }
    Matrix<float> Ainv = A.inverse();
    for (int i = 0; i < 8; i++) {
        m.array[i / 3][i % 3] =
            (((Ainv.array[i][0]) * dest_coord.array[0][0]) + ((Ainv.array[i][1]) * dest_coord.array[1][0]) +
             ((Ainv.array[i][2]) * dest_coord.array[2][0]) + ((Ainv.array[i][3]) * dest_coord.array[3][0]) +
             ((Ainv.array[i][4]) * dest_coord.array[0][1]) + ((Ainv.array[i][5]) * dest_coord.array[1][1]) +
             ((Ainv.array[i][6]) * dest_coord.array[2][1]) + ((Ainv.array[i][7]) * dest_coord.array[3][1]));
    }
    m.array[2][2] = 1;
    return m;
}
} // namespace math
} // namespace dl
