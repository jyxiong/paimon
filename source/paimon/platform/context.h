#pragma once

#include <mutex>
#include <thread>
#include <memory>

#include "paimon/platform/context_format.h"

namespace paimon {

class Context {
public:
  Context();
  virtual ~Context();

  virtual bool destroy() = 0;

  virtual long long nativeHandle() const = 0;

  virtual bool valid() const = 0;

  virtual bool loadGLFunctions() const = 0;

  virtual bool makeCurrent() const = 0;

  virtual bool doneCurrent() const = 0;

  static std::unique_ptr<Context> getCurrent();

  static std::unique_ptr<Context> create(const ContextFormat &format = {});

  static std::unique_ptr<Context> create(const Context& shared, const ContextFormat &format = {});

private:

  void init();

protected:
  std::thread::id m_threadId;

private:
  static std::once_flag s_gladLoadFlag;
};

} // namespace paimon