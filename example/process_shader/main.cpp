#include <filesystem>
#include <iostream>


#include "paimon/core/asset_path.h"
#include "paimon/rendering/shader_preprocessor.h"
#include "paimon/rendering/shader_source.h"


using namespace paimon;

int main() {
  // Use shader files from asset folder (using generated constant)
  auto assetShaderDir = std::filesystem::path(ASSET_PATH) / "shader";
  auto commonDir = assetShaderDir / "common";
  auto fragmentDir = assetShaderDir / "fragment";

  // Use the include_demo.frag which demonstrates include functionality
  auto shaderPath = fragmentDir / "include_demo.frag";

  std::cout << "Reading shader from: " << shaderPath << std::endl;

  ShaderSource shaderSource(shaderPath);
  // Add some defines to demonstrate define insertion
  shaderSource.define("USE_LIGHTING");
  shaderSource.define("MAX_LIGHTS", 4);
  shaderSource.define("ENABLE_SHADOWS");

  ShaderPreprocessor preprocessor;
  // 只设置include搜索路径，不依赖shader文件的实际位置
  // 所有include都从这些路径搜索
  preprocessor.addIncludePath(commonDir);
  preprocessor.addIncludePath(fragmentDir);

  try {
    std::cout << "\n=== Processing shader with defines and includes ===\n"
              << std::endl;
    // 不传currentDir，或传空字符串
    // 这样所有include只从addIncludePath设置的路径搜索
    auto processed = preprocessor.processShaderSource(shaderSource);
    std::cout << "--- Processed Shader Output ---\n" << processed << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
