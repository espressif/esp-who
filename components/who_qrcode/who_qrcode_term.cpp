#include "who_qrcode_term.hpp"

static const char *TAG = "WhoQRCode";

namespace who {
namespace qrcode {
void WhoQRCodeTerm::on_new_qrcode_result(const char *result)
{
    ESP_LOGI(TAG, "%s", result);
}
} // namespace qrcode
} // namespace who
