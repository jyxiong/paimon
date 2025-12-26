#pragma once

#include "paimon/app/layer.h"

namespace paimon {

class ImGuiLayer : public Layer {
public:
  ImGuiLayer(const std::string &name);

  ~ImGuiLayer() = default;

  void onAttach() override;
  void onDetach() override;
  void onUpdate() override;
  void onEvent(Event &event) override;
  void onImGuiRender() override;

  void begin();
  void end();

private:
  void showDockSpaceBegin();

  void showDockSpaceEnd();

  void showDockingDisabledMessage();

private:
  static bool s_showDockSpace;
};
} // namespace paimon