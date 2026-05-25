#include "rm/plugin/registry.hpp"

#include <filesystem>

#if defined(RM_MACOS) || defined(RM_LINUX) || defined(RM_UNIX)
#include <dlfcn.h>
#endif

#include "rm/debug/log.hpp"

namespace rome::core {
    namespace Plugin {
        struct LoadingContext {
            const std::string& path;
            const LoadingContext* parent = nullptr;
        };

        static thread_local const LoadingContext* activeLoadingContext = nullptr;

        struct ScopedLoadingContext {
            explicit ScopedLoadingContext(const std::string& path) : context{path, activeLoadingContext} {
                activeLoadingContext = &context;
            }

            ~ScopedLoadingContext() { activeLoadingContext = context.parent; }

            LoadingContext context;
        };

        static void* openLibrary([[maybe_unused]] const std::string& path) {
#if defined(RM_MACOS) || defined(RM_LINUX) || defined(RM_UNIX)
            return dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
#else
            return nullptr;
#endif
        }

        static void* findSymbol([[maybe_unused]] void* handle, [[maybe_unused]] const char* name) {
            if (!handle) return nullptr;
#if defined(RM_MACOS) || defined(RM_LINUX) || defined(RM_UNIX)
            dlerror();
            return dlsym(handle, name);
#else
            return nullptr;
#endif
        }

        static const char* getLibraryError() {
#if defined(RM_MACOS) || defined(RM_LINUX) || defined(RM_UNIX)
            const char* error = dlerror();
            return error ? error : "unknown dynamic library error";
#else
            return "dynamic libraries are not implemented on this platform";
#endif
        }

        Registry::~Registry() = default;

        std::string Registry::resolvePath(const std::string& path) const {
            namespace fs = std::filesystem;

            fs::path requested(path);
            if (!requested.is_absolute()) {
                if (activeLoadingContext) {
                    fs::path relativeToLoader = fs::path(activeLoadingContext->path).parent_path() / requested;
                    if (fs::exists(relativeToLoader)) requested = relativeToLoader;
                }
            }

            std::error_code error;
            fs::path resolved = fs::weakly_canonical(requested, error);
            if (error) resolved = fs::absolute(requested, error);
            if (error) return requested.string();
            return resolved.string();
        }

        ID Registry::load(const std::string& path, ECS& ecs) {
            const std::string resolvedPath = resolvePath(path);
            {
                std::unique_lock lock(pluginsLock);
                for (auto& [id, library] : libraries) {
                    if (library.getLoadingPath() == resolvedPath) {
                        library.addReference();
                        return id;
                    }
                }
            }

            for (const LoadingContext* context = activeLoadingContext; context; context = context->parent) {
                if (context->path == resolvedPath) {
                    RM_ERROR("Could not load plugin '%s': circular plugin dependency", resolvedPath.c_str());
                    return INVALID_ID;
                }
            }

            void* handle = openLibrary(resolvedPath);
            if (!handle) {
                RM_ERROR("Could not load plugin '%s': %s", resolvedPath.c_str(), getLibraryError());
                return INVALID_ID;
            }

            auto* loadSymbol = findSymbol(handle, LOAD_SYMBOL);
            if (!loadSymbol) {
                RM_ERROR("Plugin '%s' is missing '%s': %s", resolvedPath.c_str(), LOAD_SYMBOL, getLibraryError());
                Library{INVALID_ID, resolvedPath, handle, nullptr};
                return INVALID_ID;
            }

            auto* unloadSymbol = findSymbol(handle, UNLOAD_SYMBOL);
            auto load = reinterpret_cast<LoadFn>(loadSymbol);
            auto unload = reinterpret_cast<UnloadFn>(unloadSymbol);

            ScopedLoadingContext loadingContext(resolvedPath);
            load(ecs);

            std::unique_lock lock(pluginsLock);
            ID id = nextId++;
            libraries.try_emplace(id, id, resolvedPath, handle, unload);
            return id;
        }

        b8 Registry::unload(ID id, ECS& ecs) {
            decltype(libraries)::node_type library;
            {
                std::unique_lock lock(pluginsLock);
                auto it = libraries.find(id);
                if (it == libraries.end()) return false;

                Library& stored = it->second;
                if (stored.getReferences() > 1) {
                    stored.removeReference();
                    return true;
                }

                library = libraries.extract(it);
            }

            library.mapped().unloadFrom(ecs);
            return true;
        }

        void Registry::unload(ECS& ecs) {
            while (true) {
                ID id = INVALID_ID;
                {
                    std::shared_lock lock(pluginsLock);
                    if (libraries.empty()) return;
                    for (const auto& [libraryId, library] : libraries) {
                        if (libraryId > id) id = libraryId;
                    }
                }
                unload(id, ecs);
            }
        }

        u32 Registry::getSize() const noexcept {
            std::shared_lock lock(pluginsLock);
            return libraries.size();
        }
    }  // namespace Plugin
}  // namespace rome::core
