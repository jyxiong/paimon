#pragma once

#include <memory>
#include <string>

namespace Paimon
{
class Texture2D
{
public:
    explicit Texture2D(int mipmapLevel = 0);
    ~Texture2D() = default;

    [[nodiscard]] int GetWidth() const { return m_width; }
    [[nodiscard]] int GetHeight() const { return m_height; }
    [[nodiscard]] unsigned int GetID() const { return m_id; }
    [[nodiscard]] unsigned int GetFormat() const { return m_format; }

    static std::shared_ptr<Texture2D> LoadFromFile(const std::string &path);
    static void CompressImageFile(const std::string &path, const std::string &savePath);

private:
    int m_width{};
    int m_height{};

    int m_mipmapLevel{};
    unsigned int m_id{};
    int m_format{};

    struct CPTFileHead
    {
        char extension[3];
        int mipmapLevel;
        int width;
        int height;
        int format;
        int size;
    }; // struct CPTFileHead

}; // class Texture2D
} // namespace Paimon
