# Deep Learning Library

This is a lightweight deep neural network inference library. It contains operations and models for basic deep learning framework. With efficient implementation, many applications like face detecion and recognition can be deployed with a limited resource. Developpers can easily use the interface and the data structure to build their own applications.

## Feature
- Fully connection
- CNNs
- Pooling
- Activations
- Quantization
- Batch normalization
- Mobilenet
- Shufflenet

## API Guides

This library contains three types of matrix, one float point and two fixed point. Float point and 16-bit fixed point are for weight coefficients, and 8-bit fixed point data is for image data only.

### Data structure

- Float point matrix

This is a common type of matrix, its data is in float point format.
```c
typedef struct
{
    int w;          /*!< Width */
    int h;          /*!< Height */
    int c;          /*!< Channel */
    int n;          /*!< Number of filter, input and output must be 1 */
    int stride;     /*!< Step between lines */
    fptp_t *item;   /*!< Data */
} dl_matrix3d_t;
```

- Fixed point matrix

Quantized data has `exponent` variable besides others in float point matrix. Its data is in 16-bit fixed point format. The value is between -32768 and 32767.
```c
typedef struct
{
    /******* fix start *******/
    int w;          /*!< Width */
    int h;          /*!< Height */
    int c;          /*!< Channel */
    int n;          /*!< Number of filter, input and output must be 1 */
    int stride;     /*!< Step between lines */
    int exponent;   /*!< Exponent for quantization */
    qtp_t *item;    /*!< Data */
    /******* fix end *******/
} dl_matrix3dq_t;
```

- 8-bit image matrix

Different from quantized matrix, image data usually use 0-255 value in pixels. This contains an image matrix, with its original value in 8-bit.
```c
typedef struct
{
    int w;          /*!< Width */
    int h;          /*!< Height */
    int c;          /*!< Channel */
    int n;          /*!< Number of filter, input and output must be 1 */
    int stride;     /*!< Step between lines */
    uc_t *item;     /*!< Data */
} dl_matrix3du_t;
```

All types of operations have two versions for float point and fixed point. In the following introduction, operations of only one type is listed for short.

The interfaces starts with certain prefixes.

`dl_matrix3d_` is for float point operations, `dl_matrix3dq_` is 16-bit fixed point operations, `dl_matrix3du_` is for 8-bit fixed point operations. Those operations usually support only one data type.

`dl_matrix3dff_` is for float point input with float point output operations, `dl_matrix3duf_` is for 8-bit fixed point input with float point output operations.

`dl_matrix3duq_` is for 8-bit fixed point input with 16-bit fixed point output operations, `dl_matrix3dqq_` is for 16-bit fixed point input with 16-bit fixed point output operations.


### Data layout

Data stroed in memory is in NHWC format, so the `stride` in the data structure above is `w * c`.

The provided coefficients are already transformed to this format.

### Matrix operation

#### Allocation and free

All matrix data start at allocation.
```c
dl_matrix3d_t *dl_matrix3d_alloc(int n, int w, int h, int c);
void dl_matrix3d_free(dl_matrix3d_t *m);
```

To reach a better performance, memory in internal SRAM will be firstly allocated, if the space of memory is not sufficient, then allocate in PSRAM.

#### Transform between float point and fixed point

The two types of data can be transformed from each other. We can get float point matrix from a quantized matrix:
```c
dl_matrix3d_t *dl_matrix3d_from_matrixq(dl_matrix3dq_t *m);
```
Also, we can get fixed point matrix from a float point matrix:
```c
dl_matrix3dq_t *dl_matrixq_from_matrix3d(dl_matrix3d_t *m);
```
The exponent of the quantized matrix is auto ajusted according to the data distribution. We can also point a fixed value with the interface:
```c
dl_matrix3dq_t *dl_matrixq_from_matrix3d_qmf(dl_matrix3d_t *m, int exponent);
```
All the transformaion will allocate a new matrix to store the resulting data.

To change the exponent of a quantized matrix, we need to allocate a matrix and call the interface:
```c
void dl_matrix3dq_shift_exponent(dl_matrix3dq_t *out, dl_matrix3dq_t *in, int exponent);
```

#### Concatenation

The two matrix can only be concatenated in channel dimension.

### Neural network

#### Fully connection

The size of input is (1, 1, 1, w), the size of filter is (1, h, w, 1).

#### Activation

- Relu
- Relu with clip
- Leaky relu
- Prelu

#### Pooling

- Global pooling
- Max pooling
- Average pooling

#### Padding

- Valid
- Same

There are two types of padding calculation in 'same' type, according to tensorflow, mxnet, and pytorch. The default one is as tensorflow, which adds padding in the right and bottom at first.

The same padding will create a new padded input, so for the performance reason, the old one will be released. If the original input should be kept, then pass the `PADDING_SAME_DONT_FREE_INPUT` as the padding type.

#### Convolution

- Fully convolution
- Depthwise convolution

There are some interfaces for common convolution, which calculates whatever shape of input and kernel. While for some usually used operations, there are some dedicated interfaces for convolution 1x1 and 3x3.

In quantized mode, some special intructions of XTENSA can be used to accelerate the dot production, with `DL_XTENSA_IMPL` mode. This is only suitable for ESP32 serial chips.

#### Advanced network

- Mobilenet

```c
dl_matrix3dq_t *dl_matrix3dqq_mobilenet(dl_matrix3dq_t *in,
                                        dl_matrix3dq_t *dilate,
                                        dl_matrix3dq_t *dilate_prelu,
                                        dl_matrix3dq_t *depthwise,
                                        dl_matrix3dq_t *depth_prelu,
                                        dl_matrix3dq_t *compress,
                                        dl_matrix3dq_t *bias,
                                        dl_matrix3dq_mobilenet_config_t config,
                                        char *name);
```

The dilated operation is an 1x1 convolution with prelu activation. After a depthwise convolution with prelu activation, another 1x1 convolution follows without any activation. The bias is for the last 1x1 convolution.


