#include "paimon/platform/wgl/context.h"

#include <stdexcept>

#include "glad/wgl.h"

using namespace paimon;

WGLContext::WGLContext(HDC hdc, int major, int minor)
    : m_hdc(hdc)
{
    // 1. Create temp context
    HGLRC temp = wglCreateContext(hdc);
    wglMakeCurrent(hdc, temp);

    // 2. Load wglCreateContextAttribsARB
    if (gladLoaderLoadWGL(hdc) == 0) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(temp);
        throw std::runtime_error("Failed to load WGL extensions");
    }

    int attribs[] = {
        0x2091, major, // WGL_CONTEXT_MAJOR_VERSION_ARB
        0x2092, minor, // WGL_CONTEXT_MINOR_VERSION_ARB
        0x9126, 0x00000001, // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB
        0, 0
    };

    m_ctx = wglCreateContextAttribsARB(hdc, 0, attribs);
    if (!m_ctx) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(temp);
        throw std::runtime_error("Failed to create modern OpenGL context");
    }

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(temp);

    // Make current
    wglMakeCurrent(hdc, m_ctx);
}

WGLContext::~WGLContext() {
    wglMakeCurrent(nullptr, nullptr);
    if (m_ctx) wglDeleteContext(m_ctx);
}

void WGLContext::makeCurrent() {
    wglMakeCurrent(m_hdc, m_ctx);
}

void WGLContext::doneCurrent() {
    wglMakeCurrent(nullptr, nullptr);
}