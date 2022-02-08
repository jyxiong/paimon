#include "paimon/shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "glad/glad.h"

using namespace paimon;

Shader::Shader(const std::string& vert_path, const std::string& frag_path) {
    std::string vert_string = read_file(vert_path);
    std::string frag_string = read_file(frag_path);

    const char* vert_code = vert_string.c_str();
    const char* frag_code = frag_string.c_str();

    unsigned int vert, frag;
    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vert_code, nullptr);
    glCompileShader(vert);
    check_compile_errors(vert, "VERTEX");

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &frag_code, nullptr);
    glCompileShader(frag);
    check_compile_errors(frag, "FRAGMENT");

    // shader Program
    m_id = glCreateProgram();
    glAttachShader(m_id, vert);
    glAttachShader(m_id, frag);
    glLinkProgram(m_id);
    check_compile_errors(m_id, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vert);
    glDeleteShader(frag);
}

void Shader::use() const {
    glUseProgram(m_id);
}

void Shader::set_bool(const std::string& uniform_name, bool value) const {
    glUniform1i(glGetUniformLocation(m_id, uniform_name.c_str()), value);
}

void Shader::set_int(const std::string& uniform_name, int value) const {
    glUniform1i(glGetUniformLocation(m_id, uniform_name.c_str()), value);
}

void Shader::set_float(const std::string& uniform_name, float value) const {
    glUniform1f(glGetUniformLocation(m_id, uniform_name.c_str()), value);
}

void Shader::set_vec2(const std::string& uniform_name, const glm::vec2& value) const {
    glUniform2fv(glGetUniformLocation(m_id, uniform_name.c_str()), 1, &value[0]);
}

void Shader::set_vec2(const std::string& uniform_name, float x, float y) const {
    glUniform2f(glGetUniformLocation(m_id, uniform_name.c_str()), x, y);
}

void Shader::set_vec3(const std::string &uniform_name, const glm::vec3& value) const {
    glUniform3fv(glGetUniformLocation(m_id, uniform_name.c_str()), 1, &value[0]);
}

void Shader::set_vec3(const std::string &uniform_name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(m_id, uniform_name.c_str()), x, y, z);
}

void Shader::set_vec4(const std::string &uniform_name, const glm::vec4& value) const {
    glUniform4fv(glGetUniformLocation(m_id, uniform_name.c_str()), 1, &value[0]);
}

void Shader::set_vec4(const std::string &uniform_name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(m_id, uniform_name.c_str()), x, y, z, w);
}

void Shader::set_mat2(const std::string &uniform_name, const glm::mat2& mat) const {
    glUniformMatrix2fv(glGetUniformLocation(m_id, uniform_name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const std::string &uniform_name, const glm::mat3& mat) const {
    glUniformMatrix3fv(glGetUniformLocation(m_id, uniform_name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const std::string &uniform_name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(m_id, uniform_name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

std::string Shader::read_file(const std::string& file_path) {
    std::string shader_string;
    // file stream throw exceptions
    std::ifstream file_stream;
    file_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open file stream
        file_stream.open(file_path);
        // read string stream
        std::stringstream string_stream;
        string_stream << file_stream.rdbuf();
        // close file stream
        file_stream.close();
        // get string
        shader_string = string_stream.str();
    } catch (std::ifstream::failure& e) {
        std::cout << "Fail to read file: " << file_path << std::endl;
    }

    return shader_string;
}

void Shader::check_compile_errors(unsigned int id, std::string type) {
    int success;
    char info_log[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(id, 1024, nullptr, info_log);
            std::cout << "Fail to compile: " << type << "\n" << info_log << std::endl;
        }
    } else {
        glGetProgramiv(id, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(id, 1024, nullptr, info_log);
            std::cout << "Fail to compile: " << type << "\n" << info_log << std::endl;
        }
    }
}
