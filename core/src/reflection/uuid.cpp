#include "reflection/uuid.hpp"

#include <iomanip>
#include <random>
#include <sstream>

#include "debug/exception.hpp"
#include "platform/platform.hpp"

namespace iodine::core {
    static inline i32 hexToValue(char c) {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        THROW_CORE_EXCEPTION(Exception::Type::InvalidArgument, "Invalid hex character");
    }

    UUID::UUID() {
        u64 ns = Platform::getInstance().timeNS();
        u64 ts = static_cast<u64>(ns) / 100;

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
            u64 s[2];
            XorShift128(u64 seed) {
                s[0] = seed;
                s[1] = Platform::getInstance().randomU64();
            }
            u64 next() {
                u64 s1 = s[0];
                const u64 s0 = s[1];
                s[0] = s0;
                s1 ^= s1 << 23;
                s[1] = s1 ^ s0 ^ (s1 >> 17) ^ (s0 >> 26);
                return s[1] + s0;
            }
        };

        static thread_local XorShift128 rng(Platform::getInstance().randomU64());
        u16 random_val = static_cast<u16>(rng.next() & 0xFFFF);
        u16 clock_seq = (random_val & 0x3FFF) | 0x8000;

        u64 node = rng.next();
        node &= 0x0000FFFFFFFFFFFFULL;
        node |= 0x010000000000ULL;

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