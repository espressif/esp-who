#pragma once
#include "cam_define.hpp"
#include <functional>
#include <map>
#include <string>

namespace who {
namespace app {

class DisplayFuncManager {
public:
    using DisplayFunc = std::function<void(who::cam::cam_fb_t *fb)>;

    static DisplayFuncManager *get_instance()
    {
        // This is thread safe for C++11, please refer to `Meyers' implementation of the Singleton pattern`
        static DisplayFuncManager instance;
        return &instance;
    }

    void register_display_func(const std::string &name, DisplayFunc func)
    {
        DisplayFuncManager::m_display_funcs[name] = func;
    }

    void display(who::cam::cam_fb_t *fb)
    {
        if (!m_display_funcs.empty()) {
            for (auto it = m_display_funcs.begin(); it != m_display_funcs.end(); ++it) {
                (it->second)(fb);
            }
        }
    }

    void print()
    {
        if (!m_display_funcs.empty()) {
            for (auto it = m_display_funcs.begin(); it != m_display_funcs.end(); ++it) {
                printf("%s", (*it).first.c_str());
            }
        } else {
            printf("Empty display funcs\n");
        }
    }

private:
    DisplayFuncManager() {}
    ~DisplayFuncManager() {}
    DisplayFuncManager(const DisplayFuncManager &) = delete;
    DisplayFuncManager &operator=(const DisplayFuncManager &) = delete;
    std::map<std::string, DisplayFunc> m_display_funcs;
};

} // namespace app
} // namespace who
