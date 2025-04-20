#include "reflection/uuid.hpp"

#include <iomanip>
#include <random>
#include <sstream>

#include "debug/exception.hpp"
#include "platform/platform.hpp"

namespace iodine::core {
    static inline int hexToValue(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        throw std::invalid_argument("Invalid hex digit in UUID string");  // TODO: Reflection exceptions
    }

    UUID::UUID() {
        u64 ns = Platform::getInstance().timeNS();
        u64 ts = static_cast<u64>(ns) / 100;
        // Add the UUID epoch offset (October 15, 1582)
        static const u64 uuid_epoch_offset = 0x01B21DD213814000ULL;
        ts += uuid_epoch_offset;

        // Ensure the timestamp is strictly increasing
        static thread_local u64 last_ts = 0;
        if (ts <= last_ts) ts = last_ts + 1;
        last_ts = ts;

        // Extract the UUID time fields from the timestamp
        u32 time_low = static_cast<u32>(ts & 0xFFFFFFFFULL);
        u16 time_mid = static_cast<u16>((ts >> 32) & 0xFFFFULL);
        u16 time_hi = static_cast<u16>((ts >> 48) & 0x0FFFULL);
        // Set the version to 1
        time_hi |= (1 << 12);

        // Define a simple XOR Shift PRNG for faster random number generation
        struct XorShift128 {
            uint64_t s[2];
            XorShift128(uint64_t seed) {
                s[0] = seed;
                s[1] = Platform::getInstance().randomU64();
            }
            uint64_t next() {
                uint64_t s1 = s[0];
                const uint64_t s0 = s[1];
                s[0] = s0;
                s1 ^= s1 << 23;
                s[1] = s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26);
                return s[1] + s0;
            }
        };
        // Use a thread-local instance of the XOR Shift PRNG seeded from randomU64()
        static thread_local XorShift128 rng(Platform::getInstance().randomU64());
        u16 random_val = static_cast<u16>(rng.next() & 0xFFFF);
        u16 clock_seq = (random_val & 0x3FFF) | 0x8000;

        // Generate the node value; mask to 48 bits and set the multicast bit as required
        u64 node = rng.next();
        node &= 0x0000FFFFFFFFFFFFULL;
        node |= 0x010000000000ULL;

        // Pack the fields into the UUID byte array
        bytes[0] = static_cast<u8>((time_low >> 24) & 0xFF);
        bytes[1] = static_cast<u8>((time_low >> 16) & 0xFF);
        bytes[2] = static_cast<u8>((time_low >> 8) & 0xFF);
        bytes[3] = static_cast<u8>(time_low & 0xFF);
        bytes[4] = static_cast<u8>((time_mid >> 8) & 0xFF);
        bytes[5] = static_cast<u8>(time_mid & 0xFF);
        bytes[6] = static_cast<u8>((time_hi >> 8) & 0xFF);
        bytes[7] = static_cast<u8>(time_hi & 0xFF);
        bytes[8] = static_cast<u8>((clock_seq >> 8) & 0xFF);
        bytes[9] = static_cast<u8>(clock_seq & 0xFF);
        bytes[10] = static_cast<u8>((node >> 40) & 0xFF);
        bytes[11] = static_cast<u8>((node >> 32) & 0xFF);
        bytes[12] = static_cast<u8>((node >> 24) & 0xFF);
        bytes[13] = static_cast<u8>((node >> 16) & 0xFF);
        bytes[14] = static_cast<u8>((node >> 8) & 0xFF);
        bytes[15] = static_cast<u8>(node & 0xFF);
    }

    UUID::UUID(const char* str) {
        std::string s(str);
        CORE_ASSERT_EXCEPTION(s.length() == 36, "UUID string must be 36 characters long");
        CORE_ASSERT_EXCEPTION(s[8] == '-' && s[13] == '-' && s[18] == '-' && s[23] == '-',
                              "UUID string must contain hyphens at positions 9, 14, 19, and 24");

        // Parse the hex digits while skipping hyphens.
        int byteIndex = 0;
        for (size_t i = 0; i < s.length();) {
            if (s[i] == '-') {
                ++i;
                continue;
            }
            // Ensure there is a pair available.
            CORE_ASSERT_EXCEPTION(i + 1 < s.length(), "Incomplete hex pair in UUID string");

            int high = hexToValue(s[i]);
            int low = hexToValue(s[i + 1]);
            bytes[byteIndex++] = static_cast<u8>((high << 4) | low);
            i += 2;
        }
        CORE_ASSERT_EXCEPTION(byteIndex == 16, "UUID string did not parse to 16 bytes");
    }

    UUID::UUID(const std::string& str) : UUID(str.c_str()) {}

    UUID::operator std::string() const {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; i++) {
            oss << std::setw(2) << static_cast<unsigned int>(bytes[i]);
            if (i == 3 || i == 5 || i == 7 || i == 9) oss << "-";
        }
        return oss.str();
    }

    std::string UUID::toString() const {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; i++) {
            oss << std::setw(2) << static_cast<unsigned int>(bytes[i]);
            if (i == 3 || i == 5 || i == 7 || i == 9) oss << "-";
        }
        return oss.str();
    }

    u8 UUID::getVersion() const { return bytes[6] >> 4; }
}  // namespace iodine::core