#include "Shader.h"

#include <fstream>
#include "glad/gl.h"

using namespace Paimon;

std::unordered_map<std::string, std::shared_ptr<Shader>> Shader::s_shaders;

void Shader::Parse(const std::filesystem::path &path)
{
    m_name = path.filename().string();

    auto vsPath = path.string() + ".vs";
    auto fsPath = path.string() + ".fs";

    std::ifstream vsStream(vsPath);
    std::string vs((std::istreambuf_iterator<char>(vsStream)), std::istreambuf_iterator<char>());

    std::ifstream fsStream(fsPath);
    std::string fs((std::istreambuf_iterator<char>(fsStream)), std::istreambuf_iterator<char>());

    CreateProgram(vs, fs);
}

void Shader::CreateProgram(const std::string &vsString, const std::string &fsString)
{
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    const char *vsSource[] = { vsString.c_str() };
    glShaderSource(vs, 1, vsSource, nullptr);
    glCompileShader(vs);
    int compile_status = GL_FALSE;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE)
    {
        char message[256];
        glGetShaderInfoLog(vs, sizeof(message), nullptr, message);
    }

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fsSource[] = { fsString.c_str() };
    glShaderSource(fs, 1, fsSource, nullptr);
    glCompileShader(fs);

    compile_status = GL_FALSE;
    glGetShaderiv(fs, GL_COMPILE_STATUS, &compile_status);
    if (compile_status == GL_FALSE)
    {
        char message[256];
        glGetShaderInfoLog(fs, sizeof(message), nullptr, message);
    }

    m_id = glCreateProgram();
    glAttachShader(m_id, vs);
    glAttachShader(m_id, fs);
    glLinkProgram(m_id);

    int link_status = GL_FALSE;
    glGetProgramiv(m_id, GL_LINK_STATUS, &link_status);
    if (link_status == GL_FALSE)
    {
        char message[256];
        glGetProgramInfoLog(m_id, sizeof(message), nullptr, message);
    }
}

std::shared_ptr<Shader> Shader::Find(const std::filesystem::path &path)
{
    auto iter = s_shaders.find(path.filename().string());
    if (iter != s_shaders.end())
        return iter->second;

    auto shader = std::make_shared<Shader>();
    shader->Parse(path);

    return shader;
}
