#include "texture2d.h"

#include "glad/gl.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace Paimon;

std::shared_ptr<Texture2D> Texture2D::LoadFromFile(const std::string &path)
{
    auto texture = std::make_shared<Texture2D>();

    stbi_set_flip_vertically_on_load(true);

    int numChannels;
    unsigned char *data = stbi_load(path.c_str(), &texture->m_width, &texture->m_height, &numChannels, 0);
    if (data != nullptr)
    {
        switch (numChannels)
        {
            case 1:texture->m_format = GL_ALPHA;
                break;
            case 3:texture->m_format = GL_RGB;
                break;
            case 4:texture->m_format = GL_RGBA;
                break;
            default:break;
        }
    }

    glGenTextures(1, &texture->m_id);
    glBindTexture(GL_TEXTURE_2D, texture->m_id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_COMPRESSED_RGB,
                 texture->m_width,
                 texture->m_height,
                 0,
                 texture->m_format,
                 GL_UNSIGNED_BYTE,
                 data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return texture;
}
