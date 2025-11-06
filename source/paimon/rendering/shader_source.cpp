#include "paimon/rendering/shader_source.h"

#include "paimon/core/io/file.h"

using namespace paimon;

ShaderSource::ShaderSource(const std::filesystem::path &filePath) {
  m_source = File::readText(filePath);
}

void ShaderSource::define(const std::string &define) {
  m_defines.push_back(define);
}