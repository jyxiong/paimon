#pragma once

#include "paimon/app/layer.h"
#include "paimon/app/panel/menu_panel.h"
#include "paimon/app/panel/scene_panel.h"
#include "paimon/app/panel/viewport_panel.h"

namespace paimon {
class EditorLayer : public Layer {
public:
  EditorLayer();
  ~EditorLayer() override;

  void onAttach() override;
  void onDetach() override;
  void onUpdate() override;
  void onEvent(Event &event) override;
  void onImGuiRender() override;
  
  ViewportPanel& getViewportPanel() { return m_viewportPanel; }
  
private:
  void setupDockingLayout();
  
private:
  MenuPanel m_menuPanel;
  ScenePanel m_scenePanel;
  ViewportPanel m_viewportPanel;
  bool m_firstTime = true;
};
} // namespace paimon