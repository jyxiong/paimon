#include "Texture2D.h"

#include <fstream>
#include <memory>

#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "utils/Application.h"
#include "utils/StopWatch.h"

using namespace Paimon;

Texture2D::Texture2D(int mipmapLevel)
    : m_mipmapLevel(mipmapLevel)
{

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
        data = stbi_load((Application::GetDataPath() / path).string().c_str(), &texture->m_width, &texture->m_height, &numChannels, 0);

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
