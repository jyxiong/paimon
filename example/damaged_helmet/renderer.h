#pragma once

#include <filesystem>
#include <map>
#include <memory>

#include <glm/glm.hpp>

#include "paimon/core/fg/transient_resources.h"
#include "paimon/core/sg/material.h"
#include "paimon/core/sg/scene.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/texture.h"
#include "paimon/rendering/render_context.h"

#include "color_pass.h"
#include "final_pass.h"

namespace paimon {

// UBO structures matching shader layout
struct TransformUBO {
  alignas(16) glm::mat4 model;
  alignas(16) glm::mat4 view;
  alignas(16) glm::mat4 projection;
};

struct MaterialUBO {
  alignas(16) glm::vec4 baseColorFactor;
  alignas(16) glm::vec3 emissiveFactor;
  alignas(4) float metallicFactor;
  alignas(4) float roughnessFactor;
  alignas(4) float _padding[3]; // alignment
};

struct LightingUBO {
  alignas(16) glm::vec3 lightPos;
  alignas(4) float _padding1;
  alignas(16) glm::vec3 viewPos;
  alignas(4) float _padding2;
};

// Camera state
struct Camera {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  float yaw = -90.0f;
  float pitch = 0.0f;
  float fov = 45.0f;
};

class Renderer {
public:
  Renderer();
  ~Renderer() = default;

  // Initialize renderer with window dimensions and asset path
  bool initialize(const glm::ivec2 &size, const std::filesystem::path &assetPath);

  // Load glTF model
  bool loadModel(const std::string &modelPath);

  // Set camera parameters
  void setCamera(const Camera &camera);
  Camera &getCamera() { return m_camera; }

  // Set lighting parameters
  void setLightPosition(const glm::vec3 &lightPos);

  // Render a frame
  void render(float deltaTime);

  // Resize framebuffer
  void resize(const glm::ivec2 &newSize);

private:
  // Helper function to create OpenGL texture from sg::Image
  Texture createTextureFromImage(const std::shared_ptr<sg::Image> &image);

  // Setup buffers and textures for loaded model
  void setupModel();

  // Scene and model data
  sg::Scene m_scene;
  std::vector<MeshData> m_meshDataList;
  std::map<std::shared_ptr<sg::Texture>, std::unique_ptr<Texture>> m_textureMap;

  // UBOs
  std::unique_ptr<Buffer> m_transformUBO;
  std::unique_ptr<Buffer> m_materialUBO;
  std::unique_ptr<Buffer> m_lightingUBO;

  // Render context
  std::unique_ptr<RenderContext> m_renderContext;
  
  // Frame graph passes
  std::unique_ptr<ColorPass> m_colorPass;
  std::unique_ptr<FinalPass> m_finalPass;

  // State
  Camera m_camera;
  glm::ivec2 m_size;
  glm::vec3 m_lightPos = glm::vec3(5.0f, 5.0f, 5.0f);
  float m_rotation = 0.0f;
  std::filesystem::path m_assetPath;
};

} // namespace paimon
