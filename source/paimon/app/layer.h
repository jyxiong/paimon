#pragma once

#include <string>

namespace paimon {
class Layer {
public:
  Layer(const std::string &name = "Layer");
  virtual ~Layer() = default;

  virtual void onAttach() = 0;
  virtual void onDetach() = 0;
  virtual void onUpdate() = 0;
  // virtual void onImGuiRender() {}
  // virtual void onEvent(Event &event) {}

  const std::string &getName() const;

protected:
  std::string m_debugName;
};
} // namespace paimon