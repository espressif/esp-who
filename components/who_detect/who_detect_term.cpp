#include "who_detect_term.hpp"
#include "who_detect_utils.hpp"

namespace who {
namespace detect {
void WhoDetectTerm::on_new_detect_result(const result_t &result)
{
    print_detect_results(result.det_res);
}
} // namespace detect
} // namespace who
