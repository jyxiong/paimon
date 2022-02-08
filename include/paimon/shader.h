#pragma once
#include <string>
#include "glm/glm.hpp"

namespace paimon {

    class Shader {
    public:
        Shader() = default;
        Shader(const std::string& vertex_path, const std::string& fragment_path);

        void use() const;
        void set_bool(const std::string& uniform_name, bool value) const;
        void set_int(const std::string& uniform_name, int value) const;
        void set_float(const std::string& uniform_name, float value) const;
        void set_vec2(const std::string& uniform_name, const glm::vec2& value) const;
        void set_vec2(const std::string& uniform_name, float x, float y) const;
        void set_vec3(const std::string& uniform_name, const glm::vec3& value) const;
        void set_vec3(const std::string& uniform_name, float x, float y, float z) const;
        void set_vec4(const std::string& uniform_name, const glm::vec4& value) const;
        void set_vec4(const std::string& uniform_name, float x, float y, float z, float w) const;
        void set_mat2(const std::string& uniform_name, const glm::mat2& mat) const;
        void set_mat3(const std::string& uniform_name, const glm::mat3& mat) const;
        void set_mat4(const std::string& uniform_name, const glm::mat4& mat) const;

    private:
        static std::string read_file(const std::string& file_path);
        static void check_compile_errors(unsigned int id, std::string type);

    private:
        unsigned int m_id;

    }; // class Shader
} // namespace Paimon

