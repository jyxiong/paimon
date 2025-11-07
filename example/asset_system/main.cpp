#include "paimon/asset/asset_manager.h"
#include "paimon/asset/shader_source.h"
#include "paimon/core/log_system.h"

using namespace paimon;

/**
 * @brief Example demonstrating the asset loading and caching system
 * 
 * This example shows:
 * - Loading shader assets through AssetManager
 * - Automatic caching and memory management
 * - Adding preprocessor defines to shaders
 * - Cache statistics
 * - Asset reloading
 */

void printSeparator(const std::string &title) {
  LOG_INFO("");
  LOG_INFO("{}", std::string(60, '='));
  LOG_INFO("  {}", title);
  LOG_INFO("{}", std::string(60, '='));
  LOG_INFO("");
}

int main() {
  LogSystem::init();

  printSeparator("Asset System Demo");

  // Get asset manager instance
  auto &assetManager = AssetManager::getInstance();

  // Configure cache size (256MB)
  assetManager.setMaxCacheSize(256 * 1024 * 1024);

  // Setup shader paths
  auto assetShaderDir =
      std::filesystem::current_path().parent_path() / "asset" / "shader";
  auto vertexShaderPath = assetShaderDir / "vertex" / "basic.vert";
  auto fragmentShaderPath = assetShaderDir / "fragment" / "color.frag";

  // Check if files exist
  if (!std::filesystem::exists(vertexShaderPath)) {
    LOG_ERROR("Vertex shader not found: {}", vertexShaderPath.string());
    return 1;
  }
  if (!std::filesystem::exists(fragmentShaderPath)) {
    LOG_ERROR("Fragment shader not found: {}", fragmentShaderPath.string());
    return 1;
  }

  printSeparator("Loading Shaders");

  // Load vertex shader
  auto vertexShader = assetManager.load<ShaderSource>(vertexShaderPath);
  if (!vertexShader) {
    LOG_ERROR("Failed to load vertex shader");
    return 1;
  }
  LOG_INFO("Vertex shader loaded successfully");
  LOG_INFO("  Path: {}", vertexShader->getPath().string());
  LOG_INFO("  Memory size: {} bytes", vertexShader->getMemorySize());
  LOG_INFO("  Source lines: {}", 
           std::count(vertexShader->getSource().begin(), 
                     vertexShader->getSource().end(), '\n'));

  // Load fragment shader
  auto fragmentShader = assetManager.load<ShaderSource>(fragmentShaderPath);
  if (!fragmentShader) {
    LOG_ERROR("Failed to load fragment shader");
    return 1;
  }
  LOG_INFO("Fragment shader loaded successfully");
  LOG_INFO("  Path: {}", fragmentShader->getPath().string());
  LOG_INFO("  Memory size: {} bytes", fragmentShader->getMemorySize());

  printSeparator("Cache Statistics");

  auto stats = assetManager.getCacheStats();
  LOG_INFO("Total assets in cache: {}", stats.totalAssets);
  LOG_INFO("Total memory used: {} bytes ({:.2f} MB)", 
           stats.totalMemoryUsed,
           stats.totalMemoryUsed / (1024.0 * 1024.0));
  LOG_INFO("Maximum cache size: {} bytes ({:.2f} MB)", 
           stats.maxMemory,
           stats.maxMemory / (1024.0 * 1024.0));
  
  for (const auto &[type, count] : stats.assetCountByType) {
    LOG_INFO("  Type {}: {} assets", static_cast<int>(type), count);
  }

  printSeparator("Testing Cache Hit");

  // Load the same shader again - should hit cache
  auto vertexShader2 = assetManager.load<ShaderSource>(vertexShaderPath);
  if (vertexShader2) {
    LOG_INFO("Vertex shader loaded from cache (same pointer: {})", 
             vertexShader.get() == vertexShader2.get());
  }

  printSeparator("Adding Preprocessor Defines");

  // Create a new instance with defines
  auto fragmentShaderWithDefines = assetManager.load<ShaderSource>(
    fragmentShaderPath, true // Force reload
  );
  
  if (fragmentShaderWithDefines) {
    fragmentShaderWithDefines->define("ENABLE_GAMMA_CORRECTION");
    fragmentShaderWithDefines->define("GAMMA_VALUE", 2.2f);
    
    LOG_INFO("Added defines to fragment shader:");
    for (const auto &define : fragmentShaderWithDefines->getDefines()) {
      LOG_INFO("  #define {}", define);
    }
  }

  printSeparator("Testing Asset Reload");

  if (assetManager.isCached(vertexShaderPath)) {
    LOG_INFO("Vertex shader is currently cached");
    
    if (assetManager.reload(vertexShaderPath)) {
      LOG_INFO("Vertex shader reloaded successfully");
    }
  }

  printSeparator("Unloading Specific Assets");

  // Unload fragment shader
  assetManager.unload(fragmentShaderPath);
  LOG_INFO("Fragment shader unloaded");
  LOG_INFO("Is still cached: {}", assetManager.isCached(fragmentShaderPath));

  // Check updated stats
  stats = assetManager.getCacheStats();
  LOG_INFO("Assets remaining in cache: {}", stats.totalAssets);

  printSeparator("Cleanup");

  // Unload all assets
  assetManager.unloadAll();
  LOG_INFO("All assets unloaded");

  stats = assetManager.getCacheStats();
  LOG_INFO("Final cache size: {} bytes", stats.totalMemoryUsed);

  printSeparator("Summary");

  LOG_INFO("Asset system features demonstrated:");
  LOG_INFO("  ✓ Automatic asset loading and caching");
  LOG_INFO("  ✓ Memory tracking and statistics");
  LOG_INFO("  ✓ Cache hit detection");
  LOG_INFO("  ✓ Preprocessor defines for shaders");
  LOG_INFO("  ✓ Asset reloading");
  LOG_INFO("  ✓ Selective and bulk unloading");
  LOG_INFO("");
  LOG_INFO("Future extensions ready:");
  LOG_INFO("  - Image assets (PNG, JPG, HDR, etc.)");
  LOG_INFO("  - Model assets (OBJ, GLTF, FBX, etc.)");
  LOG_INFO("  - LRU cache eviction");
  LOG_INFO("  - Async loading support");

  return 0;
}
