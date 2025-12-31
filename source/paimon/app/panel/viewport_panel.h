#pragma once

#include <glm/glm.hpp>

namespace paimon {

class ViewportPanel {
public:
  ViewportPanel();
  ~ViewportPanel();

  void onImGuiRender();
  
  bool isFocused() const { return m_isFocused; }
  
private:
  glm::ivec2 m_viewportSize = glm::ivec2(0);
  bool m_isFocused = false;
};

} // namespace paimon
