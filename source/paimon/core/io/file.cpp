#include "paimon/core/io/file.h"

#include <fstream>
#include <filesystem>
#include <sstream>

#include "paimon/core/log_system.h"

using namespace paimon;

std::optional<std::vector<uint8_t>> File::readBinary(const std::filesystem::path& filepath) {
  std::error_code ec;
  
  if (!std::filesystem::exists(filepath, ec) || ec) {
    LOG_ERROR("File does not exist: {}", filepath.string());
    return std::nullopt;
  }
  
  auto fileSize = std::filesystem::file_size(filepath, ec);
  if (ec) {
    LOG_ERROR("Failed to get file size: {} - {}", filepath.string(), ec.message());
    return std::nullopt;
  }
  
  std::ifstream file(filepath, std::ios::binary);
  if (!file.is_open()) {
    LOG_ERROR("Failed to open file: {}", filepath.string());
    return std::nullopt;
  }
  
  std::vector<uint8_t> buffer(fileSize);
  if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
    LOG_ERROR("Failed to read file: {}", filepath.string());
    return std::nullopt;
  }
  
  return buffer;
}

std::optional<std::string> File::readText(const std::filesystem::path& filepath) {
  std::error_code ec;
  
  if (!std::filesystem::exists(filepath, ec) || ec) {
    LOG_ERROR("File does not exist: {}", filepath.string());
    return std::nullopt;
  }
  
  std::ifstream file(filepath);
  if (!file.is_open()) {
    LOG_ERROR("Failed to open file: {}", filepath.string());
    return std::nullopt;
  }
  
  std::stringstream buffer;
  buffer << file.rdbuf();
  
  return buffer.str();
}