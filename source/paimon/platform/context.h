#pragma once

namespace paimon {

class Context {
public:
  Context() = default;
  virtual ~Context() = default;

  // virtual std::unique_ptr<Context> getCurrent() = 0;
  // virtual std::unique_ptr<Context> create(const ContextFormat &format) = 0;
  // virtual std::unique_ptr<Context> create(const Context *shared,
  //                                         const ContextFormat &format) = 0;

  virtual bool destroy() = 0;

  virtual long long nativeHandle() = 0;

  virtual bool valid() = 0;

  virtual bool makeCurrent() = 0;

  virtual bool doneCurrent() = 0;
};

} // namespace paimon