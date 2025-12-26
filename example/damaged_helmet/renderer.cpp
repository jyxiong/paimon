#include "renderer.h"
#include "paimon/core/ecs/entity.h"
#include "paimon/core/ecs/scene.h"
#include <memory>

using namespace paimon;

Renderer::Renderer()
    : Layer("Renderer"), m_renderContext(std::make_unique<RenderContext>()),
      m_scene(std::make_unique<ecs::Scene>()),
      m_color_pass(*m_renderContext), m_final_pass(*m_renderContext) {}

void Renderer::onUpdate() {
  // Update rotation for auto-rotating camera

  // First Pass: Render to FBO
  m_color_pass.draw(*m_renderContext, m_resolution, *m_scene);

  // Second Pass: Render FBO texture to screen
  m_final_pass.draw(*m_renderContext, *m_color_pass.getColorTexture(),
                    m_resolution);
}

void Renderer::onResize(int32_t width, int32_t height) {
  m_resolution = glm::ivec2(width, height);
}

ecs::Entity Renderer::loadScene(const std::filesystem::path &scenePath) {
  return m_scene->load(scenePath);
}