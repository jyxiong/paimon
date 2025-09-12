#pragma once

#ifdef _WIN32
#include <windows.h>

namespace paimon {

class WGLPlatform {
public:
  static WGLPlatform &instance();

  ~WGLPlatform();

  HMODULE getModule() const { return m_module; }
  ATOM getId() const { return m_id; }

private:
  WGLPlatform();

  void registerWindowClass();

  void loadExtensions();

private:
  static const TCHAR* s_name;

  HMODULE m_module;
  ATOM m_id;

};

} // namespace paimon

#endif // _WIN32