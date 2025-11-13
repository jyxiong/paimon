#pragma once

#include <memory>
#include <vector>

#include "paimon/app/layer.h"
#include "paimon/platform/window.h"

namespace paimon {

class Application {
public:
  Application();
  ~Application();

  void pushLayer(std::unique_ptr<Layer> layer);

  void run();

private:
  std::unique_ptr<Window> m_window;

  std::vector<std::unique_ptr<Layer>> m_layers;

  bool m_running;
};

} // namespace paimon