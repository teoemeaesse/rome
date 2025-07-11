#pragma once

/* Rome API export/import */
#ifdef RM_EXPORT_ON
#ifdef _MSC_VER
#define RM_API __declspec(dllexport)
#else
#define RM_API __attribute__((visibility("default")))
#endif
#else
#ifdef _MSC_VER
#define RM_API __declspec(dllimport)
#else
#define RM_API
#endif
#endif

/* Static asserts */
#ifndef STATIC_ASSERT
#ifdef __cplusplus
#define STATIC_ASSERT static_assert
#elif defined(__clang__) || defined(__GNUC__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif
#endif

/* Exceptions */
#ifdef RM_EXCEPTIONS_ON
#include <stdexcept>
#define STATIC_CORE_ASSERT_EXCEPTION(expected, message)           \
    STATIC_ASSERT((expected), message)                            \
    if (!expected) {                                              \
        THROW_CORE_EXCEPTION(Exception::Type::Assertion, message) \
    }
#else
#define STATIC_CORE_ASSERT_EXCEPTION(expected, message) STATIC_ASSERT((expected), message)
#endif

/* Debug asserts */
#ifdef RM_ASSERTS_ON
#ifdef _MSC_VER
#include <intrin.h>
#define RM_DEBUG_BREAK() __debugbreak()
#else
#define RM_DEBUG_BREAK() __builtin_trap()
#endif

/**
 * @brief Reports an assertion failure.
 * @param expression The expression that failed.
 * @param file The file containing the assertion.
 * @param line The line number of the assertion.
 * @param function The function containing the assertion.
 * @warning This function should ALWAYS be called instead of the
 *      second definition if the assertion message is not provided.
 */
RM_API void assertFail(const char* expression, const char* file, int line, const char* function);
/**
 * @brief Reports an assertion failure.
 * @param expression The expression that failed.
 * @param message The assertion message. Length must be greater than 0.
 * @param file The file containing the assertion.
 * @param line The line number of the assertion.
 * @param function The function containing the assertion.
 * @warning This function should NEVER be called with an empty message.
 */
RM_API void assertFail(const char* expression, const char* message, const char* file, int line, const char* function);
#define RM_ASSERT(expression)                                          \
    {                                                                  \
        if (expression)                                                \
            ;                                                          \
        else {                                                         \
            assertFail(#expression, __FILE__, __LINE__, __FUNCTION__); \
            RM_DEBUG_BREAK();                                          \
        }                                                              \
    }
#define RM_ASSERT_MSG(expression, message)                                      \
    {                                                                           \
        if (expression)                                                         \
            ;                                                                   \
        else {                                                                  \
            assertFail(#expression, message, __FILE__, __LINE__, __FUNCTION__); \
            RM_DEBUG_BREAK();                                                   \
        }                                                                       \
    }
#else
#define RM_ASSERT(expression)
#define RM_ASSERT_MSG(expression, message)
#endif

#ifdef RM_EXCEPTIONS_ON
#include <stdexcept>
#define CORE_ASSERT_EXCEPTION(expected, message)                   \
    RM_ASSERT_MSG((expected), message)                             \
    if (!(expected)) {                                             \
        THROW_CORE_EXCEPTION(Exception::Type::Assertion, message); \
    }
#else
#define CORE_ASSERT_EXCEPTION(expected, message) RM_ASSERT_MSG((expected), message)
#endif

/* Platform definitions (Windows 64-bit, MacOS, Linux and Unix/Unix-like) */
#if defined(_WIN64)
#define PLATFORM_NAME "Windows"
#define RM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#if TARGET_OS_MAC
#define PLATFORM_NAME "MacOS"
#define RM_MACOS
#else
#error "Unsupported Apple platform"
#endif
#elif defined(__linux__)
#define PLATFORM_NAME "Linux"
#define RM_LINUX
#elif defined(__unix__)
#define PLATFORM_NAME "Unix-like"
#define RM_UNIX
#else
#define PLATFORM_NAME "Unknown"
#define RM_UNKNOWN_PLATFORM
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

/* Standard headers */
#include <algorithm>
#include <atomic>
#include <cstring>
#include <memory>
#include <mutex>
#include <queue>
#include <span>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace rome {
    /* Primitive types */
    using u8 = unsigned char;
    using u16 = unsigned short;
    using u32 = unsigned int;
    using u64 = unsigned long long;
    using i8 = signed char;
    using i16 = signed short;
    using i32 = signed int;
    using i64 = signed long long;
    using f32 = float;
    using f64 = double;
    using b8 = char;
    using b16 = short;
    using b32 = int;
    using byte = char;

    STATIC_ASSERT(sizeof(u8) == 1, "u8 type is not 1 byte");
    STATIC_ASSERT(sizeof(u16) == 2, "u16 type is not 2 bytes");
    STATIC_ASSERT(sizeof(u32) == 4, "u32 type is not 4 bytes");
    STATIC_ASSERT(sizeof(u64) == 8, "u64 type is not 8 bytes");
    STATIC_ASSERT(sizeof(i8) == 1, "i8 type is not 1 byte");
    STATIC_ASSERT(sizeof(i16) == 2, "i16 type is not 2 bytes");
    STATIC_ASSERT(sizeof(i32) == 4, "i32 type is not 4 bytes");
    STATIC_ASSERT(sizeof(i64) == 8, "i64 type is not 8 bytes");
    STATIC_ASSERT(sizeof(f32) == 4, "f32 type is not 4 bytes");
    STATIC_ASSERT(sizeof(f64) == 8, "f64 type is not 8 bytes");
    STATIC_ASSERT(sizeof(b8) == 1, "b8 type is not 1 byte");
    STATIC_ASSERT(sizeof(b16) == 2, "b16 type is not 2 bytes");
    STATIC_ASSERT(sizeof(b32) == 4, "b32 type is not 4 bytes");
    STATIC_ASSERT(sizeof(byte) == 1, "byte type is not 1 byte");

    /* Smart pointers */
    template <typename T>
    using Unique = std::unique_ptr<T>;
    template <typename T>
    using Shared = std::shared_ptr<T>;
    template <typename T>
    using Weak = std::weak_ptr<T>;

    template <typename T, typename... Args>
    constexpr Unique<T> MakeUnique(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    template <typename T, typename... Args>
    constexpr Shared<T> MakeShared(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
    template <typename T, typename... Args>
    constexpr Weak<T> MakeWeak(const Shared<T>& ptr) {
        return std::weak_ptr<T>(ptr);
    }

    // Implementation struct: Handles pointers and arrays
    template <typename T>
    struct remove_all_qualifiers_impl {
        using type = T;
    };

    // Primary template: Initiates the removal process
    template <typename T>
    struct remove_all_qualifiers {
        private:
        // Remove references and top-level cv-qualifiers
        using NoRefNoCV = std::remove_cv_t<std::remove_reference_t<T>>;

        public:
        // Delegate to the implementation struct
        using type = typename remove_all_qualifiers_impl<NoRefNoCV>::type;
    };

    // Specialization for pointers
    template <typename T>
    struct remove_all_qualifiers_impl<T*> {
        using type = typename remove_all_qualifiers<T>::type;
    };

    // Specialization for arrays of unknown bound
    template <typename T>
    struct remove_all_qualifiers_impl<T[]> {
        using type = typename remove_all_qualifiers<T>::type;
    };

    // Specialization for arrays of known bound
    template <typename T, std::size_t N>
    struct remove_all_qualifiers_impl<T[N]> {
        using type = typename remove_all_qualifiers<T>::type;
    };

    // Type alias for convenience
    template <typename T>
    using remove_all_qualifiers_t = typename remove_all_qualifiers<T>::type;

    /** TODO: Move into platform-specific header
     * @brief Demangles the type name string (from typeid(T).name()).
     * @param name The mangled name to demangle.
     * @return A string containing the demangled name.
     *
    static inline const std::string demangle(const char* name) noexcept {
        int status = 42;
        std::unique_ptr<char, void (*)(void*)> res{abi::__cxa_demangle(name, nullptr, nullptr, &status), std::free};
        return (status == 0) ? res.get() : name;
    }*/

    // Hash function for transparent string views
    struct TransparentSVHash {
        using is_transparent = void;
        std::size_t operator()(std::string_view sv) const noexcept { return std::hash<std::string_view>{}(sv); }
        std::size_t operator()(const std::string& s) const noexcept { return std::hash<std::string>{}(s); }
    };
}  // namespace rome