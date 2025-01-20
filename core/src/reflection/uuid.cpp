#include "reflection/uuid.hpp"

#include "platform/platform.hpp"

namespace iodine::core {
    UUIDGenerator::UUIDGenerator() : key(Platform::getInstance().random_u64()), counter(0) {
        while (!key) {
            key = Platform::getInstance().random_u64();
        }
    }

    UUID UUIDGenerator::generate() {
        u64 val = __atomic_add_fetch(&counter, 1, __ATOMIC_RELAXED);
        return val ^ key;
    }
}  // namespace iodine::core