#pragma once

#include <unordered_map>

#include "glad/gl.h"

namespace paimon {
class Capability {

  public:
    Capability(GLenum cap);

    ~Capability() = default;

    void enable();
    void disable();
    bool isEnabled() const;

    void enable(GLuint index);
    void disable(GLuint index);
    bool isEnabled(GLuint index) const;

private:
    GLenum m_cap;
    bool m_enabled = false;
    std::unordered_map<GLuint, bool> m_enabledi;
};
} // namespace paimon