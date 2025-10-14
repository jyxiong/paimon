#pragma once

#ifdef PAIMON_PLATFORM_WIN32

#include "paimon/platform/context.h"

#include <windows.h>
#include <memory>

namespace paimon {

class NativeContext : public Context {
public:
  NativeContext();
  ~NativeContext();

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