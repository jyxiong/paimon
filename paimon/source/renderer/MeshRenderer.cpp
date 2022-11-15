#include "MeshRenderer.h"

#include "glad/gl.h"
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/transform2.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "component/Transform.h"
#include "Camera.h"
#include "Material.h"
#include "MeshFilter.h"
#include "Shader.h"
#include "Texture2D.h"

using namespace Paimon;

RTTR_REGISTRATION
{
    rttr::registration::class_<MeshRenderer>("MeshRenderer")
        .constructor<Entity>()(rttr::policy::ctor::as_raw_ptr);
}

MeshRenderer::MeshRenderer(Entity &entity)
    : Component(entity)
{

}

void MeshRenderer::Render()
{
    auto currentCamera = CameraManager::GetCurrentCamera();
    if (currentCamera == nullptr)
    {
        return;
    }

    if ((currentCamera->GetCullingMask() & GetEntity().GetLayer()) == 0){
        return;
    }


    auto view = currentCamera->GetView();
    auto projection = currentCamera->GetProjection();

    auto transform = std::dynamic_pointer_cast<Transform>(GetEntity().GetComponent("Transform"));
    auto translation = glm::translate(transform->GetPosition());
    auto rotation = transform->GetRotation();
    auto eulerAngleYXZ =
        glm::eulerAngleYXZ(glm::radians(rotation.y), glm::radians(rotation.x), glm::radians(rotation.z));
    auto scale = glm::scale(transform->GetScale());
    auto model = translation * scale * eulerAngleYXZ;
    auto mvp = projection * view * model;

    auto meshFilter = std::dynamic_pointer_cast<MeshFilter>(GetEntity().GetComponent("MeshFilter"));

    auto programID = m_material->GetShader()->GetID();

    if (m_vao == 0)
    {
        auto positionLocation = glGetAttribLocation(programID, "a_pos");
        auto colorLocation = glGetAttribLocation(programID, "a_color");
        auto uvLocation = glGetAttribLocation(programID, "a_uv");

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     meshFilter->GetMesh()->vertices.size() * sizeof(Vertex),
                     meshFilter->GetMesh()->vertices.data(),
                     GL_STATIC_DRAW);

        glGenBuffers(1, &m_ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     meshFilter->GetMesh()->indices.size() * sizeof(unsigned short),
                     meshFilter->GetMesh()->indices.data(),
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
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUniformMatrix4fv(glGetUniformLocation(programID, "u_mvp"), 1, GL_FALSE, glm::value_ptr<>(mvp));

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
            glDrawElements(GL_TRIANGLES, meshFilter->GetMesh()->indices.size(), GL_UNSIGNED_SHORT, 0);
        }
        glBindVertexArray(0);
    }
}
