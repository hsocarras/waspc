#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace waspc {
namespace test {
namespace wasm {

bool ReadFileContent(const std::string &filename, const uint8_t *output, std::string &error);
uint32_t ReadFileSize(const std::string &filename);

} // namespace wasm
} // namespace test
} // namespace waspc
