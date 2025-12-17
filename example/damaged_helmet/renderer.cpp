#include "renderer.h"

using namespace paimon;

Renderer::Renderer(RenderContext &renderContext,
                   const std::map<std::shared_ptr<sg::Texture>, Texture *> &texturePtrMap)
    : m_renderContext(renderContext),
      m_colorPass(renderContext),
      m_finalPass(renderContext),
      m_transformUbo(),
      m_materialUbo(),
      m_lightingUbo(),
      m_texturePtrMap(texturePtrMap) {
  m_transformUbo.set_storage(sizeof(TransformUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
  m_materialUbo.set_storage(sizeof(MaterialUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
  m_lightingUbo.set_storage(sizeof(LightingUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
}

void Renderer::draw(const glm::ivec2 &resolution, const Camera &camera,
                    const LightingUBO &lighting,
                    const std::vector<MeshData> &meshes) {
  // Prepare transform UBO
  TransformUBO transformData;
  transformData.model = glm::mat4(1.0f);
  transformData.view = glm::lookAt(camera.position, glm::vec3(0.0f, 0.0f, 0.0f), camera.up);
  transformData.projection = glm::perspective(glm::radians(camera.fov),
                                              static_cast<float>(resolution.x) / resolution.y,
                                              0.1f, 100.0f);
  m_transformUbo.set_sub_data(0, sizeof(TransformUBO), &transformData);

  // Update lighting UBO (copy viewPos from camera)
  LightingUBO lightingData = lighting;
  lightingData.viewPos = camera.position;
  m_lightingUbo.set_sub_data(0, sizeof(LightingUBO), &lightingData);

  // First pass: render scene to color texture
  m_colorPass.draw(m_renderContext, resolution, meshes, m_texturePtrMap, m_transformUbo, m_materialUbo, m_lightingUbo);

  // Second pass: blit color texture to default framebuffer
  m_finalPass.draw(m_renderContext, *m_colorPass.getColorTexture(), resolution);
}