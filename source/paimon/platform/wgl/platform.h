#pragma once


#ifdef _WIN32
#include <windows.h>

namespace paimon {

class WindowClassRegistrar {
public:
  static WindowClassRegistrar &instance();

  ~WindowClassRegistrar();

  HMODULE getModule() const { return m_module; }
  ATOM getId() const { return m_id; }

private:
  WindowClassRegistrar();

private:
  static const TCHAR* s_name;

  HMODULE m_module;
  ATOM m_id;

};

class WGLExtensionLoader {
public:
  static WGLExtensionLoader& instance();

  ~WGLExtensionLoader() = default;

private:
  WGLExtensionLoader();
};

} // namespace paimon

#endif // _WIN32