#include "who_app.hpp"
#include "who_yield2idle.hpp"

namespace who {
namespace app {
WhoApp::~WhoApp()
{
    stop();
    m_task_group.destroy();
}

bool WhoApp::pause()
{
    m_task_group.pause();
    WhoYield2Idle::get_instance()->pause();
    return true;
}

bool WhoApp::resume()
{
    WhoYield2Idle::get_instance()->resume();
    m_task_group.resume();
    return true;
}

bool WhoApp::stop()
{
    m_task_group.stop();
    WhoYield2Idle::get_instance()->stop();
    return true;
}
} // namespace app
} // namespace who
