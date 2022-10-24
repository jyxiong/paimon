#pragma once

#include "pch.h"

namespace Paimon
{
class Texture2D
{
public:
    Texture2D() = default;
    ~Texture2D() = default;

    [[nodiscard]] int GetWidth() const { return m_width; }
    [[nodiscard]] int GetHeight() const { return m_height; }
    [[nodiscard]] unsigned int GetId() const { return m_id; }
    [[nodiscard]] unsigned int GetFormat() const { return m_format; }

    static std::shared_ptr<Texture2D> LoadFromFile(const std::string &path);

private:
    int m_width;
    int m_height;

    unsigned int m_id;
    unsigned int m_format;

}; // class Texture2D
} // namespace Paimon
