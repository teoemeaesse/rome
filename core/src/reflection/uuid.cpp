#include "reflection/uuid.hpp"

#include "platform/platform.hpp"

namespace iodine::core {
    UUIDGenerator::UUIDGenerator() : key(Platform::getInstance().random_u64()), counter(0) {
        while (!key) {
            key = Platform::getInstance().random_u64();
        }
    }

    UUID UUIDGenerator::generate() {
        u64 val = counter.fetch_add(1, std::memory_order_relaxed) + 1;
        return val ^ key;
    }
}  // namespace iodine::core