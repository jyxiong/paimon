#include "Material.h"

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include "rapidxml/rapidxml_utils.hpp"

#include "renderer/Shader.h"
#include "renderer/Texture2D.h"
#include "utils/Application.h"

using namespace Paimon;

void Material::Parse(const std::filesystem::path &path)
{
    rapidxml::file<> xmlFile((Application::s_dataPath / path).string().c_str());
    rapidxml::xml_document<> document;
    document.parse<0>(xmlFile.data());

    rapidxml::xml_node<> *materialNode = document.first_node("material");
    if (materialNode == nullptr)
    {
        return;
    }

    rapidxml::xml_attribute<> *materialShaderAttribute = materialNode->first_attribute("shader");
    if (materialShaderAttribute == nullptr)
    {
        return;
    }

    m_shader = Shader::Find(materialShaderAttribute->value());

    rapidxml::xml_node<> *materialTextureNode = materialNode->first_node("texture");
    while (materialTextureNode != nullptr)
    {
        rapidxml::xml_attribute<> *textureNameAttribute = materialTextureNode->first_attribute("name");
        if (textureNameAttribute == nullptr)
        {
            return;
        }

        rapidxml::xml_attribute<> *textureImageAttribute = materialTextureNode->first_attribute("image");
        if (textureImageAttribute == nullptr)
        {
            return;
        }

        m_textures.emplace(textureNameAttribute->value(), Texture2D::LoadFromFile(textureImageAttribute->value()));

        materialTextureNode = materialTextureNode->next_sibling("texture");
    }

}

void Material::SetUniformMatrix4fv(const std::string &shaderPropertyName, const glm::mat4 &value)
{
    m_uniform_mat4.emplace(shaderPropertyName, value);
}

void Material::SetUniform1i(const std::string &shaderPropertyName, int value)
{
    m_uniform_int.emplace(shaderPropertyName, value);
}

