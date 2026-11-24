#include "rvc_packet.h"

namespace {
int16_t readInt16Le(const uint8_t *bytes) {
    return static_cast<int16_t>(static_cast<uint16_t>(bytes[0]) |
                                (static_cast<uint16_t>(bytes[1]) << 8));
}
}

bool rvcChecksumValid(const uint8_t *packet) {
    uint8_t checksum = 0;
    for (uint8_t index = 2; index < 18; ++index) checksum += packet[index];
    return checksum == packet[18];
}

RvcAngles rvcDecodeAngles(const uint8_t *packet) {
    return {readInt16Le(&packet[3]) * 0.01f,
            readInt16Le(&packet[5]) * 0.01f,
            readInt16Le(&packet[7]) * 0.01f};
}

RvcResult RvcParser::push(uint8_t byte, RvcAngles &out) {
    if (index_ == 0) {
        if (byte == RVC_HEADER) packet_[index_++] = byte;
        return RvcResult::NeedMore;
    }
    if (index_ == 1) {
        if (byte == RVC_HEADER) packet_[index_++] = byte;
        else index_ = 0;
        return RvcResult::NeedMore;
    }
    packet_[index_++] = byte;
    if (index_ < RVC_PACKET_SIZE) return RvcResult::NeedMore;
    index_ = 0;
    if (!rvcChecksumValid(packet_)) return RvcResult::BadChecksum;
    out = rvcDecodeAngles(packet_);
    return RvcResult::Packet;
}
