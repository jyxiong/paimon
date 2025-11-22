#include "paimon/rendering/shader_preprocessor.h"

#include <algorithm>
#include <regex>

#include "paimon/core/io/file.h"
#include "paimon/core/log_system.h"
#include "paimon/rendering/shader_source.h"

using namespace paimon;

ShaderPreprocessor::ShaderPreprocessor() {}

void ShaderPreprocessor::addIncludePath(const std::filesystem::path &path) {
  auto absPath = std::filesystem::absolute(path);
  if (!std::filesystem::exists(absPath)) {
    LOG_WARN("Include path does not exist: {}", absPath.string());
  }
  m_includePaths.push_back(absPath);
}

void ShaderPreprocessor::setIncludePaths(
    const std::vector<std::filesystem::path> &paths) {
  m_includePaths.clear();
  for (const auto &path : paths) {
    addIncludePath(path);
  }
}

std::string
ShaderPreprocessor::processShaderSource(const ShaderSource &shaderSource) {
  m_processingStack.clear();

  std::string source = shaderSource.getSource();
  const auto &defines = shaderSource.getDefines();

  // Resolve defines
  resolveDefines(source, defines);

  resolveIncludes(source);
  return source;
}

void ShaderPreprocessor::clearCache() {
  m_includedFiles.clear();
  m_processingStack.clear();
}

void ShaderPreprocessor::resolveDefines(
    std::string &source, const std::vector<std::string> &defines) const {

  if (defines.empty()) {
    return;
  }

  static const std::regex versionPattern(R"(^[ \t]*#version\s+\d+[^\n]*)");
  std::smatch match;

  std::string defineBlock;
  for (const auto &define : defines) {
    defineBlock += "#define " + define + "\n";
  }

  if (std::regex_search(source, match, versionPattern)) {
    // Insert defines after #version line
    size_t insertPos = match.position() + match.length();
    // Find the end of the line
    size_t newlinePos = source.find('\n', insertPos);
    if (newlinePos != std::string::npos) {
      source.insert(newlinePos + 1, defineBlock);
    } else {
      source += "\n" + defineBlock;
    }
  } else {
    // No #version, insert at beginning
    source.insert(0, defineBlock + "\n");
  }
}

void ShaderPreprocessor::resolveIncludes(std::string &source) {
  static const std::regex includePattern(
      R"(^\s*#include\s+([<"])([^>"]+)([>"])\s*(?://.*)?$)",
      std::regex_constants::multiline);

  // Process all includes
  std::smatch match;
  while (std::regex_search(source, match, includePattern)) {
    // match[1] = opening delimiter (" or <)
    // match[2] = file path
    // match[3] = closing delimiter (" or >)
    std::string includePath = match[2].str();

    // Resolve the include path: search in all include paths
    std::filesystem::path path(includePath);
    std::filesystem::path resolvedPath;

    // Try include paths
    auto found = std::find_if(m_includePaths.begin(), m_includePaths.end(),
                              [&](const std::filesystem::path &searchPath) {
                                auto fullPath = searchPath / path;
                                if (std::filesystem::exists(fullPath)) {
                                  resolvedPath =
                                      std::filesystem::canonical(fullPath);
                                  return true;
                                }
                                return false;
                              });

    if (found == m_includePaths.end()) {
      LOG_ERROR("Included file not found: {}", includePath);
      return;
    }

    // Check for circular includes
    if (std::find(m_processingStack.begin(), m_processingStack.end(),
                  resolvedPath) != m_processingStack.end()) {
      LOG_ERROR("Circular include detected: {}", resolvedPath.string());
      return;
    }

    // Check if already included (include guard)
    std::string replacement;
    if (m_includedFiles.find(resolvedPath) != m_includedFiles.end()) {
      replacement =
          "// [Already included: " + resolvedPath.filename().string() + "]";
    } else {
      // Mark as included
      m_includedFiles.insert(resolvedPath);

      // Add to processing stack
      m_processingStack.push_back(resolvedPath);

      // Read the included file
      std::string includeSource = File::readText(resolvedPath);
      if (includeSource.empty()) {
        return;
      }

      // Recursively process the included file
      resolveIncludes(includeSource);

      // Build replacement with comments
      replacement =
          "// --- Begin include: " + includePath + " ---\n" + includeSource;
      if (!includeSource.empty() && includeSource.back() != '\n') {
        replacement += "\n";
      }
      replacement += "// --- End include: " + includePath + " ---";

      // Remove from processing stack
      m_processingStack.pop_back();
    }

    // Replace the #include directive with the processed content
    source.replace(match.position(), match.length(), replacement);
  }
}
