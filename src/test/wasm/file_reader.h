#pragma once

#include <vector>
#include <cstdint>
#include <string>

namespace waspc {
namespace test {
namespace wasm {

bool ReadFileContent(const std::string &filename, std::vector<uint8_t> &output, std::string &error);

} // namespace wasm
} // namespace test
} // namespace waspc
