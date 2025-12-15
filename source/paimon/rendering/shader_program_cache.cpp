#include "paimon/rendering/shader_program_cache.h"

#include <regex>

#include "paimon/core/hash.h"
#include "paimon/core/log_system.h"

using namespace paimon;

ShaderProgram *
ShaderProgramCache::get(const std::string &baseSource, GLenum type,
                        const std::vector<ShaderDefine> &defines) {
  // Compute hash for the defines
  std::size_t hash = createHash(defines);

  // Check if program exists in cache
  auto it = m_cache.find(hash);
  if (it != m_cache.end()) {
    return it->second.get();
  }

  // Create new shader program
  auto program = createShaderProgram(baseSource, defines, type);
  if (!program || !program->is_valid()) {
    LOG_ERROR("Failed to create shader program");
    if (program) {
      std::string infoLog = program->get_info_log();
      if (!infoLog.empty()) {
        LOG_ERROR("Shader compilation error:\n{}", infoLog);
      }
    }
    return nullptr;
  }

  // Check for warnings/info
  std::string infoLog = program->get_info_log();
  if (!infoLog.empty()) {
    LOG_INFO("Shader program info log:\n{}", infoLog);
  }

  ShaderProgram *ptr = program.get();
  m_cache[hash] = std::move(program);

  LOG_DEBUG("Created and cached shader program (cache size: {})",
            m_cache.size());

  return ptr;
}

void ShaderProgramCache::clear() { m_cache.clear(); }

std::size_t
ShaderProgramCache::createHash(const std::vector<ShaderDefine> &defines) const {
  std::size_t hash = 0;
  for (const auto &define : defines) {
    hashCombine(hash, define.getHash());
  }
  return hash;
}

std::unique_ptr<ShaderProgram> ShaderProgramCache::createShaderProgram(
    const std::string &baseSource, const std::vector<ShaderDefine> &defines,
    GLenum type) const {

  std::string variantSource = baseSource;

  // Build define block
  std::string defineBlock;
  for (const auto &define : defines) {
    defineBlock += define.getSource();
  }

  // Insert defines after #version line
  static const std::regex versionPattern(R"(^[ \t]*#version\s+\d+[^\n]*)");
  std::smatch match;

  if (std::regex_search(variantSource, match, versionPattern)) {
    // Insert defines after #version line
    size_t insertPos = match.position() + match.length();
    // Find the end of the line
    size_t newlinePos = variantSource.find('\n', insertPos);
    if (newlinePos != std::string::npos) {
      variantSource.insert(newlinePos + 1, defineBlock);
    } else {
      variantSource += "\n" + defineBlock;
    }
  } else {
    // No #version, insert at beginning
    variantSource.insert(0, defineBlock + "\n");
  }

  return std::make_unique<ShaderProgram>(type, variantSource);
}