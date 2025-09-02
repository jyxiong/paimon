#include "paimon/opengl/state.h"

using namespace paimon;

// https://registry.khronos.org/OpenGL-Refpages/gl4/html/glEnable.xhtml

State::State() {
    std::vector<GLenum> capabilities = {
        GL_BLEND,
        GL_COLOR_LOGIC_OP,
        GL_CULL_FACE,
        GL_DEBUG_OUTPUT,
        GL_DEBUG_OUTPUT_SYNCHRONOUS,
        GL_DEPTH_CLAMP,
        GL_DEPTH_TEST,
        GL_DITHER,
        GL_FRAMEBUFFER_SRGB,
        GL_LINE_SMOOTH,
        GL_MULTISAMPLE,
        GL_POLYGON_OFFSET_FILL,
        GL_POLYGON_OFFSET_LINE,
        GL_POLYGON_OFFSET_POINT,
        GL_POLYGON_SMOOTH,
        GL_PRIMITIVE_RESTART,
        GL_PRIMITIVE_RESTART_FIXED_INDEX,
        GL_PROGRAM_POINT_SIZE,
        GL_RASTERIZER_DISCARD,
        GL_SAMPLE_ALPHA_TO_COVERAGE,
        GL_SAMPLE_ALPHA_TO_ONE,
        GL_SAMPLE_COVERAGE,
        GL_SAMPLE_MASK,
        GL_SCISSOR_TEST,
        GL_STENCIL_TEST,
        GL_TEXTURE_CUBE_MAP_SEAMLESS,
    };

    for (const auto& cap : capabilities) {
        m_capabilities.emplace(cap, std::make_unique<Capability>(cap));
    }
}

void State::enable(GLenum cap) {
  m_capabilities.at(cap)->enable();
}

void State::disable(GLenum cap) {
  m_capabilities.at(cap)->disable();
}

bool State::isEnabled(GLenum cap) const {
  return m_capabilities.at(cap)->isEnabled();
}
