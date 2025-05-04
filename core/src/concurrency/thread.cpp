#include "concurrency/thread.hpp"

static thread_local iodine::core::UUID localID{};
static thread_local std::string localAlias = "Main";

namespace iodine::core {
    Thread::Thread(const std::string& alias) : alias(alias) {}

    Thread::~Thread() {
        if (thread.joinable()) {
            thread.join();
        }
    }

    Thread::Thread(Thread&& other) noexcept : thread(std::move(other.thread)) {}

    Thread& Thread::operator=(Thread&& other) noexcept {
        if (this != &other) {
            if (thread.joinable()) {
                thread.join();  // Join current thread before moving
            }
            thread = std::move(other.thread);
        }
        return *this;
    }

    void Thread::join() {
        if (thread.joinable()) {
            thread.join();
        }
    }

    void Thread::detach() {
        if (thread.joinable()) {
            thread.detach();
        }
    }

    const UUID& Thread::getID() const { return id; }

    const std::string& Thread::getAlias() const { return alias; }

    b8 Thread::isRunning() const { return thread.joinable(); }

    const UUID& ThreadInfo::getLocalID() { return localID; }

    const std::string& ThreadInfo::getLocalAlias() { return localAlias; }

    void ThreadInfo::setLocalID(const UUID& id) { localID = id; }

    void ThreadInfo::setLocalAlias(const std::string& alias) { localAlias = alias; }
}  // namespace iodine::core