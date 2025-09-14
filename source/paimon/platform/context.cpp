#include "paimon/platform/context.h"

#include <cassert>

#include "glad/gl.h"

#include "paimon/core/base/macro.h"

using namespace paimon;

std::once_flag Context::s_glad_flag;

Context::Context() : m_threadId(std::this_thread::get_id()) {}

Context::~Context() {
  if (std::this_thread::get_id() != m_threadId) {
    LOG_ERROR("Context destroyed from wrong thread");
  }
}

void Context::init() {
// IMPORTANT:
// On many platforms/drivers, a newly created OpenGL context (including shared contexts)
// MUST be made current at least once in the thread where it was created before being
// used or made current in other threads. Failing to do so may result in undefined behavior
// or crashes. Always makeCurrent() in the creation thread before passing the context to others.
  makeCurrent();

  loadGLFunctions();
  
  doneCurrent();
}

void Context::loadGLFunctions() {
  std::call_once(s_glad_flag, []() {
    if (!gladLoaderLoadGL()) {
      LOG_ERROR("Failed to load OpenGL functions");
    }

    auto glVersion = glGetString(GL_VERSION);
    auto glRenderer = glGetString(GL_RENDERER);
    auto glVendor = glGetString(GL_VENDOR);
    LOG_INFO("Loaded OpenGL functions");
    LOG_INFO("  OpenGL Version: {}", reinterpret_cast<const char *>(glVersion));
    LOG_INFO("  OpenGL Renderer: {}",
             reinterpret_cast<const char *>(glRenderer));
    LOG_INFO("  OpenGL Vendor: {}", reinterpret_cast<const char *>(glVendor));
  });
}