#include "crypto/md5.hpp"

#include <iomanip>
#include <sstream>

namespace iodine::core {
    struct MD5::Context {
        u32 state[4];   // MD5 state (ABCD)
        u32 count[2];   // Number of bits, modulo 2^64 (low-order word first)
        u8 buffer[64];  // Input buffer
    };

    static inline u32 F(u32 x, u32 y, u32 z) { return (x & y) | (~x & z); }
    static inline u32 G(u32 x, u32 y, u32 z) { return (x & z) | (y & ~z); }
    static inline u32 H(u32 x, u32 y, u32 z) { return x ^ y ^ z; }
    static inline u32 I(u32 x, u32 y, u32 z) { return y ^ (x | ~z); }

    static inline u32 rotate_left(u32 x, i32 n) { return (x << n) | (x >> (32 - n)); }

    static inline void FF(u32& a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac) { a = rotate_left(a + F(b, c, d) + x + ac, s) + b; }
    static inline void GG(u32& a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac) { a = rotate_left(a + G(b, c, d) + x + ac, s) + b; }
    static inline void HH(u32& a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac) { a = rotate_left(a + H(b, c, d) + x + ac, s) + b; }
    static inline void II(u32& a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac) { a = rotate_left(a + I(b, c, d) + x + ac, s) + b; }

    static constexpr u8 padding[64] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0,    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    /**
     * @brief Encodes input (u32 array) into output (u8 array).
     * @param output The output buffer.
     * @param input The input data.
     * @param len The length of the input data.
     */
    void encode(u8* output, const u32* input, u32 len) {
        for (u32 i = 0, j = 0; j < len; i++, j += 4) {
            u32 temp = input[i];
            if (Platform::getInstance().isBigEndian()) {
                Platform::getInstance().swapEndian(&temp, sizeof(u32));
            }
            output[j] = static_cast<u8>(temp & 0xff);
            output[j + 1] = static_cast<u8>((temp >> 8) & 0xff);
            output[j + 2] = static_cast<u8>((temp >> 16) & 0xff);
            output[j + 3] = static_cast<u8>((temp >> 24) & 0xff);
        }
    }

    /**
     * @brief Decodes input (u8 array) into output (u32 array).
     * @param output The output buffer.
     * @param input The input data.
     * @param len The length of the input data.
     */
    void decode(u32* output, const u8* input, u32 len) {
        for (u32 i = 0, j = 0; j < len; i++, j += 4) {
            output[i] = (static_cast<u32>(input[j])) | (static_cast<u32>(input[j + 1]) << 8) | (static_cast<u32>(input[j + 2]) << 16) | (static_cast<u32>(input[j + 3]) << 24);
            if (Platform::getInstance().isBigEndian()) {
                Platform::getInstance().swapEndian(&output[i], sizeof(u32));
            }
        }
    }

    /**
     * @brief Process a 64-byte block of data.`
     * @param context The MD5 context.
     */
    void transform(MD5::Context& context) {
        u32 a = context.state[0], b = context.state[1], c = context.state[2], d = context.state[3];
        u32 x[16];
        decode(x, context.buffer, 64);

        // Round 1
        FF(a, b, c, d, x[0], 7, 0xd76aa478);
        FF(d, a, b, c, x[1], 12, 0xe8c7b756);
        FF(c, d, a, b, x[2], 17, 0x242070db);
        FF(b, c, d, a, x[3], 22, 0xc1bdceee);
        FF(a, b, c, d, x[4], 7, 0xf57c0faf);
        FF(d, a, b, c, x[5], 12, 0x4787c62a);
        FF(c, d, a, b, x[6], 17, 0xa8304613);
        FF(b, c, d, a, x[7], 22, 0xfd469501);
        FF(a, b, c, d, x[8], 7, 0x698098d8);
        FF(d, a, b, c, x[9], 12, 0x8b44f7af);
        FF(c, d, a, b, x[10], 17, 0xffff5bb1);
        FF(b, c, d, a, x[11], 22, 0x895cd7be);
        FF(a, b, c, d, x[12], 7, 0x6b901122);
        FF(d, a, b, c, x[13], 12, 0xfd987193);
        FF(c, d, a, b, x[14], 17, 0xa679438e);
        FF(b, c, d, a, x[15], 22, 0x49b40821);

        // Round 2
        GG(a, b, c, d, x[1], 5, 0xf61e2562);
        GG(d, a, b, c, x[6], 9, 0xc040b340);
        GG(c, d, a, b, x[11], 14, 0x265e5a51);
        GG(b, c, d, a, x[0], 20, 0xe9b6c7aa);
        GG(a, b, c, d, x[5], 5, 0xd62f105d);
        GG(d, a, b, c, x[10], 9, 0x02441453);
        GG(c, d, a, b, x[15], 14, 0xd8a1e681);
        GG(b, c, d, a, x[4], 20, 0xe7d3fbc8);
        GG(a, b, c, d, x[9], 5, 0x21e1cde6);
        GG(d, a, b, c, x[14], 9, 0xc33707d6);
        GG(c, d, a, b, x[3], 14, 0xf4d50d87);
        GG(b, c, d, a, x[8], 20, 0x455a14ed);
        GG(a, b, c, d, x[13], 5, 0xa9e3e905);
        GG(d, a, b, c, x[2], 9, 0xfcefa3f8);
        GG(c, d, a, b, x[7], 14, 0x676f02d9);
        GG(b, c, d, a, x[12], 20, 0x8d2a4c8a);

        // Round 3
        HH(a, b, c, d, x[5], 4, 0xfffa3942);
        HH(d, a, b, c, x[8], 11, 0x8771f681);
        HH(c, d, a, b, x[11], 16, 0x6d9d6122);
        HH(b, c, d, a, x[14], 23, 0xfde5380c);
        HH(a, b, c, d, x[1], 4, 0xa4beea44);
        HH(d, a, b, c, x[4], 11, 0x4bdecfa9);
        HH(c, d, a, b, x[7], 16, 0xf6bb4b60);
        HH(b, c, d, a, x[10], 23, 0xbebfbc70);
        HH(a, b, c, d, x[13], 4, 0x289b7ec6);
        HH(d, a, b, c, x[0], 11, 0xeaa127fa);
        HH(c, d, a, b, x[3], 16, 0xd4ef3085);
        HH(b, c, d, a, x[6], 23, 0x04881d05);
        HH(a, b, c, d, x[9], 4, 0xd9d4d039);
        HH(d, a, b, c, x[12], 11, 0xe6db99e5);
        HH(c, d, a, b, x[15], 16, 0x1fa27cf8);
        HH(b, c, d, a, x[2], 23, 0xc4ac5665);

        // Round 4
        II(a, b, c, d, x[0], 6, 0xf4292244);
        II(d, a, b, c, x[7], 10, 0x432aff97);
        II(c, d, a, b, x[14], 15, 0xab9423a7);
        II(b, c, d, a, x[5], 21, 0xfc93a039);
        II(a, b, c, d, x[12], 6, 0x655b59c3);
        II(d, a, b, c, x[3], 10, 0x8f0ccc92);
        II(c, d, a, b, x[10], 15, 0xffeff47d);
        II(b, c, d, a, x[1], 21, 0x85845dd1);
        II(a, b, c, d, x[8], 6, 0x6fa87e4f);
        II(d, a, b, c, x[15], 10, 0xfe2ce6e0);
        II(c, d, a, b, x[6], 15, 0xa3014314);
        II(b, c, d, a, x[13], 21, 0x4e0811a1);
        II(a, b, c, d, x[4], 6, 0xf7537e82);
        II(d, a, b, c, x[11], 10, 0xbd3af235);
        II(c, d, a, b, x[2], 15, 0x2ad7d2bb);
        II(b, c, d, a, x[9], 21, 0xeb86d391);

        context.state[0] += a;
        context.state[1] += b;
        context.state[2] += c;
        context.state[3] += d;
    }

    /**
     * @brief Process input in blocks of 64 bits.
     * @param context The MD5 context.
     * @param input The input data.
     * @param length The length of the input data.
     */
    void update(MD5::Context& context, const u8* input, u32 length) {
        u32 index = (context.count[0] >> 3) & 0x3F;

        u32 addBits = length << 3;
        if ((context.count[0] += addBits) < addBits) context.count[1]++;
        context.count[1] += length >> 29;

        u32 partLen = 64 - index;
        u32 i = 0;
        if (length >= partLen) {
            std::memcpy(&context.buffer[index], input, partLen);
            transform(context);

            for (i = partLen; i + 63 < length; i += 64) transform(context);

            index = 0;
        }

        std::memcpy(&context.buffer[index], &input[i], length - i);
    }

    MD5::MD5(const std::string& input) : MD5::MD5(input.c_str(), input.length()) {}

    MD5::MD5(const char* input, u32 length) { hash(input, length); }

    MD5::operator std::string() const {
        std::ostringstream oss;
        oss << std::hex << std::setfill('0');
        for (int i = 0; i < 16; ++i) oss << std::setw(2) << static_cast<u32>(digest[i]);
        return oss.str();
    }

    void MD5::hash(const char* input, u32 length) {
        Context context;
        context.count[0] = context.count[1] = 0;
        context.state[0] = 0x67452301;  // Magic numbers
        context.state[1] = 0xefcdab89;
        context.state[2] = 0x98badcfe;
        context.state[3] = 0x10325476;

        update(context, reinterpret_cast<const u8*>(input), length);

        u8 bits[8];
        encode(bits, context.count, 8);

        // Pad out to 56 mod 64.
        u32 index = (context.count[0] >> 3) & 0x3F;
        u32 padLen = (index < 56) ? (56 - index) : (120 - index);
        update(context, padding, padLen);
        update(context, bits, 8);

        encode(digest, context.state, 16);
    }
}  // namespace iodine::core