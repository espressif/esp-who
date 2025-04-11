#pragma once
#include <vector>

namespace who {
template <typename T, bool AllowExist = false>
class WhoContainer {
public:
    virtual ~WhoContainer() {}
    bool add_element(T element)
    {
        if constexpr (std::is_pointer_v<T>) {
            if (!element) {
                ESP_LOGE("WhoContainer", "element is nullptr.");
                return false;
            }
        }
        auto it = std::find(m_elements.begin(), m_elements.end(), element);
        if constexpr (AllowExist) {
            if (it != m_elements.end()) {
                return true;
            }
        } else {
            if (it != m_elements.end()) {
                ESP_LOGE("WhoContainer", "Failed to add element, element already exists.");
                return false;
            }
        }
        m_elements.push_back(element);
        return true;
    }

protected:
    std::vector<T> m_elements;
};
} // namespace who
