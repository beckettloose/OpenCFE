#include <VolvoLib/d2_message.h>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <deque>

namespace Volvo {

uint32_t D2_Message::numFrames(uint32_t numBytes) {
    // a zero length message is not part of the known D2 spec
    assert(numBytes > 0);

    // each d2 CAN frame can hold 7 bytes of message data.
    uint32_t numFrames = numBytes / 7;

    // if we have remaining bytes after this, then we need another message.
    uint32_t remainder = numBytes % 7;
    if (remainder > 0) {
        numFrames++;
    }

    return numFrames;
}

std::deque<uint8_t>* D2_Message::toFrames(std::deque<uint8_t> *data) {
    // Validate inputs
    assert(data != nullptr);

    // Temporary variables for frame generation
    FrameHeader header;
    std::deque<uint8_t> *frames = new std::deque<uint8_t>;

    // calculate the number of frames
    uint32_t totalFrames = numFrames(data->size());

    if (totalFrames == 1) { // Message can fit into 1 can frame
        header.type = FrameHeader::Type::SINGLE;
        header.numSigBytes = data->size();
        frames->push_back(buildHeader(&header));
        while (!data->empty()) {
            frames->push_back(data->front());
            data->pop_front();
        }
        // make sure we round out the end with zeros
        while ((frames->size() % 8) != 0) {
            frames->push_back(0x0);
        }
    } else { // We need to generate a sequence of multiple frames
        uint8_t sequenceNumber = 0; // Used for keeping track of intermediate frames

        for (uint32_t frame = 0; frame < totalFrames; frame++) {
            // Reset our temporary variables
            header = FrameHeader();

            if (frame == 0) { // Special case for the first frame
                header.type = FrameHeader::Type::EXT_FIRST;
                frames->push_back(buildHeader(&header));
                for (int i = 0; i < 7; i++) {
                    frames->push_back(data->front());
                    data->pop_front();
                }
            } else if (frame == (totalFrames - 1)) { // Special case for the last frame
                assert(data->size() <= 7);

                header.type = FrameHeader::Type::EXT_LAST;
                header.numSigBytes = data->size();
                frames->push_back(buildHeader(&header));
                while (!data->empty()) {
                    frames->push_back(data->front());
                    data->pop_front();
               }
                // make sure we round out the end with zeros
                while ((frames->size() % 8) != 0) {
                    frames->push_back(0x0);
                }
            } else { // All other cases, generate an intermediate frame
                header.type = FrameHeader::Type::EXT_MIDDLE;
                header.sequenceNumber = sequenceNumber;
                frames->push_back(buildHeader(&header));
                for (int i = 0; i < 7; i++) {
                    frames->push_back(data->front());
                    data->pop_front();
                }

                // update sequence number for next iteration
                sequenceNumber = nextSeqNumber(sequenceNumber);
            }
        }
    }

    return frames;
}

std::deque<uint8_t>* D2_Message::fromFrames(std::deque<uint8_t> *data) {
    // make sure the size of our input array is valid
    assert((data->size() % 8) == 0);

    uint32_t numFrames = data->size() / 8;

    // Temporary variables for frame decoding
    FrameHeader header;
    /*uint8_t currentFrame[8] = {0};*/

    std::deque<uint8_t>* message = new std::deque<uint8_t>();

    // NOTE: this might need to change based on the sequence number range
    uint8_t lastSequenceNumber = -1;

    for (uint32_t frame = 0; frame < numFrames; frame++) {
        header = FrameHeader();

        header = parseHeader(&data->front());
        data->pop_front();

        // if not first or last frame, check sequence number
        if (!((frame == 0) || (frame == (numFrames - 1)))) {
            uint8_t expectedSequenceNumber = nextSeqNumber(lastSequenceNumber);
            assert(header.sequenceNumber == expectedSequenceNumber);
            lastSequenceNumber = header.sequenceNumber;
        }
        // if last frame, only read significant bytes
        if (frame == (numFrames - 1)) {
            for (int i = 0; i < header.numSigBytes; i++) {
                message->push_back(data->front());
                data->pop_front();
            }
        } else { // otherwise read all bytes
            for (int i = 0; i < 7; i++) {
                message->push_back(data->front());
                data->pop_front();
            }
        }
    }

    return message;
}

D2_Message::FrameHeader D2_Message::parseHeader(uint8_t *firstByte) {
    // single frame: mask 0xF8, filter 0xC8, last3 num sig bytes
    // multi first: mask 0xFF, filter 0x8F, last3 always 0b111
    // multi middle: mask 0xF8, filter 0x08, last3 seq number
    // multi last: mask 0xF8, filter 0x48, last3 num sig bytes

    // this pointer should be valid
    assert(firstByte != nullptr);

    D2_Message::FrameHeader header;

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
        assert(false);
    }

    return header;
}

uint8_t D2_Message::buildHeader(D2_Message::FrameHeader *header) {
    // single frame: 0xC8 + num sig bytes (up to 7)
    // multi first: 0x8F
    // multi middle: 0x08 + sequence number (up to 7, rolls over to 0)
    // multi last: 0x48 + num sig bytes (up to 7)

    // Make sure our pointer is valid
    assert(header != nullptr);

    uint8_t headerByte = 0;

    uint8_t numSigBytes = header->numSigBytes;
    uint8_t sequenceNum = header->sequenceNumber;

    switch (header->type) {
        case FrameHeader::Type::SINGLE: {
            // number of sig bytes should always be [1,7]
            assert(numSigBytes >= 1 && numSigBytes <= 7);

            headerByte = 0xC8 + numSigBytes;
            break;
        };
        case FrameHeader::Type::EXT_FIRST: {
            headerByte = 0x8F;
            break;
        };
        case FrameHeader::Type::EXT_MIDDLE: {
            // sequence number should always be [0,7] (or maybe 1,7??)
            // TODO: figure out which sequence range is correct
            assert(sequenceNum >= 0 && sequenceNum <= 7);

            headerByte = 0x08 + sequenceNum;
            break;
        };
        case FrameHeader::Type::EXT_LAST: {
            // number of sig bytes should always be [1,7]
            assert(numSigBytes >= 1 && numSigBytes <= 7);

            headerByte = 0x48 + numSigBytes;
            break;
        };
        default: {
            assert(false);
        };
    }

    return headerByte;
}

uint8_t D2_Message::nextSeqNumber(uint8_t currentNumber) {
    if (currentNumber == 0b111) {
        return 0;
    } else {
        return currentNumber + 1;
    }
}

}
