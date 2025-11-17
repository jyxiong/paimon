#include "paimon/app/layer.h"

namespace paimon {
Layer::Layer(const std::string &name) : m_debugName(name) {}

const std::string &Layer::getName() const { return m_debugName; }
} // namespace paimon