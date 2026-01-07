#pragma once

#include <glm/glm.hpp>

namespace paimon {

class ViewportPanel {
public:
  ViewportPanel();
  ~ViewportPanel();

  void onImGuiRender();
    
private:
  glm::ivec2 m_viewportSize = glm::ivec2(0);
  bool m_isRightButtonPressed = false;
  bool m_isMiddleButtonPressed = false;
};

} // namespace paimon
