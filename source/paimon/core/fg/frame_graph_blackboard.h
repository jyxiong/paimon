#pragma once

#include <any>
#include <typeindex>
#include <unordered_map>

namespace paimon {
class FrameGraphBlackboard {
public:
  FrameGraphBlackboard() = default;
  FrameGraphBlackboard(const FrameGraphBlackboard &) = delete;
  FrameGraphBlackboard(FrameGraphBlackboard &&) noexcept = delete;

  FrameGraphBlackboard &operator=(const FrameGraphBlackboard &) = delete;
  FrameGraphBlackboard &operator=(FrameGraphBlackboard &&) noexcept = delete;

  template <class T, class... Args>
  T &set(Args &&...args) {
    auto type = std::type_index(typeid(T));
    m_data[type] = T(std::forward<Args>(args)...);
    return std::any_cast<T &>(m_data.at(type));
  }

  template <class T>
  T &get() {
    auto type = std::type_index(typeid(T));
    return std::any_cast<T &>(m_data.at(type));
  }

  template <class T>
  const T &get() const {
    auto type = std::type_index(typeid(T));
    return std::any_cast<const T &>(m_data.at(type));
  }

  template <class T>
  bool has() const {
    auto type = std::type_index(typeid(T));
    return m_data.contains(type);
  }

private:
  std::unordered_map<std::type_index, std::any> m_data;
};
} // namespace paimon