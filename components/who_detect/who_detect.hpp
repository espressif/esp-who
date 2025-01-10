#pragma once
#include "cam.hpp"
#include "dl_detect_base.hpp"
#include <list>

namespace who {
namespace app {
class WhoDetect {
public:
    typedef struct {
        std::list<dl::detect::result_t> det_res;
        struct timeval timestamp;
    } result_t;

    WhoDetect(dl::detect::Detect *detect, who::cam::Cam *cam, const std::string &name, EventBits_t task_bit = 0);
    void run();
    void display(who::cam::cam_fb_t *fb);

private:
    static void task(void *args);
    SemaphoreHandle_t m_mutex;
    dl::detect::Detect *m_detect;
    who::cam::Cam *m_cam;
    std::queue<result_t> m_results;
    std::string m_name;
    EventBits_t m_task_bit;
};

} // namespace app
} // namespace who
