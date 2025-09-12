#pragma once

#ifdef _WIN32

#include "paimon/platform/context.h"
#include "paimon/platform/context_format.h"

#include <windows.h>
#include <memory>

namespace paimon {

class WGLContext : public Context {
public:
  WGLContext();
  ~WGLContext();

  bool destroy() override;

  long long nativeHandle() override;

  bool valid() override;

  bool makeCurrent() override;

  bool doneCurrent() override;

  static std::unique_ptr<Context> getCurrent();

  static std::unique_ptr<Context> create(const Context& shared, const ContextFormat &format);

  static std::unique_ptr<Context> create(const ContextFormat &format);

private:
  void createWindow();
  void setPixelFormat() const;
  void createContext(HGLRC shared, const ContextFormat &format);

private:
  HWND m_hwnd = nullptr;
  HDC m_hdc = nullptr;
  HGLRC m_context;
  bool m_owning;
};

} // namespace paimon

#endif // _WIN32