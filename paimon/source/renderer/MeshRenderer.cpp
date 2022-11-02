#include "MeshRenderer.h"

#include "glad/gl.h"
#include "glm/gtc/type_ptr.hpp"

#include "renderer/Material.h"
#include "renderer/MeshFilter.h"
#include "renderer/Shader.h"
#include "renderer/Texture2D.h"

using namespace Paimon;

void MeshRenderer::Render()
{
    auto programID = m_material->GetShader()->GetID();

    if (m_vao == 0)
    {
        auto positionLocation = glGetAttribLocation(programID, "a_pos");
        auto colorLocation = glGetAttribLocation(programID, "a_color");
        auto uvLocation = glGetAttribLocation(programID, "a_uv");

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     m_meshFilter->GetMesh()->vertices.size() * sizeof(Vertex),
                     m_meshFilter->GetMesh()->vertices.data(),
                     GL_STATIC_DRAW);

        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     m_meshFilter->GetMesh()->indices.size() * sizeof(unsigned short),
                     m_meshFilter->GetMesh()->indices.data(),
                     GL_STATIC_DRAW);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
            glEnableVertexAttribArray(positionLocation);

            glVertexAttribPointer(colorLocation, 4, GL_FLOAT, false, sizeof(Vertex), (void *)(sizeof(float) * 3));
            glEnableVertexAttribArray(colorLocation);

            glVertexAttribPointer(uvLocation, 2, GL_FLOAT, false, sizeof(Vertex), (void *)(sizeof(float) * (3 + 4)));
            glEnableVertexAttribArray(uvLocation);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glUseProgram(programID);
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glUniformMatrix4fv(glGetUniformLocation(programID, "u_mvp"), 1, GL_FALSE, glm::value_ptr<>(m_mvp));

        int textureIndex = 0;
        for (auto &texture : m_material->GetTextures())
        {
            auto textureLocation = glGetUniformLocation(programID, texture.first.c_str());
            glActiveTexture(GL_TEXTURE0 + textureIndex);
            glBindTexture(GL_TEXTURE_2D, texture.second->GetID());
            glUniform1i(textureLocation, textureIndex);

            textureIndex++;
        }

        glBindVertexArray(m_vao);
        {
            glDrawElements(GL_TRIANGLES, m_meshFilter->GetMesh()->indices.size(), GL_UNSIGNED_SHORT, 0);
        }
        glBindVertexArray(0);
    }
}
