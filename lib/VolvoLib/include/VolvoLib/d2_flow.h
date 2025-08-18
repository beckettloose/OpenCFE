#ifndef __D2_FLOW_H
#define __D2_FLOW_H

namespace Volvo {

/* Implements flow management for the D2 Protocol. This handles the process of
* sending a message and waiting for it's response. Additionally, messages are
* inspected to determine if the response was an error. */
class D2_Flow {
public:
    /* Represents the current state of a D2 Flow */
    // typedef struct FlowState {
    //     enum class Status { TRANSMIT, WAIT, RESPONSE_READY } status;
    //     bool parseError; // Parsing failed
    //     bool responseError; // Response was parsed but its content indicated that it was failed
    // } FlowState;
};

}

#endif // !__D2_FLOW_H
