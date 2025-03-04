#include "d2/d2_protocol.h"
#include <deque>
#include <array>
#include <string>
#include <algorithm>

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>

using namespace Volvo;

char hexChar(uint8_t nibble) {
    switch (nibble) {
        case 0x0: return '0';
        case 0x1: return '1';
        case 0x2: return '2';
        case 0x3: return '3';
        case 0x4: return '4';
        case 0x5: return '5';
        case 0x6: return '6';
        case 0x7: return '7';
        case 0x8: return '8';
        case 0x9: return '9';
        case 0xA: return 'A';
        case 0xB: return 'B';
        case 0xC: return 'C';
        case 0xD: return 'D';
        case 0xE: return 'E';
        case 0xF: return 'F';
        default: return (char) 0x0;
    }
}

std::array<char, 2> byteToChars(uint8_t byte) {
    uint8_t lowByte = byte & 0xF;
    uint8_t highByte = (byte >> 4) & 0xF;

    return {hexChar(highByte), hexChar(lowByte)};
}

std::string* dequeToString(std::deque<uint8_t> *buf) {
    std::string test;

    std::deque<uint8_t> bufcopy(buf->size());
    copy(buf->begin(), buf->end(), bufcopy.begin());

    for (uint i = 0; i < bufcopy.size(); i++) {
        std::array<char, 2> chars = byteToChars(bufcopy.front());
        test += "0x";
        test += chars[0];
        test += chars[1];
        test += ", ";
        bufcopy.pop_front();
    }

    std::string *testptr = new std::string(test);

    return testptr;
}

TEST_SUITE("D2 Frame Header Parser") {

    TEST_CASE("Valid Single-Frame Header") {
        std::deque<uint8_t> test_frame = { 0xCE, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };

        D2_Protocol::FrameHeader header = D2_Protocol::parseFrameHeader(&test_frame.front());

        CHECK(header.type == D2_Protocol::FrameHeader::Type::SINGLE);
        CHECK(header.numSigBytes == 6);
        CHECK(header.sequenceNumber == 0);
    }

    TEST_CASE("Valid Start-Frame Header") {
        std::deque<uint8_t> test_frame = { 0x8F, 0xC0, 0xFF, 0xEE, 0x01, 0x23, 0x45, 0x67 };

        D2_Protocol::FrameHeader header = D2_Protocol::parseFrameHeader(&test_frame.front());

        CHECK(header.type == D2_Protocol::FrameHeader::Type::EXT_FIRST);
        CHECK(header.numSigBytes == 0);
        CHECK(header.sequenceNumber == 0);
    }

    TEST_CASE("Valid Intermediate-Frame Header") {
        std::deque<uint8_t> test_frame = { 0x0B, 0xC0, 0xFF, 0xEE, 0x00, 0x00, 0x00, 0x00 };

        D2_Protocol::FrameHeader header = D2_Protocol::parseFrameHeader(&test_frame.front());

        CHECK(header.type == D2_Protocol::FrameHeader::Type::EXT_MIDDLE);
        CHECK(header.numSigBytes == 0);
        CHECK(header.sequenceNumber == 3);
    }

    TEST_CASE("Valid End-Frame Header") {
        std::deque<uint8_t> test_frame = { 0x4B, 0xC0, 0xFF, 0xEE, 0x00, 0x00, 0x00, 0x00 };

        D2_Protocol::FrameHeader header = D2_Protocol::parseFrameHeader(&test_frame.front());

        CHECK(header.type == D2_Protocol::FrameHeader::Type::EXT_LAST);
        CHECK(header.numSigBytes == 3);
        CHECK(header.sequenceNumber == 0);
    }

}

TEST_SUITE("D2 Frame Decoder") {
    TEST_CASE("Valid Single-Frame Message") {
        std::deque<uint8_t> frame = { 0xCF, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };
        std::deque<uint8_t> correctMessage = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77 };

        std::deque<uint8_t> *message = Volvo::D2_Protocol::unpackMessage(&frame);

        bool messageDataCorrect = *message == correctMessage;

        INFO("Expected Message: ", *dequeToString(&correctMessage));
        INFO("Decoded Message: ", *dequeToString(message));

        CHECK(message->size() == 7);
        CHECK(messageDataCorrect);
    }

    TEST_CASE("Valid Multi-Frame Message") {
        std::deque<uint8_t> frame = {
            0x8F, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x08, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
            0x09, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
            0x4B, 0x66, 0x77, 0x88, 0x00, 0x00, 0x00, 0x00,
        };

        std::deque<uint8_t> *message = Volvo::D2_Protocol::unpackMessage(&frame);

        std::deque<uint8_t> correctMessage = {
            0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
            0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
            0x66, 0x77, 0x88
        };

        bool messageDataCorrect = *message == correctMessage;

        INFO("Expected Message: ", *dequeToString(&correctMessage));
        INFO("Decoded Message: ", *dequeToString(message));

        CHECK(message->size() == 24);
        CHECK(messageDataCorrect);
    }

}

TEST_SUITE("D2 Frame Encoder") {

    TEST_CASE("Full Single-Frame Message") {
        std::deque<uint8_t> message = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };
        std::deque<uint8_t> correctFrame = { 0xCF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };

        std::deque<uint8_t> *frames = Volvo::D2_Protocol::messageToFrames(&message);

        bool arraysAreEqual = (*frames == correctFrame);

        INFO("Expected Frame: ", *dequeToString(&correctFrame));
        INFO("Generated Frame: ", *dequeToString(frames));

        CHECK(frames->size() == correctFrame.size());
        CHECK(arraysAreEqual);
    }

    TEST_CASE("Partial Single-Frame Message") {
        std::deque<uint8_t> message = { 0x11, 0x22, 0x33, 0x44 };
        std::deque<uint8_t> correctFrame = { 0xCC, 0x11, 0x22, 0x33, 0x44, 0x00, 0x00, 0x00 };

        std::deque<uint8_t> *frames = D2_Protocol::messageToFrames(&message);

        bool arraysAreEqual = *frames == correctFrame;

        INFO("Expected Frame: ", *dequeToString(&correctFrame));
        INFO("Generated Frame: ", *dequeToString(frames));

        CHECK(frames->size() == correctFrame.size());
        CHECK(arraysAreEqual);
    }

    TEST_CASE("Multi-Frame Message with Partial End Frame") {
        std::deque<uint8_t> message = {
            0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
            0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
            0x66, 0x77, 0x88
        };
        std::deque<uint8_t> *frames = Volvo::D2_Protocol::messageToFrames(&message);

        std::deque<uint8_t> correctFrames = {
            0x8F, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x08, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
            0x09, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
            0x4B, 0x66, 0x77, 0x88, 0x00, 0x00, 0x00, 0x00
        };

        bool arraysAreEqual = (*frames == correctFrames);

        INFO("Expected Frame: ", *dequeToString(&correctFrames));
        INFO("Generated Frame: ", *dequeToString(frames));

        CHECK(frames->size() == correctFrames.size());
        CHECK(arraysAreEqual);
    }

    TEST_CASE("Multi-Frame Message with Full End Frame") {
        std::deque<uint8_t> message = {
            0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
            0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
            0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC
        };
        std::deque<uint8_t> *frames = Volvo::D2_Protocol::messageToFrames(&message);

        std::deque<uint8_t> correctFrames = {
            0x8F, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
            0x08, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
            0x09, 0xFF, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
            0x4F, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC
        };

        bool arraysAreEqual = (*frames == correctFrames);

        INFO("Expected Frame: ", *dequeToString(&correctFrames));
        INFO("Generated Frame: ", *dequeToString(frames));

        CHECK(frames->size() == correctFrames.size());
        CHECK(arraysAreEqual);
    }
}





int main(int argc, char **argv)
{
    doctest::Context context;

    // BEGIN:: PLATFORMIO REQUIRED OPTIONS
    context.setOption("success", true);
    context.setOption("no-exitcode", true);
    // END:: PLATFORMIO REQUIRED OPTIONS

    // YOUR CUSTOM DOCTEST OPTIONS

    context.applyCommandLine(argc, argv);
    return context.run();
}
