#pragma once

#include <filesystem>
#include <unordered_map>
#include <string>
#include <vector>

#include "glm/glm.hpp"

namespace Paimon
{
class Shader;
class Texture2D;
class Material
{
public:
    Material() = default;
    ~Material() = default;

    void Parse(const std::filesystem::path &path);

    void SetUniformMatrix4fv(const std::string &shaderPropertyName, const glm::mat4 &value);
    void SetUniform1i(const std::string &shaderPropertyName, int value);

    std::shared_ptr<Shader> GetShader() { return m_shader; }
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> &GetTextures() { return m_textures; }

private:
    std::shared_ptr<Shader> m_shader;
    std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_textures;

    std::unordered_map<std::string, glm::mat4> m_uniform_mat4;
    std::unordered_map<std::string, int> m_uniform_int;
}; // class Material
} // namespace Paimon
