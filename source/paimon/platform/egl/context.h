#pragma once

#ifdef __linux__

#include "paimon/platform/context.h"

#include <memory>

#include "glad/egl.h"

#include "paimon/platform/context_format.h"

namespace paimon {

class EglContext : public Context {
public:
  EglContext();
  ~EglContext() override;

  bool destroy() override;
  long long nativeHandle() override;
  bool valid() override;
  bool makeCurrent() override;
  bool doneCurrent() override;

  static std::unique_ptr<Context> getCurrent();
  static std::unique_ptr<Context> create(const ContextFormat &format);

private:
  void createContext(EGLContext shared, const ContextFormat &format);

private:
  EGLContext m_contextHandle;
  bool m_owning;
};

} // namespace paimon

#endif // __linux__