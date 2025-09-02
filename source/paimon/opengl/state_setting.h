#pragma once

#include <functional>
#include <set>

#include "glad/gl.h"

namespace paimon {

class StateSettingUnified {
public:
  template <typename... Arguments>
  StateSettingUnified(void (*function)(Arguments...), Arguments... arguments)
      : m_functionIdentifier(reinterpret_cast<void *>(function)) {
    m_invoker = [function, arguments...]() { function(arguments...); };
  }

  void apply() {
    if (m_invoker)
      m_invoker();
  }

  bool operator==(const StateSettingUnified &other) const {
    return m_functionIdentifier == other.m_functionIdentifier &&
           m_subtypes == other.m_subtypes;
  }

  std::size_t hash() const { return std::hash<void *>()(m_functionIdentifier); }

  void specializeType(GLenum subtype) { m_subtypes.insert(subtype); }

private:
  void *m_functionIdentifier;
  std::set<GLenum> m_subtypes;
  std::function<void()> m_invoker;
};

} // namespace paimon

namespace std {
template <> struct hash<paimon::StateSettingUnified> {
  size_t operator()(const paimon::StateSettingUnified &setting) const {
    return setting.hash();
  }
};
} // namespace std