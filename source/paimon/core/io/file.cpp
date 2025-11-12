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
  file.read(reinterpret_cast<char *>(buffer.data()), size);

  return buffer;
}

void File::writeText(const std::filesystem::path &path,
                     const std::string &content) {
  // Create parent directories if they don't exist
  if (path.has_parent_path()) {
    std::filesystem::create_directories(path.parent_path());
  }

  std::ofstream file(path, std::ios::out | std::ios::trunc);
  if (!file) {
    LOG_ERROR("Failed to write file: {}", path.string());
    return;
  }

  file << content;
  file.close();

  LOG_INFO("File written successfully: {}", path.string());
}