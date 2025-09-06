#include "paimon/platform/wgl/platform.h"

#include "window.h"

#include "glad/wgl.h"

#include "paimon/core/base/macro.h"

using namespace paimon;

Platform &Platform::instance() {
  static Platform instance;
  return instance;
}

Platform::Platform() {
  Window window;

  PIXELFORMATDESCRIPTOR pixelFormatDesc;
  ZeroMemory(&pixelFormatDesc, sizeof(PIXELFORMATDESCRIPTOR));
  pixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pixelFormatDesc.nVersion = 1;
  pixelFormatDesc.dwFlags = PFD_SUPPORT_OPENGL;
  pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
  pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;

  const auto pixelFormat = ChoosePixelFormat(window.hdc(), &pixelFormatDesc);
  if (pixelFormat == 0) {
    LOG_ERROR("ChoosePixelFormat failed on temporary context");
  }

  auto success = SetPixelFormat(window.hdc(), pixelFormat, &pixelFormatDesc);
  if (!success) {
    LOG_ERROR("SetPixelFormat failed on temporary context");
  }

  const auto dummyContext = wglCreateContext(window.hdc());
  if (dummyContext == nullptr) {
    LOG_ERROR("wglCreateContext failed on temporary context");
  }

  success = wglMakeCurrent(window.hdc(), dummyContext);
  if (!success) {
    wglDeleteContext(dummyContext);
    LOG_ERROR("wglMakeCurrent failed on temporary context");
  }

  // 2. Load wglCreateContextAttribsARB
  if (gladLoaderLoadWGL(window.hdc()) == 0) {
    LOG_ERROR("Failed to load WGL extensions");
  }

  wglMakeCurrent(nullptr, nullptr);
  wglDeleteContext(dummyContext);
}

Platform::~Platform() {}