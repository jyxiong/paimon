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
  long long nativeHandle() const override;
  bool valid() const override;
  bool makeCurrent() const override;
  bool doneCurrent() const override;

  static std::unique_ptr<Context> getCurrent();
  static std::unique_ptr<Context> create(const ContextFormat &format);

private:
  void createContext(EGLContext shared, const ContextFormat &format);

private:
  EGLSurface m_surface;
  EGLContext m_context;
  bool m_owning;
};

} // namespace paimon

#endif // __linux__