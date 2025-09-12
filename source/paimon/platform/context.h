#pragma once

#include <thread>

namespace paimon {

class Context {
public:
  Context();
  virtual ~Context();

  virtual bool destroy() = 0;

  virtual long long nativeHandle() const = 0;

  virtual bool valid() const = 0;

  virtual bool makeCurrent() const = 0;

  virtual bool doneCurrent() const = 0;

protected:
  std::thread::id m_threadId;
};

} // namespace paimon