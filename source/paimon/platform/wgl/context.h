#pragma once

#ifdef _WIN32

#include "paimon/platform/context.h"
#include "paimon/platform/context_format.h"

#include <windows.h>
#include <memory>

#include "paimon/platform/wgl/window.h"

namespace paimon {

class WGLContext : public Context {
public:
  WGLContext();
  ~WGLContext();

//   std::unique_ptr<Context> getCurrent() override;

//   std::unique_ptr<Context> create(const ContextFormat &format);
//   std::unique_ptr<Context> create(const Context *shared,
//                                   const ContextFormat &format) override;

  bool destroy() override;

  long long nativeHandle() override;

  bool valid() override;

  bool makeCurrent() override;

  bool doneCurrent() override;

  static std::unique_ptr<Context> getCurrent();

  static std::unique_ptr<Context> create(const ContextFormat &format);

private:
  void setPixelFormat() const;
  void createContext(HGLRC shared, const ContextFormat &format);

private:
  std::unique_ptr<Window> m_window;

  HGLRC m_contextHandle;

  bool m_owning;
};

} // namespace paimon

#endif // _WIN32