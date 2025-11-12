#pragma once

#include <functional>

namespace paimon {
template <class T, class... Args>
void hashCombine(std::size_t &seed, const T &value,
                 const Args &...args) noexcept {
  std::hash<T> hasher;
  seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  if constexpr (sizeof...(args) > 0) {
    hashCombine(seed, args...);
  }
}
} // namespace paimon