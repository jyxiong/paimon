#include "paimon/core/io/file.h"

#include <fstream>

#include "paimon/core/log_system.h"

using namespace paimon;

std::string File::readText(const std::filesystem::path &path) {
  std::ifstream file(path, std::ios::in);
  if (!file) {
    LOG_ERROR("Failed to open file: {}", path.string());
    return "";
  }

  return std::string(std::istreambuf_iterator<char>(file),
                     std::istreambuf_iterator<char>());
}

std::vector<uint8_t> File::readBinary(const std::filesystem::path &path) {
  std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
  if (!file) {
    LOG_ERROR("Failed to open file: {}", path.string());
    return {};
  }

  const auto size = file.tellg();
  file.seekg(0, std::ios::beg);
  
  std::vector<uint8_t> buffer(static_cast<size_t>(size));
  file.read(reinterpret_cast<char*>(buffer.data()), size);
  
  return buffer;
}