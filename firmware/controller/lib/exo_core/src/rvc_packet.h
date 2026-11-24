#pragma once

#include <stdint.h>

// Parser for BNO085 UART-RVC frames.
//
// Frame layout (19 bytes): 0xAA 0xAA, index, yaw, pitch, roll, x/y/z accel
// (int16 little-endian, angles in 0.01 deg), 3 reserved bytes, checksum.
// The checksum is the 8-bit sum of bytes 2 through 17.

constexpr uint8_t RVC_PACKET_SIZE = 19;
constexpr uint8_t RVC_HEADER = 0xAA;

struct RvcAngles {
    float yaw;
    float pitch;
    float roll;
};

bool rvcChecksumValid(const uint8_t *packet);
RvcAngles rvcDecodeAngles(const uint8_t *packet);

enum class RvcResult { NeedMore, Packet, BadChecksum };

// Feeds one byte at a time from the serial port and resyncs on the header.
class RvcParser {
public:
    RvcResult push(uint8_t byte, RvcAngles &out);

private:
    uint8_t packet_[RVC_PACKET_SIZE] = {};
    uint8_t index_ = 0;
};
