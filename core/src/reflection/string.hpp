#pragma once

#include "reflection/reflect.hpp"

IO_REFLECT(std::string, "std::string");
IO_REFLECT(std::string*, "std::string*");
IO_REFLECT(std::string**, "std::string**");
IO_REFLECT(std::string&, "std::string&");
IO_REFLECT(std::string&&, "std::string&&");
IO_REFLECT(std::string[], "std::string[]");

IO_REFLECT(const std::string, "const std::string");
IO_REFLECT(const std::string*, "const std::string*");
IO_REFLECT(const std::string**, "const std::string**");
IO_REFLECT(const std::string&, "const std::string&");
IO_REFLECT(const std::string&&, "const std::string&&");
IO_REFLECT(const std::string[], "const std::string[]");