#include "paimon/rendering/shader_includer.h"

#include <algorithm>
#include <regex>

#include "paimon/core/io/file.h"
#include "paimon/core/log_system.h"

using namespace paimon;

void ShaderIncluder::addSearchPath(const std::filesystem::path &path) {
  auto absPath = std::filesystem::absolute(path);
  if (!std::filesystem::exists(absPath)) {
    LOG_WARN("Include path does not exist: {}", absPath.string());
  }
  m_searchPaths.push_back(absPath);
}

std::string ShaderIncluder::process(const std::string &source) {
  // Clear state for new processing
  m_includedFiles.clear();
  m_processingStack.clear();

  std::string result = source;
  resolveIncludes(result);
  return result;
}

void ShaderIncluder::resolveIncludes(std::string &source) {
  // Regex to match #include "file" or #include <file>
#ifdef PAIMON_OS_WINDOWS
  // MSVC doesn't support std::regex_constants::multiline
  static const std::regex includePattern(
      R"(^\s*#include\s+([<"])([^>"]+)([>"])\s*(?://.*)?$)");
#else
  static const std::regex includePattern(
      R"(^\s*#include\s+([<"])([^>"]+)([>"])\s*(?://.*)?$)",
      std::regex_constants::multiline);
#endif

  // Process all includes
  std::smatch match;
  while (std::regex_search(source, match, includePattern)) {
    // match[1] = opening delimiter (" or <)
    // match[2] = file path
    // match[3] = closing delimiter (" or >)
    std::string includeFile = match[2].str();

    // Resolve the include path: search in all include paths
    std::filesystem::path filePath(includeFile);
    std::filesystem::path resolvedPath;

    // Try include paths
    auto found = std::find_if(
        m_searchPaths.begin(), m_searchPaths.end(),
        [&](const std::filesystem::path &searchPath) {
          auto fullPath = searchPath / filePath;
          if (std::filesystem::exists(fullPath)) {
            resolvedPath = std::filesystem::canonical(fullPath);
            return true;
          }
          return false;
        });

    if (found == m_searchPaths.end()) {
      LOG_ERROR("Included file not found: {}", includeFile);
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
        LOG_ERROR("Failed to read included file: {}", resolvedPath.string());
        return;
      }

      // Recursively process the included file
      resolveIncludes(includeSource);

      // Build replacement with comments
      replacement =
          "// --- Begin include: " + includeFile + " ---\n" + includeSource;
      if (!includeSource.empty() && includeSource.back() != '\n') {
        replacement += "\n";
      }
      replacement += "// --- End include: " + includeFile + " ---";

      // Remove from processing stack
      m_processingStack.pop_back();
    }

    // Replace the #include directive with the processed content
    source.replace(match.position(), match.length(), replacement);
  }
}
