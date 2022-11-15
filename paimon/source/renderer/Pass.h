#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include "rapidxml/rapidxml.hpp"

namespace Paimon
{
class Shader;
class Texture2D;

class Pass
{
public:
    Pass() = default;
    ~Pass() = default;

    void Parse(rapidxml::xml_node<>* passNode);

    std::shared_ptr<Shader> GetShader() { return m_shader; }

    std::unordered_map<std::string, std::shared_ptr<Texture2D>> GetTextures() { return m_textures; }
    void SetTexture(const std::string &name, const std::shared_ptr<Texture2D> &texture) { m_textures[name] = texture; }

private:
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;

}; // class Pass

} // namespace Paimon
