#pragma once

#ifdef PAIMON_PLATFORM_X11

#include "paimon/platform/context.h"

#include <memory>

#include "glad/glx.h"

namespace paimon {
class NativeContext : public Context {
public:
  NativeContext();
  ~NativeContext() override;

  bool destroy() override;

  long long nativeHandle() const override;

  bool valid() const override;

  bool loadGLFunctions() const override;

  bool makeCurrent() const override;

  bool doneCurrent() const override;

  static std::unique_ptr<Context> getCurrent();

  static std::unique_ptr<Context> create(const Context& shared, const ContextFormat &format);

  static std::unique_ptr<Context> create(const ContextFormat &format);

private:
  void createContext(GLXContext shared, const ContextFormat &format);

private:
  GLXDrawable m_drawable;
  GLXContext m_context;
  bool m_owning;
};
} // namespace paimon

#endif // PAIMON_PLATFORM_X11
