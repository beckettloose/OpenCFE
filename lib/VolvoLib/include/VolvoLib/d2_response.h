#ifndef __D2_RESPONSE_H
#define __D2_RESPONSE_H

#include "d2_builder.h"
#include <deque>
#include <cstdint>

namespace Volvo {
class D2_Response {
public:
    typedef struct ResponseData {
        uint8_t ecuId;
        D2_Builder::OperationIdentifier operation = D2_Builder::Unknown;
        std::deque<uint8_t>* payload;
    } ResponseData;
};
}

#endif // !__D2_RESPONSE_H
