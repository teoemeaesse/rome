#include "rm/plugin/library.hpp"

#include <utility>

#if defined(RM_MACOS) || defined(RM_LINUX) || defined(RM_UNIX)
#include <dlfcn.h>
#endif

namespace rome::core {
    namespace Plugin {
        static void closeLibrary([[maybe_unused]] void* handle) {
            if (!handle) return;
#if defined(RM_MACOS) || defined(RM_LINUX) || defined(RM_UNIX)
            dlclose(handle);
#endif
        }

        Library::Library(ID id, std::string loadingPath, void* handle, UnloadFn unload)
            : id(id), loadingPath(std::move(loadingPath)), handle(handle), unload(unload) {}

        Library::~Library() { close(); }

        Library::Library(Library&& other) noexcept
            : id(other.id),
              loadingPath(std::move(other.loadingPath)),
              handle(other.handle),
              unload(other.unload),
              references(other.references) {
            other.id = INVALID_ID;
            other.handle = nullptr;
            other.unload = nullptr;
            other.references = 0;
        }

        Library& Library::operator=(Library&& other) noexcept {
            if (this == &other) return *this;

            close();
            id = other.id;
            loadingPath = std::move(other.loadingPath);
            handle = other.handle;
            unload = other.unload;
            references = other.references;

            other.id = INVALID_ID;
            other.handle = nullptr;
            other.unload = nullptr;
            other.references = 0;
            return *this;
        }

        void Library::unloadFrom(ECS& ecs) {
            if (unload) unload(ecs);
        }

        void Library::close() noexcept {
            if (!handle) return;
            closeLibrary(handle);
            handle = nullptr;
        }
    }  // namespace Plugin
}  // namespace rome::core
