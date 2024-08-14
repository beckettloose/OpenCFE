#ifndef __D2_ECU_H
#define __D2_ECU_H

#include <cstdint>

class D2ECU {
public:
    void startSession();
    void endSession();
private:
    uint8_t d2_ecu_id = 0x00;
    uint32_t d2_response_id = 0x0;
};

#endif
