#pragma once

#include "app/app.hpp"

/**
 * @brief Creates a new application. This function must be implemented by the user.
 * @return A pointer to the application.
 */
extern iodine::core::Application* createApplication();