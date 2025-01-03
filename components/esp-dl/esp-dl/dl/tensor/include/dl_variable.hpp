#pragma once

#include "dl_tensor_base.hpp"
#include "esp_heap_caps.h"
#include <iostream>
namespace dl {

/**
 * @brief Tensor
 *
 * @tparam T support int8_t, int16_t and float.
 */
template <typename T>
class Tensor : public TensorBase {
private:
    /**
     * @brief Get dtype from Template type
     *
     */
    dtype_t get_template_dtype()
    {
        if (std::is_same<T, int8_t>::value) {
            return DATA_TYPE_INT8;
        } else if (std::is_same<T, int16_t>::value) {
            return DATA_TYPE_INT16;
        } else if (std::is_same<T, float>::value) {
            return DATA_TYPE_FLOAT;
        } else if (std::is_same<T, int32_t>::value) {
            return DATA_TYPE_INT32;
        } else {
            return DATA_TYPE_UNDEFINED;
        }
        return DATA_TYPE_UNDEFINED;
    }

public:
    T *element; /*<! point to element */
    // std::vector<int> shape; /*<! shape of Tensor */

    /**
     * @brief Construct a new Tensor object
     *
     */
    Tensor() : element(NULL)
    {
        this->auto_free = true, this->exponent = 0;
        this->set_shape({0});
        this->dtype = this->get_template_dtype();
    }

    /**
     * @brief Construct a new Tensor object by copying from input.
     *
     * @param input an input Tensor
     * @param deep  one of true or false
     *              - true: apply a new memory, copy value from input.element to this new memory
     *              - false: take over input.element to this->element
     */
    Tensor(Tensor<T> &input, bool deep) : TensorBase()
    {
        this->auto_free = input.auto_free;
        this->exponent = input.exponent;
        this->set_shape(input.shape);
        this->dtype = input.dtype;
        if (deep && (input.element != NULL)) {
            int size_real = input.get_size();
            T *new_element = (T *)tool::calloc_aligned(size_real, sizeof(T), 16, MALLOC_CAP_8BIT);
            tool::copy_memory(new_element, input.element, size_real * sizeof(T));
            this->element = new_element;
        } else {
            this->element = input.element;
            this->auto_free = false;
        }
    }

    /**
     * @brief Construct a new Tensor object by element and shape.
     *
     */
    Tensor(std::vector<int> shape,
           T *element = nullptr,
           int exponent = 0,
           bool deep = true,
           uint32_t caps = MALLOC_CAP_8BIT) :
        TensorBase()
    {
        this->set_shape(shape);
        this->exponent = exponent;
        this->dtype = this->get_template_dtype();
        if (element) {
            if (deep) {
                this->auto_free = true;
                this->element = (T *)tool::malloc_aligned(this->get_size(), sizeof(T), 16, caps);
                tool::copy_memory(this->element, element, this->get_size() * sizeof(T));
            } else {
                this->auto_free = false;
                this->element = element;
            }
        } else {
            this->auto_free = true;
            this->element = (T *)tool::malloc_aligned(this->get_size(), sizeof(T), 16, caps);
        }
    }

    /**
     * @brief Construct a new Tensor object by element and shape.
     *
     */
    Tensor(
        std::vector<int> shape, const T *element, int exponent = 0, bool deep = true, uint32_t caps = MALLOC_CAP_8BIT) :
        TensorBase()
    {
        this->set_shape(shape);
        this->exponent = exponent;
        this->dtype = this->get_template_dtype();
        if (element) {
            if (deep) {
                this->auto_free = true;
                this->element = (T *)tool::malloc_aligned(this->get_size(), sizeof(T), 16, caps);
                tool::copy_memory(this->element, const_cast<T *>(element), this->get_size() * sizeof(T));
            } else {
                this->auto_free = false;
                this->element = const_cast<T *>(element);
            }
        } else {
            this->auto_free = true;
            this->element = (T *)tool::malloc_aligned(this->get_size(), sizeof(T), 16, caps);
        }
    }

    /**
     * @brief Destroy the Tensor object
     *
     */
    ~Tensor()
    {
        if (this->auto_free)
            this->free_element();
    }

    /**
     * @brief copy the element of the input Tensor.
     *
     * @param input an input Tensor
     * @param deep one of true or false
     *              - true: apply a new memory, copy value from input.element to this new memory
     *              - false: take over input.element to this->element
     * @return Tensor<T>& self
     */
    Tensor<T> &copy_element(Tensor<T> &input, bool deep)
    {
        assert(this->get_size() == input.get_size());
        assert(input.element != NULL);

        this->malloc_element();
        if (deep) {
            tool::copy_memory(this->element, input.element, this->get_size() * sizeof(T));
        } else {
            this->element = input.element;
            this->auto_free = false;
        }
        return *this;
    }

    /**
     * @brief Set the auto free object.
     *
     * @param auto_free one of true or false
     *                  - true: free element when object destroyed
     *                  - false: do not
     * @return self
     */
    Tensor<T> &set_auto_free(const bool auto_free)
    {
        this->auto_free = auto_free;
        return *this;
    }

    /**
     * @brief Set the element.
     *
     * @param element point to element memory
     * @return self
     */
    Tensor<T> &set_element(T *element, const bool auto_free = false)
    {
        // assert(this->element == NULL);
        this->element = element;
        this->auto_free = auto_free;

        return *this;
    }

    /**
     * @brief Set the exponent.
     *
     * @param exponent exponent of element
     * @return self
     */
    Tensor<T> &set_exponent(const int exponent)
    {
        this->exponent = exponent;

        return *this;
    }

    /**
     * @brief Set the shape of Tensor.
     *
     * @param shape the target shape
     *
     * @return self
     */
    Tensor<T> &set_shape(const std::vector<int> shape);

    /**
     * @brief print the shape of the Tensor
     *
     */
    void print_shape()
    {
        if (this->shape.size()) {
            printf("shape = (");
            for (int i = 0; i < this->shape.size() - 1; i++) {
                printf("%d, ", this->shape[i]);
            }
            printf("%d)\n", this->shape.back());
        } else {
            printf("shape = ()\n");
        }
    }

    /**
     * @brief flatten the Tensor
     *
     * @return Tensor<T>& self
     */
    Tensor<T> &flatten();

    /**
     * @brief Change a new shape to the Tensor without changing its data.
     *
     * @param shape  the target shape
     * @return Tensor<T>&  self
     */
    Tensor<T> &reshape(std::vector<int> shape);

    /**
     * @brief Remove dims with length==1 from Tensor
     *
     * @param axis the dim to to be remove. make sure the length of the dim is equal to 1.
     *              if axis == INT32_MAX, all the dims with length==1 will be removed.
     * @return Tensor<T>& self
     */
    Tensor<T> &squeeze(int axis = INT32_MAX);

    /**
     * @brief Insert a new dim that will appear at the axis position in the expanded Tensor shape.
     *
     * @param axis the dim to be inserted
     * @return Tensor<T>& self
     */
    Tensor<T> &expand_dims(int axis);

    /**
     * @brief Insert a new dim that will appear at the axis position in the expanded Tensor shape.
     *
     * @param axis  the dim to be inserted
     * @return Tensor<T>& self
     */
    Tensor<T> &expand_dims(std::vector<int> axis);

    /**
     * @brief Reverse or permute the axes of the Tensor
     *
     * @param perm the new arangement of the dims. if perm == {}, the dims arangement will be reversed.
     * @return Tensor<T>& self
     */
    Tensor<T> &transpose(std::vector<int> perm = {});

    /**
     * @brief Reverse or permute the axes of the input Tensor
     *
     * @param input the input Tensor
     * @param perm the new arangement of the dims. if perm == {}, the dims arangement will be reversed.
     * @return Tensor<T>& self
     */
    Tensor<T> &transpose(Tensor<T> &input, std::vector<int> perm = {});

    /**
     * @brief Get the element pointer.
     *
     * @return pointer to memory
     */
    void *get_element_ptr() { return this->element; }

    /**
     * @brief Get the element value.
     *
     * @param index   the index of each dim.
     * @return T element value
     */
    T get_element_value(const std::vector<int> index) { return this->element[this->get_element_index(index)]; }

    /**
     * @brief Get the element value.
     *
     * @param index  the index of the element.
     * @return T  element value
     */
    T get_element_value(int index) { return this->element[index]; }

    /**
     * @brief Set the all the element to value.
     *
     * @param value  target value
     * @return Tensor<T>& self
     */
    Tensor<T> &set_value(T value);

    /**
     * @brief Set the the element to value
     *
     * @param value target value, it will be broadcast automatically.
     * @return Tensor<T>& self
     */
    Tensor<T> &set_value(Tensor<T> &value);

    /**
     * @brief Set the sliced element to value
     *
     * @param axis_index_range range of slices
     * @param value target value
     * @return Tensor<T>& self
     */
    Tensor<T> &set_value(std::vector<int> axis_index_range, T value);

    /**
     * @brief Set the sliced element to value
     *
     * @param axis_index_range range of slices
     * @param value target value, it will be broadcast automatically.
     * @return Tensor<T>& self
     */
    Tensor<T> &set_value(std::vector<int> axis_index_range, Tensor<T> &value);

    /**
     * @brief Extracts a slice from the Tensor.
     *
     * @param axis_index_range range of slices
     * @return Tensor<T> output
     */
    Tensor<T> slice(std::vector<int> axis_index_range);

    /**
     * @brief Reverses specific dims of the tensor.
     *
     * @param axis The dims to be reversed
     * @return Tensor<T>&
     */
    Tensor<T> &reverse(std::vector<int> axis);

    /**
     * @brief Get the axis offset
     *
     * @return std::vector<int> the axis offset
     */
    std::vector<int> get_axis_offset() { return this->axis_offset; }

    /**
     * @brief Apply memory with zero-initialized only if this->element is NULL.
     *
     * @param auto_free one of true or false
     *                  - true: free element when object destroyed
     *                  - false: do not
     * @return
     *         - true: on success
     *         - false: if applying failed
     */
    bool calloc_element(const bool auto_free = true)
    {
        if (this->element != NULL)
            return false;

        this->element = (T *)tool::calloc_aligned(this->get_size(), sizeof(T), 16, MALLOC_CAP_8BIT);
        this->auto_free = auto_free;

        return true;
    }

    /**
     * @brief Apply memory without initialized only if this->element is NULL.
     *
     * @param auto_free one of true or false
     *                  - true: free element when object destroyed
     *                  - false: do not
     * @return
     *         - true: on success
     *         - false: if applying failed
     */
    bool malloc_element(const bool auto_free = true)
    {
        if (this->element != NULL)
            return false;

        this->element = (T *)tool::malloc_aligned(this->get_size(), sizeof(T), 16, MALLOC_CAP_8BIT);
        this->auto_free = auto_free;

        return true;
    }

    /**
     * @brief free element only if this->element != NULL
     * set this->element to NULL, after free
     * @brief Free element if this->element is not NULL.
     */
    void free_element()
    {
        if (this->auto_free && this->element) {
            tool::free_aligned(this->element);
            this->element = NULL;
        }
    }

    /**
     * @brief print the element of the tensor
     *
     * @param axis_index_range  the element range of each dims to be print. if axis_index_range == {}, all the element
     * will be print.
     * @param message  to print
     */
    void print(std::vector<int> axis_index_range = {}, const char *message = "");

    /**
     * @brief  print all the element of the Tensor.
     *
     * @param message to print
     */
    void print_all(const char *message = "")
    {
        std::cout << "\n" << message << " | ";
        this->print_shape();

        for (int i = 0; i < this->get_size(); i++) {
            std::cout << this->element[i] << " ";
        }
        std::cout << "\n";
        return;
    }

    /**
     * @brief Get the index of element
     *
     * @param axis_index the index of each dims
     * @return int the index of element
     */
    int get_element_index(const std::vector<int> axis_index);

    /**
     * @brief Check the element value with input ground-truth.
     *
     * @param gt_element ground-truth value of element
     * @param bias permissible error
     * @param info one of true or false
     *             - true:  shape and result
     *             - false: do not
     * @param failed_number maximum number of wrong element that will be printed
     *
     * @return
     *         - true: in permissible error
     *         - false: not
     */
    bool check_element(T *gt_element, int bias = 2, bool info = true, int failed_number = 0)
    {
        int count = 0;
        if (info)
            this->print_shape();
        int size = this->get_size();
        for (int i = 0; i < size; i++) {
            if (DL_ABS(this->element[i] - gt_element[i]) > bias) {
                std::vector<int> index = get_axis_index(i);
                std::cout << "element[";
                for (int j = 0; j < index.size() - 1; j++) {
                    std::cout << index[j] << ", ";
                }
                std::cout << index.back() << "]: ";
                std::cout << +this->element[i] << " v.s. " << +gt_element[i] << "\n";
                count++;
                if (count > failed_number)
                    return false;
            }
        }
        if (count)
            return false;

        if (info)
            printf("PASS\n");

        return true;
    }

    /**
     * @brief Check the shape is the same as the shape of input.
     *
     * @param input an input tensor
     * @return
     *         - true: same shape
     *         - false: not
     */
    bool is_same_shape(const TensorBase &input)
    {
        if (input.shape.size() != this->shape.size()) {
            return false;
        }
        for (int i = 0; i < this->shape.size(); i++) {
            if (input.shape[i] != this->shape[i]) {
                return false;
            }
        }
        return true;
    }

    Tensor<T> &operator=(const Tensor<T> &input)
    {
        this->auto_free = input.auto_free;
        this->exponent = input.exponent;
        int size_real_tmp = this->size;
        int size_input_real = input.size;
        this->set_shape(input.shape);
        if (input.element) {
            if (this->element) {
                if (size_real_tmp != size_input_real) {
                    tool::free_aligned(this->element);
                    T *new_element = (T *)tool::malloc_aligned(size_input_real, sizeof(T), 16, MALLOC_CAP_8BIT);
                    tool::copy_memory(new_element, input.element, size_input_real * sizeof(T));
                    this->element = new_element;
                } else {
                    tool::copy_memory(this->element, input.element, size_input_real * sizeof(T));
                }
            } else {
                T *new_element = (T *)tool::malloc_aligned(size_input_real, sizeof(T), 16, MALLOC_CAP_8BIT);
                tool::copy_memory(new_element, input.element, size_input_real * sizeof(T));
                this->element = new_element;
            }
            return *this;
        } else {
            if (this->element) {
                tool::free_aligned(this->element);
                this->element = NULL;
            }
            return *this;
        }
    }

    static Tensor<T> arange(int size)
    {
        Tensor<T> output;
        output.set_auto_free(true).set_exponent(0).set_shape({size}).malloc_element();
        for (int i = 0; i < size; ++i) {
            output.element[i] = i;
        }
        return output;
    }

    /**
     * @brief Copy element from input.
     * If the dtype of input is different from this tensor, convert the element of input to the dtype of this tensor and
     * copy them. If this tensor is empty, malloc the element and copy input element.
     *
     * @param input an input tensor
     * @return
     *         - true: same shape
     *         - false: not
     */
    bool convert_from(TensorBase *input);
    bool convert_from(const Tensor<int8_t> &input);
    bool convert_from(const Tensor<int16_t> &input);
    bool convert_from(const Tensor<float> &input);

    // /**
    //  * @brief Copy element to dest.
    //  * If the dtype of dest is different from this tensor, convert the element of this tensor to the dtype of dest
    //  and copy them.
    //  *
    //  * @param dest an input tensor
    //  * @return
    //  *         - true: same shape
    //  *         - false: not
    //  */
    // bool convert_to(Tensor<int8_t> &dest);
    // bool convert_to(Tensor<int16_t> &dest);
    // bool convert_to(Tensor<float> &dest);

    // /**
    //  * @brief Quantize input: round(clip((in * scale ), quant_min, quant_max))
    //  *
    // */
    // static float quantize(float input, float scale, float quant_min, float quant_max);

    // /**
    //  * @brief Dequantize input: input * scale
    // */
    // static float dequantize(int input, float scale);
};
} // namespace dl
