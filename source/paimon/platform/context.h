#pragma once

#include <mutex>
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

  void init();

private:
  static void loadGLFunctions();

protected:
  std::thread::id m_threadId;

private:
  static std::once_flag s_glad_flag;
};

} // namespace paimon