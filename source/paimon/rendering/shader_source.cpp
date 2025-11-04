#include "paimon/rendering/shader_source.h"

#include <regex>
#include <sstream>

#include "paimon/core/io/file.h"
#include "paimon/core/log_system.h"
#include "paimon/core/resource/resource_manager.h"

using namespace paimon;

ShaderSource::ShaderSource(const std::filesystem::path &filepath) {
  // Try to resolve the path using ResourceManager first
  auto& resMgr = ResourceManager::getInstance();
  std::filesystem::path resolvedPath = filepath;
  
  // If filepath is relative, try to resolve it through ResourceManager
  if (filepath.is_relative()) {
    auto resourcePath = resMgr.resolve(filepath.string(), ResourceManager::ResourceType::Shader);
    if (!resourcePath.empty()) {
      resolvedPath = resourcePath;
    }
  }
  
  // Load the shader source from the file
  m_source = File::readText(resolvedPath).value_or("");
  if (m_source.empty()) {
    LOG_ERROR("Failed to load shader source from: {}", resolvedPath.string());
  }

  // Set the base directory to the directory of the shader file
  m_baseDirectory = resolvedPath.parent_path();
}

void ShaderSource::addDefine(const std::string &define) {
  m_defines.push_back(define);
}

void ShaderSource::replace(
  const std::string &search, const std::string &replace
) {
  m_replacements[search] = replace;
}

void ShaderSource::setIncludePaths(
  const std::vector<std::filesystem::path> &paths
) {
  m_includePaths = paths;
}

std::string ShaderSource::getSource() const {
  std::string processedSource = m_source;

  // Clear included files from previous processing
  m_includedFiles.clear();

  // Process includes recursively
  processIncludes(processedSource, m_includedFiles);

  // Apply defines
  for (const auto &define : m_defines) {
    processedSource = "#define " + define + "\n" + processedSource;
  }

  // Apply replacements
  for (const auto &[search, replace] : m_replacements) {
    size_t pos = 0;
    while ((pos = processedSource.find(search, pos)) != std::string::npos) {
      processedSource.replace(pos, search.length(), replace);
      pos += replace.length();
    }
  }

  return processedSource;
}

const std::string &ShaderSource::getOriginalSource() const { return m_source; }

const std::unordered_set<std::filesystem::path> &
ShaderSource::getIncludedFiles() const {
  return m_includedFiles;
}

const std::filesystem::path &ShaderSource::getBaseDirectory() const {
  return m_baseDirectory;
}

void ShaderSource::processIncludes(
  std::string &source,
  std::unordered_set<std::filesystem::path> &includedFiles
) const {
  // Regular expression to match #include directives
  // Supports both #include "file.glsl" and #include <file.glsl>
  static const std::regex kIncludePattern(
    R"(^\s*#\s*include\s*([<"])([^>"]+)([>"]))"
  );

  std::string result;
  std::istringstream stream(source);
  std::string line;

  while (std::getline(stream, line)) {
    std::smatch match;

    // Check if this line contains an #include directive
    if (std::regex_search(line, match, kIncludePattern)) {
      std::string includePath = match[2].str();
      bool isSystemInclude = (match[1].str() == "<");

      // Resolve the full path
      std::filesystem::path fullPath =
        resolveIncludePath(includePath, isSystemInclude);

      if (fullPath.empty()) {
        LOG_ERROR("Failed to resolve include path: {}", includePath);
        result += "// Failed to resolve: " + includePath + "\n";
        continue;
      }

      // Normalize the path
      fullPath = fullPath.lexically_normal();

      // Check for circular includes
      if (includedFiles.find(fullPath) != includedFiles.end()) {
        LOG_WARN("Circular include detected, skipping: {}", fullPath.string());
        result += "// Circular include: " + includePath + "\n";
        continue;
      }

      // Mark this file as included
      includedFiles.insert(fullPath);

      // Load the include file
      auto includeContent = File::readText(fullPath);

      if (!includeContent.has_value()) {
        LOG_ERROR("Failed to load shader include file: {}", fullPath.string());
        result += "// Failed to include: " + includePath + "\n";
        result += line + "\n";
        continue;
      }

      // Add a comment to show where the include starts
      result += "// Begin include: " + includePath + "\n";

      // Recursively process includes in the included file
      std::string content = includeContent.value();
      processIncludes(content, includedFiles);

      result += content;

      // Add a comment to show where the include ends
      result += "\n// End include: " + includePath + "\n";
    } else {
      // Not an include directive, keep the line as-is
      result += line + "\n";
    }
  }

  source = result;
}

std::filesystem::path ShaderSource::resolveIncludePath(
  const std::string &includePath, bool isSystemInclude
) const {
  std::filesystem::path path(includePath);

  // For local includes (#include "file"), try relative to current file first
  if (!isSystemInclude) {
    std::filesystem::path localPath = m_baseDirectory / path;
    if (std::filesystem::exists(localPath)) {
      return localPath;
    }
  }

  // Search in include paths
  for (const auto &searchPath : m_includePaths) {
    std::filesystem::path fullPath = searchPath / path;
    if (std::filesystem::exists(fullPath)) {
      return fullPath;
    }
  }

  // If not found, return empty path
  return {};
}