#ifndef __D2_MESSAGE_H
#define __D2_MESSAGE_H

#include <cstdint>
#include <deque>

namespace Volvo {

/* Implements functions regarding D2 message processing (does not handle flow or
 * session management. */
class D2_Message {
public:
    /* Represents the data contained in the header of a D2 CAN frame */
    typedef struct FrameHeader {
        enum class Type { SINGLE, EXT_FIRST, EXT_MIDDLE, EXT_LAST, INVALID } type;
        uint8_t sequenceNumber;
        uint8_t numSigBytes;
        FrameHeader(): type(Type::INVALID), sequenceNumber(0), numSigBytes(0) {}
    } FrameHeader;

    /* Start a D2 session and begin sending the keepalive signal */
    // void startSession();

    /* End the D2 session and cancel the keepalive signal */
    // void endSession();

    /* Calculate the expected number of CAN frames for a D2 message */
    static uint32_t numFrames(uint32_t numBytes);

    /* Convert the message at *data to can frames and store it at *dest. Returns the total number of frames generated*/
    static std::deque<uint8_t>* toFrames(std::deque<uint8_t> *data);

    /* Try to unpack the D2 message. */
    static std::deque<uint8_t>* fromFrames(std::deque<uint8_t> *data);

    /* Parse the header frame of a D2 message. Takes a pointer to the first byte of the frame */
    static FrameHeader parseHeader(uint8_t *firstByte);

    /* Build the header frame of a D2 message. Takes a pointer to the FrameHeader struct to use */
    static uint8_t buildHeader(FrameHeader *header);

    /* Calculate the next sequence number based on the current one. */
    static uint8_t nextSeqNumber(uint8_t currentNumber);
private:
    D2_Message();
};

}

#endif
