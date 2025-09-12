#pragma once

namespace paimon {

class Context {
public:
  Context() = default;
  virtual ~Context() = default;

  virtual bool destroy() = 0;

  virtual long long nativeHandle() = 0;

  virtual bool valid() = 0;

  virtual bool makeCurrent() = 0;

  virtual bool doneCurrent() = 0;
};

} // namespace paimon