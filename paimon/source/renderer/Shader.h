#pragma once

#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace Paimon
{
class Shader
{
public:
    Shader() = default;
    ~Shader() = default;

    std::string GetName() { return m_name; }
    [[nodiscard]] unsigned int GetID() const { return m_id; }

    void Parse(const std::filesystem::path &path);
    void CreateProgram(const std::string &vsString, const std::string &fsString);

    static std::shared_ptr<Shader> Find(const std::filesystem::path &path);

private:
    std::string m_name;
    unsigned int m_id{};

    static std::unordered_map<std::string, std::shared_ptr<Shader>> s_shaders;

}; // class Shader
} // namespace Paimon
