#include <unity.h>

#include "rvc_packet.h"

namespace {
// Builds a valid RVC frame with the given angles (in 0.01 deg units).
void makeFrame(uint8_t *frame, int16_t yaw, int16_t pitch, int16_t roll) {
    for (int i = 0; i < RVC_PACKET_SIZE; ++i) frame[i] = 0;
    frame[0] = 0xAA;
    frame[1] = 0xAA;
    frame[2] = 7;  // packet index
    const int16_t values[3] = {yaw, pitch, roll};
    for (int i = 0; i < 3; ++i) {
        frame[3 + 2 * i] = static_cast<uint8_t>(values[i] & 0xFF);
        frame[4 + 2 * i] = static_cast<uint8_t>((values[i] >> 8) & 0xFF);
    }
    uint8_t sum = 0;
    for (int i = 2; i < 18; ++i) sum += frame[i];
    frame[18] = sum;
}

RvcResult feed(RvcParser &parser, const uint8_t *bytes, int count, RvcAngles &out) {
    RvcResult last = RvcResult::NeedMore;
    for (int i = 0; i < count; ++i) {
        const RvcResult r = parser.push(bytes[i], out);
        if (r != RvcResult::NeedMore) last = r;
    }
    return last;
}
}

void setUp() {}
void tearDown() {}

void test_checksum_accepts_valid_frame() {
    uint8_t frame[RVC_PACKET_SIZE];
    makeFrame(frame, 1234, -567, 89);
    TEST_ASSERT_TRUE(rvcChecksumValid(frame));
}

void test_checksum_rejects_corrupted_byte() {
    uint8_t frame[RVC_PACKET_SIZE];
    makeFrame(frame, 1234, -567, 89);
    frame[5] ^= 0x01;
    TEST_ASSERT_FALSE(rvcChecksumValid(frame));
}

void test_decode_signed_little_endian_angles() {
    uint8_t frame[RVC_PACKET_SIZE];
    makeFrame(frame, 17999, -9000, -1);
    const RvcAngles a = rvcDecodeAngles(frame);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 179.99f, a.yaw);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, -90.00f, a.pitch);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, -0.01f, a.roll);
}

void test_parser_returns_packet_after_19_bytes() {
    uint8_t frame[RVC_PACKET_SIZE];
    makeFrame(frame, 4500, 0, 0);
    RvcParser parser;
    RvcAngles out = {0, 0, 0};
    for (int i = 0; i < RVC_PACKET_SIZE - 1; ++i)
        TEST_ASSERT_TRUE(parser.push(frame[i], out) == RvcResult::NeedMore);
    TEST_ASSERT_TRUE(parser.push(frame[RVC_PACKET_SIZE - 1], out) == RvcResult::Packet);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 45.0f, out.yaw);
}

void test_parser_resyncs_after_garbage() {
    uint8_t stream[3 + RVC_PACKET_SIZE] = {0x12, 0xAA, 0x34};  // lone header byte, then junk
    makeFrame(&stream[3], 100, 200, 300);
    RvcParser parser;
    RvcAngles out = {0, 0, 0};
    TEST_ASSERT_TRUE(feed(parser, stream, sizeof(stream), out) == RvcResult::Packet);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, 3.0f, out.roll);
}

void test_parser_reports_bad_checksum_and_recovers() {
    uint8_t bad[RVC_PACKET_SIZE];
    makeFrame(bad, 100, 100, 100);
    bad[18] ^= 0xFF;
    uint8_t good[RVC_PACKET_SIZE];
    makeFrame(good, -100, 0, 0);
    RvcParser parser;
    RvcAngles out = {0, 0, 0};
    TEST_ASSERT_TRUE(feed(parser, bad, RVC_PACKET_SIZE, out) == RvcResult::BadChecksum);
    TEST_ASSERT_TRUE(feed(parser, good, RVC_PACKET_SIZE, out) == RvcResult::Packet);
    TEST_ASSERT_FLOAT_WITHIN(1e-3f, -1.0f, out.yaw);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_checksum_accepts_valid_frame);
    RUN_TEST(test_checksum_rejects_corrupted_byte);
    RUN_TEST(test_decode_signed_little_endian_angles);
    RUN_TEST(test_parser_returns_packet_after_19_bytes);
    RUN_TEST(test_parser_resyncs_after_garbage);
    RUN_TEST(test_parser_reports_bad_checksum_and_recovers);
    return UNITY_END();
}
