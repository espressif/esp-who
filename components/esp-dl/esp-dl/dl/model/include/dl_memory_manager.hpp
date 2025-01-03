#pragma once

#include "dl_module_base.hpp"
#include "esp_heap_caps.h"
#include "fbs_model.hpp"
#include <list>

namespace dl {
namespace memory {

/**
 * @brief Memory manager base class, each model has its own memory manager
 * TODO: share memory manager with different models
 */
class MemoryManagerBase {
private:
    void *psram_root;    // PSRAM root pointer
    void *internal_root; // Internal ram pointer

public:
    std::vector<TensorBase *> tensors;     // All tensors in the model
    int alignment;                         // The root pointer needs to be aligned must be a power of two
    std::map<std::string, int> name2index; // Tensor name to index map
    size_t internal_size;                  // The bytes of internal ram
    size_t psram_size;                     // The bytes of psram

    /**
     * @brief Construct a new MemoryManager object.
     */
    MemoryManagerBase(size_t internal_size, int alignment = 16) :
        psram_root(nullptr),
        internal_root(nullptr),
        tensors({}),
        alignment(alignment),
        internal_size(internal_size),
        psram_size(0)
    {
    }

    /**
     * @brief Destroy the MemoryManager object. Return resource.
     */
    virtual ~MemoryManagerBase() { this->reset(); }

    /**
     * @brief Allocate memory for each tensor, include all input and output tensors
     *
     * @param fbs_model       FlatBuffer's Model
     * @param execution_plan  Topological sorted module list
     *
     * @return The output TensorBase vector
     */
    virtual std::vector<TensorBase *> alloc(fbs::FbsModel *fbs_model, std::vector<dl::module::Module *> &execution_plan)
    {
        return {};
    }

    /**
     * @brief Set preload address for module's parameters
     *
     * @param execution_plan   Topological sorted module list
     */
    virtual void set_preload_addr(std::vector<dl::module::Module *> execution_plan) {}

    /**
     * @brief Reset the memory manager, free all memory for each tensor, include all input and output tensors
     */
    virtual void reset();

    /**
     * @brief Get tensor by index
     *
     * @param index The tensor index, type: int
     *
     * @return The TensorBase pointer
     */
    TensorBase *get_tensor(int index);

    /**
     * @brief Get tensor by name
     *
     * @param name The tensor name, type: std::string
     *
     * @return The TensorBase pointer
     */
    TensorBase *get_tensor(std::string &name);

    /**
     * @brief Get tensor index by name
     *
     * @param name The tensor name, type: std::string
     *
     * @return The TensorBase index
     */
    int get_tensor_index(std::string &name);

    /**
     * @brief Allocate root pointer by dl::tool::calloc_aligned API
     *
     * @param internal_size      Size, in bytes, of a chunk of Internal ram to allocate
     * @param psram_size             Size, in bytes, of a chunk of PSRAM to allocate
     *
     * @return true if success, false if fail
     */
    bool root_calloc(size_t internal_size, size_t psram_size);

    /**
     * @brief Allocate root pointer by dl::tool::calloc_aligned API
     *
     * @param psram_size             Size, in bytes, of a chunk of PSRAM to allocate
     *
     * @return data pointer if success, nullptr if fail
     */
    void *psram_root_calloc(size_t psram_size);

    /**
     * @brief Allocate root pointer by dl::tool::calloc_aligned API
     *
     * @param internal_size           Size, in bytes, of a chunk of Internal ram to allocate
     *
     * @return data pointer if success, nullptr if fail
     */
    void *internal_root_calloc(size_t internal_size);

    /**
     * @brief Free psram root and internal root pointer
     */
    void root_free();

    /**
     * @brief Get psram root pointer
     */
    void *get_psram_root() { return this->psram_root; }

    /**
     * @brief Get internal ram root pointer
     */
    void *get_internal_root() { return this->internal_root; }
};

/**
 * @brief Tensor info, include tensor name, shape, dtype, size, time range
 * and call times, which is used to plan model memory
 */
class TensorInfo {
private:
    std::string name;
    int time_begin;
    int time_end;
    std::vector<int> shape;
    dtype_t dtype;
    int exponent;
    size_t size; // Size, in bytes
    uint32_t call_times;
    uint32_t offset;          // PSRAM offset
    uint32_t internal_offset; // Internal ram offset, used to allocate tensor on both PSRAM and internal ram
    bool is_internal;
    TensorInfo *m_leader_tensor;
    TensorInfo
        *m_follower_dirty_tensor; // Only reference the follower tensor which will modify the data of leader tensor.

public:
    TensorInfo(std::string &name,
               int time_begin,
               int time_end,
               std::vector<int> shape,
               dtype_t dtype,
               int exponent,
               bool is_internal = false);

    ~TensorInfo() {}

    void set_inplace_leader_tensor(TensorInfo *tensor);

    void set_inplace_follower_tensor(TensorInfo *tensor) { m_follower_dirty_tensor = tensor; }

    TensorInfo *get_inplace_follower_tensor() { return m_follower_dirty_tensor; }

    void update_time(int new_time);

    TensorBase *create_tensor(void *internal_root, void *psram_root);

    bool is_inplaced() { return this->m_leader_tensor != nullptr; }

    uint32_t get_offset()
    {
        if (m_leader_tensor) {
            return m_leader_tensor->get_offset();
        }
        return this->offset;
    }

    void set_offset(uint32_t offset)
    {
        if (m_leader_tensor) {
            m_leader_tensor->set_offset(offset);
        }
        this->offset = offset;
    }

    uint32_t get_internal_offset()
    {
        if (m_leader_tensor) {
            return m_leader_tensor->get_internal_offset();
        }
        return this->internal_offset;
    }

    bool get_internal_state()
    {
        if (m_leader_tensor) {
            return m_leader_tensor->get_internal_state();
        }
        return this->is_internal;
    }

    void set_internal_state(bool is_internal)
    {
        if (m_leader_tensor) {
            m_leader_tensor->set_internal_state(is_internal);
        }
        this->is_internal = is_internal;
    }

    void set_internal_offset(uint32_t offset)
    {
        if (m_leader_tensor) {
            m_leader_tensor->set_internal_offset(offset);
            m_leader_tensor->set_internal_state(true);
        }
        this->is_internal = true;
        this->internal_offset = offset;
    }

    int get_time_end()
    {
        if (m_leader_tensor) {
            return m_leader_tensor->get_time_end();
        }
        return this->time_end;
    }

    int get_time_begin()
    {
        if (m_leader_tensor) {
            return m_leader_tensor->get_time_begin();
        }
        return this->time_begin;
    }

    size_t get_size() { return this->size; }

    std::string get_name() { return this->name; }

    std::vector<int> get_shape() { return this->shape; }

    void print()
    {
        printf("name:%s, from %d to %d, size:%d, offset:(%ld, %ld)\n",
               name.c_str(),
               time_begin,
               time_end,
               size,
               offset,
               internal_offset);
    }
};

/**
 * @brief Memory chunk, include size, is free, offset, alignment and tensor, which is used to simulate memory allocation
 */
class MemoryChunk {
public:
    size_t size;
    bool is_free;
    int offset;
    int alignment;
    TensorInfo *tensor;

    MemoryChunk(size_t size, int is_free, int alignment = 16);

    MemoryChunk(TensorInfo *tensor, int alignment = 16);

    ~MemoryChunk() {}

    /**
     * @brief Merge continuous free chunk
     */
    MemoryChunk *merge_free_chunk(MemoryChunk *chunk);

    /**
     * @brief Insert tensor into free chunk
     */
    MemoryChunk *insert(TensorInfo *tensor);

    /**
     * @brief Extend free chunk and insert tensor
     */
    MemoryChunk *extend(TensorInfo *tensor);

    /**
     * @brief Free memory chunk, set is_free to true and set tensor to nullptr
     */
    void free()
    {
        this->is_free = true;
        this->tensor = nullptr;
    }

    /**
     * @brief get aligned size, which is 16/alignemt bytes aligned
     */
    size_t get_aligned_size(size_t size);
};

/**
 * @brief print memory list
 */
void print_memory_list(const char *tag, std::list<MemoryChunk *> &memory_list);

/**
 * @brief sort memory list by memory chunk size
 */
void sort_memory_list(std::list<MemoryChunk *> &memory_list);

}; // namespace memory
} // namespace dl
