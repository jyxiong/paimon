#pragma once

#include <filesystem>
#include <memory>

namespace Paimon
{
struct CPTFileHead
{
    char extension[3];
    int mipmapLevel;
    int width;
    int height;
    int format;
    int size;
}; // struct CPTFileHead

class Texture2D
{
public:
    explicit Texture2D(int mipmapLevel = 0);
    ~Texture2D() = default;

    [[nodiscard]] int GetWidth() const { return m_width; }
    [[nodiscard]] int GetHeight() const { return m_height; }
    [[nodiscard]] unsigned int GetID() const { return m_id; }
    [[nodiscard]] unsigned int GetFormat() const { return m_format; }

    void UpdateSubImage(int x,
                        int y,
                        int width,
                        int height,
                        unsigned int pixelFormat,
                        unsigned int pixelType,
                        unsigned char *data);

    static std::shared_ptr<Texture2D> LoadFromFile(const std::filesystem::path &path);

    static std::shared_ptr<Texture2D> CreateFromTTF(const std::filesystem::path &path, const std::string &word);

    static std::shared_ptr<Texture2D> Create(unsigned short width,
                                             unsigned short height,
                                             int internalFormat,
                                             unsigned int pixelFormat,
                                             unsigned int pixelType,
                                             unsigned char *data);

    static void CompressImageFile(const std::filesystem::path &path, const std::filesystem::path &savePath);

private:
    int m_width{};
    int m_height{};

    int m_mipmapLevel{};
    unsigned int m_id{};
    int m_format{};

}; // class Texture2D
} // namespace Paimon
