#include "reflection/uuid.hpp"

namespace iodine::core {
    UUID::operator std::string() const {
        char buffer[37];
        snprintf(buffer, sizeof(buffer), "%08x-%04x-%04x-%04x-%012llx", *(u32*)&bytes[0], *(u16*)&bytes[4], *(u16*)&bytes[6], *(u16*)&bytes[8], *(u64*)&bytes[10]);
        return std::string(buffer);
    }

    u8 UUID::getVersion() const { return (bytes[6] >> 4) & 0x0F; }
}