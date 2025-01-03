#include "dl_memory_manager.hpp"

namespace dl {
namespace memory {

/*oooooooooooooooooo00000000000000000000 MemoryManagerBase 00000000000000000000ooooooooooooooooo*/

void MemoryManagerBase::reset()
{
    if (!this->tensors.empty()) {
        for (int i = 0; i < this->tensors.size(); ++i) {
            delete tensors[i];
        }
        this->tensors.clear();
    }
    this->root_free();
    this->name2index.clear();
}

TensorBase *MemoryManagerBase::get_tensor(int index)
{
    if (index < 0 || index >= this->tensors.size()) {
        return nullptr;
    }
    return this->tensors[index];
}

TensorBase *MemoryManagerBase::get_tensor(std::string &name)
{
    auto it = this->name2index.find(name);
    if (it != name2index.end()) {
        return tensors[it->second];
    } else {
        return nullptr;
    }

    return nullptr;
}

int MemoryManagerBase::get_tensor_index(std::string &name)
{
    auto it = this->name2index.find(name);
    if (it != name2index.end()) {
        return it->second;
    } else {
        return -1;
    }

    return -1;
}

bool MemoryManagerBase::root_calloc(size_t internal_size, size_t psram_size)
{
    if (internal_size > 0) {
        this->internal_root = tool::calloc_aligned(internal_size, 1, alignment, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        if (this->internal_root == nullptr) {
            return false;
        }
    }

    if (psram_size > 0) {
        this->psram_root = tool::calloc_aligned(psram_size, 1, alignment, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
        if (this->psram_root == nullptr) {
            return false;
        }
    }

    return true;
}

void *MemoryManagerBase::psram_root_calloc(size_t psram_size)
{
    if (psram_size > 0) {
        this->psram_root = tool::calloc_aligned(psram_size, 1, alignment, MALLOC_CAP_8BIT | MALLOC_CAP_SPIRAM);
        if (this->psram_root)
            this->psram_size = psram_size;
    }
    return this->psram_root;
}

void *MemoryManagerBase::internal_root_calloc(size_t internal_size)
{
    if (internal_size > 0) {
        this->internal_root = tool::calloc_aligned(internal_size, 1, alignment, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
        if (this->internal_root)
            this->internal_size = internal_size;
    }
    return this->internal_root;
}

void MemoryManagerBase::root_free()
{
    // In IDF, free(p) is equivalent to heap_caps_free(p).
    if (this->internal_root) {
        ::free(this->internal_root);
        this->internal_root = nullptr;
    }
    if (this->psram_root) {
        ::free(this->psram_root);
        this->psram_root = nullptr;
    }
}

/*oooooooooooooooooo00000000000000000000 TensorInfo 00000000000000000000ooooooooooooooooo*/

TensorInfo::TensorInfo(std::string &name,
                       int time_begin,
                       int time_end,
                       std::vector<int> shape,
                       dtype_t dtype,
                       int exponent,
                       bool is_internal) :
    name(name),
    time_begin(time_begin),
    time_end(time_end),
    dtype(dtype),
    exponent(exponent),
    is_internal(is_internal),
    m_leader_tensor(nullptr),
    m_follower_dirty_tensor(nullptr)
{
    if (shape.size() > 0) {
        this->shape.push_back(shape[0]);
        this->size = shape[0];
        for (int i = 1; i < shape.size(); i++) {
            this->shape.push_back(shape[i]);
            this->size *= shape[i];
        }
    }

    if (dtype == DATA_TYPE_FLOAT || dtype == DATA_TYPE_INT32 || dtype == DATA_TYPE_UINT32) {
        this->size = this->size * 4;
    } else if (dtype == DATA_TYPE_UINT8 || dtype == DATA_TYPE_INT8) {
        this->size = this->size * 1;
    } else if (dtype == DATA_TYPE_UINT16 || dtype == DATA_TYPE_INT16) {
        this->size = this->size * 2;
    } else if (dtype == DATA_TYPE_DOUBLE || dtype == DATA_TYPE_INT64 || dtype == DATA_TYPE_UINT64) {
        this->size = this->size * 8;
    }

    this->call_times = 0;
    this->offset = 0;
    this->internal_offset = 0;
}

void TensorInfo::set_inplace_leader_tensor(TensorInfo *tensor)
{
    this->m_leader_tensor = tensor;
    if (tensor) {
        if (tensor->time_end < this->time_end || this->time_end == -1) {
            tensor->update_time(this->time_end);
        }
    }
}

void TensorInfo::update_time(int new_time)
{
    if (m_leader_tensor) { // if inplace tensor is not null, update end time of inplace tensor
        m_leader_tensor->update_time(new_time);
        this->time_end = m_leader_tensor->time_end;
    } else {
        if (new_time == -1) {
            this->time_end = -1;
        } else {
            if (new_time > this->time_end) {
                this->time_end = new_time;
            }
        }
    }

    this->call_times++;
}

TensorBase *TensorInfo::create_tensor(void *internal_root, void *psram_root)
{
    TensorBase *tensor = nullptr;
    uint8_t *element = nullptr;

    if (this->is_internal) {
        element = (uint8_t *)internal_root + this->get_internal_offset();
    } else {
        element = (uint8_t *)psram_root + this->get_offset();
    }

    tensor = new TensorBase(shape, element, exponent, dtype, false);
    return tensor;
}

/*oooooooooooooooooo00000000000000000000 MemoryChunk 00000000000000000000ooooooooooooooooo*/

MemoryChunk::MemoryChunk(size_t size, int is_free, int alignment) : size(size), is_free(is_free), alignment(alignment)
{
    this->tensor = nullptr;
    this->offset = 0;
}

MemoryChunk::MemoryChunk(TensorInfo *tensor, int alignment)
{
    this->alignment = alignment;
    this->size = this->get_aligned_size(tensor->get_size());
    this->is_free = false;
    this->tensor = tensor;
    this->offset = 0;
}

MemoryChunk *MemoryChunk::merge_free_chunk(MemoryChunk *chunk)
{
    if (chunk != nullptr) {
        if (chunk->is_free) {
            this->size = this->size + chunk->size;
            if (chunk->offset < this->offset) {
                this->offset = chunk->offset;
            }
            return this;
        }
    }
    return nullptr;
}

MemoryChunk *MemoryChunk::insert(TensorInfo *tensor)
{
    int aligned_size = this->get_aligned_size(tensor->get_size());

    if (this->is_free && this->size >= aligned_size) {
        this->tensor = tensor;
        this->is_free = false;
        if (this->size > aligned_size) {
            MemoryChunk *chunk = new MemoryChunk(this->size - aligned_size, true, this->alignment);
            this->size = aligned_size;
            chunk->offset = this->offset + aligned_size;
            return chunk;
        }
    }
    return nullptr;
}

MemoryChunk *MemoryChunk::extend(TensorInfo *tensor)
{
    int aligned_size = this->get_aligned_size(tensor->get_size());

    // only extend the size of memory chunk
    if (this->is_free && this->size < aligned_size) {
        this->tensor = tensor;
        this->is_free = false;
        this->size = aligned_size;
        return this;
    }
    return nullptr;
}

size_t MemoryChunk::get_aligned_size(size_t size)
{
    int remainder = size % this->alignment;

    if (remainder != 0) {
        return size + this->alignment - remainder;
    }

    return size;
}

void print_memory_list(const char *tag, std::list<MemoryChunk *> &memory_list)
{
    for (auto it = memory_list.begin(); it != memory_list.end(); ++it) {
        std::string name = "";
        std::string state = "false";
        if ((*it)->tensor) {
            name = (*it)->tensor->get_name();
        }
        if ((*it)->is_free) {
            state = "true";
        }
        ESP_LOGI(tag,
                 "[size:%d, offset:%d, free:%s, tensor:%s] -> ",
                 (*it)->size,
                 (*it)->offset,
                 state.c_str(),
                 name.c_str());
    }
    printf("\n");
}

void sort_memory_list(std::list<MemoryChunk *> &memory_list)
{
    // sort free list by size
    memory_list.sort([](MemoryChunk *a, MemoryChunk *b) {
        return a->size < b->size; // 升序排序
    });
}

} // namespace memory
} // namespace dl
