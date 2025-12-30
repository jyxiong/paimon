#pragma once

#include <glm/glm.hpp>

namespace paimon {

class Texture;

class ViewportPanel {
public:
  ViewportPanel();
  ~ViewportPanel();

  void onImGuiRender();

  void setTexture(const Texture* texture) { m_texture = texture; }
  
private:
  const Texture* m_texture = nullptr;
  glm::ivec2 m_viewportSize;
};

} // namespace paimon
