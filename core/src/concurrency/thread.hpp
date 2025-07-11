#pragma once

#include <thread>

#include "reflection/uuid.hpp"

namespace rome::core {
    namespace ThreadInfo {
        /**
         * @brief Gets the current thread ID.
         * @return The current thread ID.
         */
        const UUID& getLocalID();
        /**
         * @brief Gets the current thread alias.
         * @return The current thread alias.
         */
        const std::string& getLocalAlias();

        /**
         * @brief Sets the UUID for the current thread.
         * @param id The new UUID for the thread.
         */
        void setLocalID(const UUID& id);
        /**
         * @brief Sets the alias for the current thread.
         * @param alias The new alias for the thread.
         * @note Every thread should call this after creation to set its alias. This implementation of threading
         *       does so by default.
         */
        void setLocalAlias(const std::string& alias);
    }  // namespace ThreadInfo

    /**
     * @brief A class that represents a thread of execution. Should be instantiated via a pool / dispatcher.
     */
    class RM_API Thread {
        public:
        Thread(const std::string& alias);
        ~Thread();
        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;
        Thread(Thread&& other) noexcept;
        Thread& operator=(Thread&& other) noexcept;

        /**
         * @brief Begins thread execution.
         * @tparam Function The function type to run in the thread.
         * @tparam ...Args The types of arguments to pass to the function.
         * @param function The function to run in the thread.
         * @param ...args The arguments to pass to the function.s
         */
        template <typename Function, typename... Args>
        void run(Function&& function, Args&&... args) {
            start(std::forward<Function>(function), std::forward<Args>(args)...);
        }

        /**
         * @brief Joins the thread, blocking until it completes.
         */
        void join();

        /**
         * @brief Detaches the thread, allowing it to run in the background.
         */
        void detach();

        /**
         * @brief Gets the UUID for this thread.
         * @return The thread UUID.
         */
        const UUID& getID() const;

        /**
         * @brief Gets the alias for this thread.
         * @return The thread alias.
         */
        const std::string& getAlias() const;

        /**
         * @brief Checks whether the thread is running.
         * @return Whether the thread is running.
         */
        b8 isRunning() const;

        private:
        UUID id;               ///< The UUID for this thread.
        std::string alias;     ///< The alias for this thread.
        std::thread thread{};  ///< The thread object.

        /**
         * @brief Starts the thread after registering it with the metrics tracker.
         * @tparam Function The function type to run in the thread.
         * @tparam ...Args The types of arguments to pass to the function.
         * @param function The function to run in the thread.
         * @param ...args The arguments to pass to the function.
         */
        template <typename Function, typename... Args>
        void start(Function&& function, Args&&... args) {
            std::string aliasCopy = alias;
            UUID idCopy = id;
            auto funcCopy = std::forward<Function>(function);
            auto argsTuple = std::make_tuple(std::forward<Args>(args)...);

            thread = std::thread([idCopy, aliasCopy, funcCopy = std::move(funcCopy), argsTuple = std::move(argsTuple)]() mutable {
                ThreadInfo::setLocalAlias(aliasCopy);  // Inject the alias and ID into the thread.
                ThreadInfo::setLocalID(idCopy);
                std::apply(funcCopy, std::move(argsTuple));
            });
        }
    };
}  // namespace rome::core