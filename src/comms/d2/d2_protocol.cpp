#include "comms/d2/d2_protocol.h"
#include "mbed_assert.h"
#include "mbed_error.h"
#include <cstdint>
#include <cstring>

D2_Protocol::D2_Protocol() {
    // initialize can bus and register a message filter for D2 Responses
    // Initialize repeating keepalive event
}

void D2_Protocol::startSession() {}

void D2_Protocol::endSession() {}

uint32_t D2_Protocol::getNumFrames(uint32_t numBytes) {
    // a zero length message is not part of the known D2 spec
    if (numBytes == 0) {
        // TODO: throw an error
    }

    // each d2 CAN frame can hold 7 bytes of message data.
    uint32_t numFrames = numBytes / 7;

    // if we have remaining bytes after this, then we need another message.
    uint32_t remainder = numBytes % 7;
    if (remainder > 0) {
        numFrames++;
    }

    return numFrames;
}

uint32_t D2_Protocol::messageToFrames(uint8_t *data, uint8_t *dest, uint32_t size) {
    // Validate inputs
    if (
        data == nullptr ||
        dest == nullptr ||
        size < 1
    ) {
        // error, invalid pointer or no data
    }

    // Temporary variables for frame generation
    FrameHeader header;
    uint8_t currentFrame[8] = {0};

    // calculate the number of frames and zero out the memory
    uint32_t numFrames = getNumFrames(size);
    memset(dest, 0, sizeof(uint8_t[8]) * numFrames);

    if (numFrames == 1) { // Message can fit into 1 can frame
        header.type = FrameHeader::Type::SINGLE;
        header.numSigBytes = size;
        currentFrame[0] = buildFrameHeader(&header);
        memcpy(&currentFrame[1], &data[0], size * sizeof(uint8_t));
        memcpy(&dest[0], &currentFrame, sizeof(currentFrame));
    } else { // We need to generate a sequence of multiple frames
        uint8_t sequenceNumber = 0; // Used for keeping track of intermediate frames

        for (uint32_t frame = 0; frame < numFrames; frame++) {
            // Reset our temporary variables
            header = FrameHeader();
            memset(currentFrame, 0, sizeof(currentFrame));

            // Determine the starting indicies of the data for this frame
            uint32_t dataStartIndex = 7 * frame;
            uint32_t destStartIndex = 8 * frame;

            if (frame == 0) { // Special case for the first frame
                header.type = FrameHeader::Type::EXT_FIRST;
                currentFrame[0] = buildFrameHeader(&header);
                memcpy(&currentFrame[1], &data[0], 7 * sizeof(uint8_t));
            } else if (frame == (numFrames - 1)) { // Special case for the last frame
                header.type = FrameHeader::Type::EXT_LAST;
                uint8_t numSigBytes = (dataStartIndex + 6) - size;
                header.numSigBytes = numSigBytes;
                currentFrame[0] = buildFrameHeader(&header);
                memcpy(&currentFrame[1], &data[dataStartIndex], numSigBytes * sizeof(uint8_t));
            } else { // All other cases, generate an intermediate frame
                header.type = FrameHeader::Type::EXT_MIDDLE;
                header.sequenceNumber = sequenceNumber;
                currentFrame[0] = buildFrameHeader(&header);
                memcpy(&currentFrame[1], &data[dataStartIndex], 7 * sizeof(uint8_t));

                // update sequence number for next iteration
                sequenceNumber = getNextSeqNumber(sequenceNumber);
            }

            // at end of each loop iteration, copy the current frame data to the dest.
            memcpy(&dest[destStartIndex], &currentFrame, sizeof(currentFrame));
        }
    }

    // return the number of frames generated
    return numFrames;
}

uint8_t* D2_Protocol::unpackMessage(uint8_t *data, uint32_t size) {
    // make sure the size of our input array is valid
    if ((size % 8) != 0) {
        // error, we do not have proper frames
    }

    uint32_t numFrames = size / 8;

    // Temporary variables for frame decoding
    FrameHeader header;
    uint8_t currentFrame[8] = {0};

    header = parseFrameHeader(&data[(numFrames -1) * 8]);
    uint8_t lastFrameSigBytes = header.numSigBytes;

    uint32_t messageTotalBytes = (8 * (numFrames - 1)) + lastFrameSigBytes;

    uint8_t* message = new uint8_t[messageTotalBytes];

    uint8_t lastSequenceNumber = 0;

    for (uint32_t frame = 0; frame < numFrames; frame++) {
        header = FrameHeader();
        memset(currentFrame, 0, sizeof(currentFrame));

        memcpy(&currentFrame, &data[frame * 8], 8 * sizeof(uint8_t));
        header = parseFrameHeader(currentFrame);

        // if not first or last frame, check sequence number
        if (!((frame == 0) || (frame == (numFrames - 1)))) {
            uint8_t nextSequenceNumber = getNextSeqNumber(lastSequenceNumber);
            if (header.sequenceNumber != nextSequenceNumber) {
                // bad things have happened
            }
            lastSequenceNumber = nextSequenceNumber;
        }
        // if last frame, only read significant bytes
        if (frame == (numFrames - 1)) {
            memcpy(&message[frame * 7], &currentFrame[1], lastFrameSigBytes * sizeof(uint8_t));
        } else { // otherwise read all bytes
            memcpy(&message[frame * 7], &currentFrame[1], 7 * sizeof(uint8_t));
        }
    }

    return message;
}

D2_Protocol::FrameHeader D2_Protocol::parseFrameHeader(uint8_t *firstByte) {
    // single frame: mask 0xF8, filter 0xC8, last3 num sig bytes
    // multi first: mask 0xFF, filter 0x8F, last3 always 0b111
    // multi middle: mask 0xF8, filter 0x08, last3 seq number
    // multi last: mask 0xF8, filter 0x48, last3 num sig bytes

    // this pointer should be valid
    MBED_ASSERT(firstByte);
    if (firstByte == nullptr) {
        MBED_ERROR( MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "Pointer to first byte is null");
    }

    D2_Protocol::FrameHeader header;

    if ((*firstByte & 0xF8) == 0xC8) { // Single-frame message
        header.type = FrameHeader::Type::SINGLE;
        header.numSigBytes = (*firstByte & 0b111);
    } else if ((*firstByte & 0xFF) == 0x8F) { // Multi-frame, first
        header.type = FrameHeader::Type::EXT_FIRST;
    } else if ((*firstByte & 0xF8) == 0x08) { // Multi-frame, middle
        header.type = FrameHeader::Type::EXT_MIDDLE;
        header.sequenceNumber = (*firstByte & 0b111);
    } else if ((*firstByte & 0xF8) == 0x48) { // Multi-frame, last
        header.type = FrameHeader::Type::EXT_LAST;
        header.numSigBytes = (*firstByte & 0b111);
    } else {
        // Throw error if we don't match any conditions
        MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "Header byte did not match expected format");
    }

    return header;
}

uint8_t D2_Protocol::buildFrameHeader(D2_Protocol::FrameHeader *header) {
    // single frame: 0xC8 + num sig bytes (up to 7)
    // multi first: 0x8F
    // multi middle: 0x08 + sequence number (up to 7, rolls over to 0)
    // multi last: 0x48 + num sig bytes (up to 7)

    // Make sure our pointer is valid
    if (header == nullptr) {
        MBED_ERROR( MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "Header pointer is null");
    }

    uint8_t headerByte = 0;

    uint8_t numSigBytes = header->numSigBytes;
    uint8_t sequenceNum = header->sequenceNumber;

    switch (header->type) {
        case FrameHeader::Type::SINGLE: {
            // number of sig bytes should always be [1,7]
            if (numSigBytes < 1 || numSigBytes > 7) {
                MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "Invalid number of significant bytes");
            }
            headerByte = 0xC8 + numSigBytes;
            break;
        };
        case FrameHeader::Type::EXT_FIRST: {
            headerByte = 0x8F;
            break;
        };
        case FrameHeader::Type::EXT_MIDDLE: {
            // sequence number should always be [0,7] (or maybe 1,7??)
            MBED_ASSERT(sequenceNum <= 7);
            if (sequenceNum > 7) {
                MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "Invalid sequence number");
            }
            headerByte = 0x08 + sequenceNum;
            break;
        };
        case FrameHeader::Type::EXT_LAST: {
            // number of sig bytes should always be [1,7]
            if (numSigBytes < 1 || numSigBytes > 7) {
                MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "Invalid number of significant bytes");
            }
            headerByte = 0x48 + numSigBytes;
            break;
        };
        default: {
            MBED_ERROR(MBED_MAKE_ERROR(MBED_MODULE_APPLICATION, MBED_ERROR_CODE_INVALID_ARGUMENT), "Switch on header type fell through");
        };
    }

    return headerByte;
}

uint8_t D2_Protocol::getNextSeqNumber(uint8_t currentNumber) {
    if (currentNumber == 0b111) {
        return 0;
    } else {
        return currentNumber + 1;
    }
}

void D2_Protocol::_keepAliveSend() {
}
