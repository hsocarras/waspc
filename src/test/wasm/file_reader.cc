#include "file_reader.h"
#include <filesystem>
#include <fstream>
#include <limits>
#include <system_error>

namespace waspc {
namespace test {
namespace wasm {

bool ReadFileContent(const std::string &filename, const uint8_t *output, std::string &error) {
    std::filesystem::path wasm_dir(__FILE__);
    wasm_dir = wasm_dir.parent_path();  // Directorio src/test/wasm/

    std::filesystem::path full_path;
    full_path = wasm_dir / filename;

    if (!std::filesystem::exists(full_path)) {
        error = "Archivo no encontrado en directorio wasm: " + full_path.string();
        return false;
    }

    if (!output) {
        error = "Buffer de salida nulo para archivo: " + full_path.string();
        return false;
    }

    std::ifstream file(full_path, std::ios::binary);
    if (!file) {
        error = "No se pudo abrir el archivo: " + full_path.string();
        return false;
    }

    const uintmax_t size = std::filesystem::file_size(full_path);
    file.read(reinterpret_cast<char *>(const_cast<uint8_t *>(output)), static_cast<std::streamsize>(size));
    if (!file.good() && !file.eof()) {
        error = "Error de lectura en " + full_path.string();
        return false;
    }

    return true;
}


uint32_t ReadFileSize(const std::string &filename) {
    std::filesystem::path wasm_dir(__FILE__);
    wasm_dir = wasm_dir.parent_path();

    std::filesystem::path full_path = wasm_dir / filename;
    std::error_code error;
    const uintmax_t size = std::filesystem::file_size(full_path, error);

    if (error || size > std::numeric_limits<uint32_t>::max()) {
        return 0;
    }

    return static_cast<uint32_t>(size);
}

} // namespace wasm
} // namespace test
} // namespace waspc
