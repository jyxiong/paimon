#pragma once

#include "paimon/platform/context.h"

#include <windows.h>

namespace paimon {

class WGLContext : public Context {
public:
    WGLContext(HDC hdc, int major = 4, int minor = 6);
    ~WGLContext();

    void makeCurrent() override;
    void doneCurrent() override;

private:
    HDC m_hdc = nullptr;
    HGLRC m_ctx = nullptr;
};

} // namespace paimon