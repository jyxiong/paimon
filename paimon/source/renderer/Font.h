#pragma once

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <vector>

#include "freetype/ftglyph.h"

namespace Paimon
{
class Texture2D;

struct Character
{
    float leftTopX{};
    float leftTopY{};
    float rightBottomX{};
    float rightBottomY{};

    Character(float leftTopX, float leftTopY, float rightBottomX, float rightBottomY);

}; // struct Character

class Font
{
public:
    Font() = default;
    ~Font() = default;

    void LoadCharacter(const char &ch);

    std::vector<std::shared_ptr<Character>> LoadStr(const std::string &str);

    std::shared_ptr<Texture2D> GetTexture() { return m_texture; }

    static std::shared_ptr<Font> LoadFromFile(const std::filesystem::path &path, unsigned short size);

    static std::shared_ptr<Font> GetFont(const std::string &name) { return s_fonts[name]; }

private:
    unsigned short m_fontSize{ 20 };
    std::vector<char> m_fontData;

    FT_Library m_ftLibrary{};
    FT_Face m_ftFace{};

    std::shared_ptr<Texture2D> m_texture;
    unsigned short m_textureSize{ 1024 };
    unsigned short m_textureOffsetX{};
    unsigned short m_textureOffsetY{};

    std::unordered_map<char, std::shared_ptr<Character>> m_characters;

    static std::unordered_map<std::string, std::shared_ptr<Font>> s_fonts;
}; // class Font

} // namespace Paimon
