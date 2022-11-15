#include "Font.h"

#include <fstream>

#include "freetype/ftbitmap.h"

#include "utils/Application.h"
#include "utils/Macros.h"
#include "Texture2D.h"

using namespace Paimon;

Character::Character(float leftTopX, float leftTopY, float rightBottomX, float rightBottomY)
    : leftTopX(leftTopX), leftTopY(leftTopY), rightBottomX(rightBottomX), rightBottomY(rightBottomY)
{

}

std::unordered_map<std::string, std::shared_ptr<Font>> Font::s_fonts;

void Font::LoadCharacter(const char &ch)
{
    // 加载这个字的字形到m_ftFace上
    FT_Load_Glyph(m_ftFace, FT_Get_Char_Index(m_ftFace, ch), FT_LOAD_DEFAULT);

    // 从FTFace上面读取字形到ftGlyph;
    FT_Glyph ftGlyph;
    FT_Get_Glyph(m_ftFace->glyph, &ftGlyph);
    // 渲染为256级灰度图
    FT_Glyph_To_Bitmap(&ftGlyph, ft_render_mode_normal, nullptr, 1);

    auto ftBitmapGlyph = (FT_BitmapGlyph)ftGlyph;
    auto &ftBitmap = ftBitmapGlyph->bitmap;

    // 从左上角往右下角填充，满了就换一行
    if (m_textureOffsetX + ftBitmap.width >= m_textureSize)
    {
        m_textureOffsetX = 0;
        m_textureOffsetY += m_fontSize;
    }

    if (m_textureOffsetY + ftBitmap.rows >= m_textureSize)
    {
        return;
    }

    m_texture->UpdateSubImage(m_textureOffsetX,
                              m_textureOffsetY,
                              (int)ftBitmap.width,
                              (int)ftBitmap.rows,
                              GL_RED,
                              GL_UNSIGNED_BYTE,
                              ftBitmap.buffer);

    auto character = std::make_shared<Character>(
        m_textureOffsetX * 1.f / m_textureSize,
        m_textureOffsetY * 1.f / m_textureSize,
        (m_textureOffsetX + ftBitmap.width) * 1.f / m_textureSize,
        (m_textureOffsetY + ftBitmap.rows) * 1.f / m_textureSize);
    m_characters[ch] = character;

    m_textureOffsetX += ftBitmap.width;
}

std::vector<std::shared_ptr<Character>> Font::LoadStr(const std::string &str)
{
    // 生成所有的字符bitmap
    for (auto ch : str)
    {
        LoadCharacter(ch);
    }

    // 返回所有字符信息
    std::vector<std::shared_ptr<Character>> characters;
    for (auto ch : str)
    {
        auto character = m_characters[ch];
        if (character == nullptr)
        {
            CORE_LOG_ERROR("LoadStr error, no bitmap, ch:{0}", ch)
            continue;
        }
        characters.push_back(character);
    }
    return characters;
}

std::shared_ptr<Font> Font::LoadFromFile(const std::filesystem::path &path, unsigned short fontSize)
{
    auto font = GetFont(path.string());
    if (font != nullptr)
        return font;

    std::ifstream inputStream(Application::GetDataPath() / path, std::ios::in | std::ios::binary);

    // 获取文件大小
    inputStream.seekg(0, std::ios::end); // 设置文件指针到文件尾，基于文件尾偏移0个字节
    auto fileSize = inputStream.tellg(); // 获取文件大小（文件尾 - 文件头，单位：字节）
    inputStream.seekg(0, std::ios::beg); // 重新设置文件指针到文件头

    // 读取文件内容
    auto fontBuffer = std::vector<char>(fileSize);
    inputStream.read(fontBuffer.data(), fileSize);

    // 将ttf传入FreeType解析
    FT_Library ftLibrary = nullptr;
    FT_Face ftFace = nullptr;
    FT_Init_FreeType(&ftLibrary);

    FT_Error
        error = FT_New_Memory_Face(ftLibrary, reinterpret_cast<FT_Byte *>(fontBuffer.data()), fileSize, 0, &ftFace);
    if (error != 0)
        return nullptr;

    FT_Select_Charmap(ftFace, FT_ENCODING_UNICODE);

    FT_Set_Char_Size(ftFace, (FT_F26Dot6)(fontSize * (1 << 6)), 0, 72, 72);

    if (ftFace == nullptr)
    {
        CORE_LOG_ERROR("FT_Set_Char_Size error!")
        return nullptr;
    }

    // 创建Font，保存Freetype解析字体结果
    font = std::make_shared<Font>();
    font->m_fontSize = fontSize;
    font->m_fontData = std::move(fontBuffer);
    font->m_ftLibrary = ftLibrary;
    font->m_ftFace = ftFace;
    s_fonts[path.string()] = font;

    auto pixels = std::vector<unsigned char>(font->m_textureSize * font->m_textureSize, 0);
    font->m_texture =
        Texture2D::Create(font->m_textureSize, font->m_textureSize, GL_RED, GL_RED, GL_UNSIGNED_BYTE, pixels.data());

    return font;
}
