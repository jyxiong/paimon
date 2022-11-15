#include "Pass.h"

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"

#include "renderer/Shader.h"
#include "renderer/Texture2D.h"
#include "utils/Application.h"

using namespace Paimon;

void Pass::Parse(rapidxml::xml_node<>* passNode)
{
    // 解析pass名
    rapidxml::xml_attribute<> *passNameAttribute = passNode->first_attribute("name");
    if (passNameAttribute == nullptr)
    {
        return;
    }

    // 解析shader
    rapidxml::xml_attribute<> *passShaderAttribute = passNode->first_attribute("shader");
    if (passShaderAttribute == nullptr)
    {
        return;
    }

    m_shader = Shader::Find(passShaderAttribute->value());

    // 解析texture
    rapidxml::xml_node<> *passTextureNode = passNode->first_node("texture");
    while (passTextureNode != nullptr)
    {
        rapidxml::xml_attribute<> *textureNameAttribute = passTextureNode->first_attribute("name");
        if (textureNameAttribute == nullptr)
        {
            return;
        }

        rapidxml::xml_attribute<> *textureImageAttribute = passTextureNode->first_attribute("image");
        if (textureImageAttribute == nullptr)
        {
            return;
        }

        m_textures.emplace(textureNameAttribute->value(), Texture2D::LoadFromFile(textureImageAttribute->value()));

        passTextureNode = passTextureNode->next_sibling("texture");
    }

}
