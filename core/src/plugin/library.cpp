#include "rm/plugin/library.hpp"

#include <utility>

#if defined(RM_MACOS) || defined(RM_LINUX) || defined(RM_UNIX)
#include <dlfcn.h>
#endif

namespace rome::core {
    namespace Plugin {
        static void* findSymbol(void* handle, const char* name) {
            if (!handle) return nullptr;
#if defined(RM_MACOS) || defined(RM_LINUX) || defined(RM_UNIX)
            dlerror();
            return dlsym(handle, name);
#else
            return nullptr;
#endif
        }

        static void closeLibrary(void* handle) {
            if (!handle) return;
#if defined(RM_MACOS) || defined(RM_LINUX) || defined(RM_UNIX)
            dlclose(handle);
#endif
        }

        Library::Library(ID id, Descriptor&& descriptor, void* handle) : id(id), descriptor(std::move(descriptor)), handle(handle) {}

        Library::~Library() { close(); }

        Library::Library(Library&& other) noexcept
            : id(other.id), descriptor(std::move(other.descriptor)), handle(other.handle) {
            other.id = INVALID_ID;
            other.handle = nullptr;
        }

        Library& Library::operator=(Library&& other) noexcept {
            if (this == &other) return *this;

            close();
            id = other.id;
            descriptor = std::move(other.descriptor);
            handle = other.handle;

            other.id = INVALID_ID;
            other.handle = nullptr;
            return *this;
        }

        void Library::unload(ECS& ecs) {
            auto unload = reinterpret_cast<UnloadFn>(findSymbol(handle, UNLOAD_SYMBOL));
            if (unload) unload(ecs);
        }

        void Library::close() noexcept {
            if (!handle) return;
            closeLibrary(handle);
            handle = nullptr;
        }
    }  // namespace Plugin
}  // namespace rome::core
