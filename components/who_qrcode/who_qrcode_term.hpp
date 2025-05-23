#pragma once
#include "who_qrcode_base.hpp"
#include <list>
#include <string>

namespace who {
namespace qrcode {
class WhoQRCodeTerm : public WhoQRCodeBase {
public:
    using WhoQRCodeBase::WhoQRCodeBase;

private:
    void on_new_qrcode_result(const char *result) override;
};
} // namespace qrcode
} // namespace who
