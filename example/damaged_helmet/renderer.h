#pragma once

#include <vector>
#include <map>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "paimon/rendering/render_context.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/texture.h"

#include "color_pass.h"
#include "final_pass.h"
#include "mesh_data.h"

namespace paimon {

// Camera struct used by Renderer (matches previous main.cpp fields)
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
  Renderer(RenderContext &renderContext,
           const std::map<std::shared_ptr<sg::Texture>, Texture *> &texturePtrMap);

  // resolution: framebuffer size in pixels
  // camera: camera state (position, fov...)
  // lighting: LightingUBO structure (lightPos, viewPos)
  // meshes: list of MeshData to render
  void draw(const glm::ivec2 &resolution, const Camera &camera,
            const LightingUBO &lighting,
            const std::vector<MeshData> &meshes);

  // expose color texture for tests or external use if needed
  const Texture *getColorTexture() const { return m_colorPass.getColorTexture(); }

private:
  RenderContext &m_renderContext;
  ColorPass m_colorPass;
  FinalPass m_finalPass;

  Buffer m_transformUbo;
  Buffer m_materialUbo;
  Buffer m_lightingUbo;

  const std::map<std::shared_ptr<sg::Texture>, Texture *> &m_texturePtrMap;
};

} // namespace paimon
