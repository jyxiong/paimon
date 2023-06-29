#include "Texture2D.h"

#include <fstream>
#include <memory>
#include <vector>

#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
#include "utils/Application.h"
#include "utils/StopWatch.h"

using namespace Paimon;

Texture2D::Texture2D(int mipmapLevel)
    : m_mipmapLevel(mipmapLevel)
{

}

void Texture2D::UpdateSubImage(int x,
                               int y,
                               int width,
                               int height,
                               unsigned int pixelFormat,
                               unsigned int pixelType,
                               unsigned char *data)
{
    if (width <= 0 || height <= 0)
    {
        return;
    }

    glBindTexture(GL_TEXTURE_2D, m_id);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, pixelFormat, pixelType, data);
}

std::shared_ptr<Texture2D> Texture2D::LoadFromFile(const std::filesystem::path &path)
{
    auto texture = std::make_shared<Texture2D>();

    stbi_set_flip_vertically_on_load(true);

    StopWatch stopWatch;
    unsigned char *data;
    if (path.extension() == ".cpt")
    {
        stopWatch.Start();

        std::ifstream inputFileStream(Application::GetDataPath() / path, std::ios::in | std::ios::binary);

        CPTFileHead cptFileHead{};
        inputFileStream.read((char *)&cptFileHead, sizeof(CPTFileHead));

        data = (unsigned char *)malloc(cptFileHead.size);
        inputFileStream.read((char *)data, cptFileHead.size);
        inputFileStream.close();

        texture->m_format = cptFileHead.format;
        texture->m_width = cptFileHead.width;
        texture->m_height = cptFileHead.height;

        stopWatch.Stop();
        auto loadFromDiskCost = stopWatch.GetMilliseconds();

        glGenTextures(1, &texture->m_id);
        glBindTexture(GL_TEXTURE_2D, texture->m_id);

        stopWatch.Start();
        glCompressedTexImage2D(GL_TEXTURE_2D,
                               0,
                               texture->m_format,
                               texture->m_width,
                               texture->m_height,
                               0,
                               cptFileHead.size,
                               data);
        stopWatch.Stop();
        auto loadFromMemoryCost = stopWatch.GetMilliseconds();
    } else
    {
        stopWatch.Start();

        int numChannels;
        data = stbi_load((Application::GetDataPath() / path).string().c_str(),
                         &texture->m_width,
                         &texture->m_height,
                         &numChannels,
                         0);

        auto pixelFormat = GL_RGB;
        if (data != nullptr)
        {
            switch (numChannels)
            {
                case 1:
                    pixelFormat = GL_ALPHA;
                    break;
                case 3:
                    pixelFormat = GL_RGB;
                    texture->m_format = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
                    break;
                case 4:
                    pixelFormat = GL_RGBA;
                    texture->m_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
                    break;
                default:
                    break;
            }
        }

        stopWatch.Stop();
        auto loadFromDiskCost = stopWatch.GetMilliseconds();

        glGenTextures(1, &texture->m_id);
        glBindTexture(GL_TEXTURE_2D, texture->m_id);

        stopWatch.Start();
        glTexImage2D(GL_TEXTURE_2D,
                     texture->m_mipmapLevel,
                     texture->m_format,
                     texture->m_width,
                     texture->m_height,
                     0,
                     pixelFormat,
                     GL_UNSIGNED_BYTE,
                     data);
        stopWatch.Stop();
        auto loadFromMemoryCost = stopWatch.GetMilliseconds();

        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return texture;
}

std::shared_ptr<Texture2D> Texture2D::CreateFromTTF(const std::filesystem::path &path, const std::string &word)
{
    FILE *fontFile = fopen((Application::GetDataPath() / path).string().c_str(), "rb");

    // 获取文件大小
    fseek(fontFile, 0, SEEK_END); // 设置文件指针到文件尾，基于文件尾偏移0个字节
    auto fileSize = ftell(fontFile); // 获取文件大小（文件尾 - 文件头，单位：字节）
    fseek(fontFile, 0, SEEK_SET); // 重新设置文件指针到文件头

    // 读取文件内容
    auto fontBuffer = std::vector<unsigned char>(fileSize);
    fread(fontBuffer.data(), fileSize, 1, fontFile);
    fclose(fontFile);

    // 初始化字体
    stbtt_fontinfo fontInfo;
    if (!stbtt_InitFont(&fontInfo, fontBuffer.data(), 0))
    {
        return nullptr;
    }

    // 创建bitmap
    int bitmapWidth = 512;
    int bitmapHeight = 512;
    auto bitmap = std::vector<unsigned char>(bitmapWidth * bitmapHeight * sizeof(unsigned char));

    // 根据指定的行高，计算字体缩放
    float lineHeight = 64; // 字号（字体大小）
    float scale = stbtt_ScaleForPixelHeight(&fontInfo, lineHeight);

    // 获取字体垂直方向的度量，行间距 = ascent - descent + lineGap
    int ascent; // 字体从基线到顶部的高度
    int descent; // 字体从基线到底部的高度，通常为负数
    int lineGap; // 字体之间的间距
    stbtt_GetFontVMetrics(&fontInfo, &ascent, &descent, &lineGap);

    // 根据缩放调整字高
    ascent = roundf(ascent * scale);
    descent = roundf(descent * scale);

    int x = 0; // 位图的x

    for (int i = 0; i < word.size(); ++i)
    {
        // 获取字体水平方向的度量
        int advanceWidth = 0; // 字体宽度
        int leftSideBearing = 0; // 字体左侧位置
        stbtt_GetCodepointHMetrics(&fontInfo, word[i], &advanceWidth, &leftSideBearing);

        // 获取字体边框
        int x1, y1, x2, y2;
        stbtt_GetCodepointBitmapBox(&fontInfo, word[i], scale, scale, &x1, &y1, &x2, &y2);

        // 计算字体的y，不同字体高度不同
        int y = ascent + y1;

        /* render word[i] (stride and offset is important here) */
        int byteOffset = x + roundf(leftSideBearing * scale) + (y * bitmapWidth);
        stbtt_MakeCodepointBitmap(&fontInfo,
                                  bitmap.data() + byteOffset,
                                  x2 - x1,
                                  y2 - y1,
                                  bitmapHeight,
                                  scale,
                                  scale,
                                  word[i]);

        // 调整x
        x += roundf(advanceWidth * scale);

        // 调整字距
        int kern;
        kern = stbtt_GetCodepointKernAdvance(&fontInfo, word[i], word[i + 1]);
        x += roundf(kern * scale);
    }

    //上下翻转
    for (int i = 0; i < bitmapWidth; ++i)
    {
        for (int j = 0; j < bitmapHeight / 2; ++j)
        {
            unsigned char top_char = bitmap[bitmapWidth * j + i];
            unsigned char bottom_char = bitmap[bitmapWidth * (bitmapHeight - j - 1) + i];

            bitmap[bitmapWidth * (bitmapHeight - j - 1) + i] = top_char;
            bitmap[bitmapWidth * j + i] = bottom_char;
        }
    }

    auto texture = std::make_shared<Texture2D>();

    texture->m_format = GL_RED;
    texture->m_width = bitmapWidth;
    texture->m_height = bitmapHeight;

    //1. 通知显卡创建纹理对象，返回句柄;
    glGenTextures(1, &texture->m_id);

    //2. 将纹理绑定到特定纹理目标;
    glBindTexture(GL_TEXTURE_2D, texture->GetID());

    //3. 将图片rgb数据上传到GPU;
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED,
                 texture->m_width,
                 texture->m_height,
                 0,
                 texture->m_format,
                 GL_UNSIGNED_BYTE,
                 bitmap.data());

    //4. 指定放大，缩小滤波方式，线性滤波，即放大缩小的插值方式;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return texture;
}

std::shared_ptr<Texture2D> Texture2D::Create(unsigned short width,
                                             unsigned short height,
                                             int internalFormat,
                                             unsigned int pixelFormat,
                                             unsigned int pixelType,
                                             unsigned char *data)
{
    auto texture = std::make_shared<Texture2D>();

    texture->m_width = width;
    texture->m_height = height;
    texture->m_format = internalFormat;

    glGenTextures(1, &texture->m_id);
    glBindTexture(GL_TEXTURE_2D, texture->m_id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 texture->m_format,
                 texture->m_width,
                 texture->m_height,
                 0,
                 pixelFormat,
                 pixelType,
                 data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    return texture;
}

void Texture2D::CompressImageFile(const std::filesystem::path &path, const std::filesystem::path &savePath)
{
    auto texture = LoadFromFile(path);

    int compressSuccess, compressSize, compressFormat;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressSuccess);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressSize);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &compressFormat);

    void *image = malloc(compressSize);
    glGetCompressedTexImage(GL_TEXTURE_2D, 0, image);

    CPTFileHead cptFileHead{};
    cptFileHead.extension[0] = 'c';
    cptFileHead.extension[1] = 'p';
    cptFileHead.extension[2] = 't';
    cptFileHead.mipmapLevel = texture->m_mipmapLevel;
    cptFileHead.width = texture->m_width;
    cptFileHead.height = texture->m_height;
    cptFileHead.format = compressFormat;
    cptFileHead.size = compressSize;

    std::ofstream outputFileStream(Application::GetDataPath() / savePath, std::ios::out | std::ios::binary);
    outputFileStream.write((char *)&cptFileHead, sizeof(CPTFileHead));
    outputFileStream.write((char *)image, compressSize);
    outputFileStream.close();
}
