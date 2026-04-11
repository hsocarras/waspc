#include "file_reader.h"
#include <filesystem>
#include <fstream>

namespace waspc {
namespace test {
namespace wasm {

bool ReadFileContent(const std::string &filename, std::vector<uint8_t> &output, std::string &error) {
    std::filesystem::path wasm_dir(__FILE__);
    wasm_dir = wasm_dir.parent_path();  // Directorio src/test/wasm/

    std::filesystem::path full_path;
    full_path = wasm_dir / filename;

    if (!std::filesystem::exists(full_path)) {
        error = "Archivo no encontrado en directorio wasm: " + full_path.string();
        return false;
    }

    std::ifstream file(full_path, std::ios::binary);
    if (!file) {
        error = "No se pudo abrir el archivo: " + full_path.string();
        return false;
    }

    output.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    if (!file.good() && !file.eof()) {
        error = "Error de lectura en " + full_path.string();
        return false;
    }

    return true;
}

} // namespace wasm
} // namespace test
} // namespace waspc
