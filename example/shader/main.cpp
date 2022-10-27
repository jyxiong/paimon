#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "renderer/MeshFilter.h"
#include "renderer/Shader.h"
#include "renderer/Texture2D.h"

using namespace Paimon;

static void errorCallback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow *window;
GLuint vertexShader, fragmentShader;
GLint mvpLocation, positionLocation, colorLocation, uvLocation, textureLocation;
GLuint kVBO, kEBO;
GLuint kVAO;

std::shared_ptr<Texture2D> texture = nullptr;
std::shared_ptr<MeshFilter> meshFilter = nullptr;

void InitOpengl()
{
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(960, 640, "Simple example", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);
}

void createTexture(const std::filesystem::path &path)
{
    texture = Texture2D::LoadFromFile(path);
}

/// 创建VAO
void GeneratorVertexArrayObject()
{
    glGenVertexArrays(1, &kVAO);
}

void GeneratorBufferObject()
{
    //在GPU上创建缓冲区对象
    glGenBuffers(1, &kVBO);
    //将缓冲区对象指定为顶点缓冲区对象
    glBindBuffer(GL_ARRAY_BUFFER, kVBO);
    //上传顶点数据到缓冲区对象
    glBufferData(GL_ARRAY_BUFFER,
                 meshFilter->GetMesh()->vertices.size() * sizeof(Vertex),
                 meshFilter->GetMesh()->vertices.data(),
                 GL_STATIC_DRAW);

    //在GPU上创建缓冲区对象
    glGenBuffers(1, &kEBO);
    //将缓冲区对象指定为顶点索引缓冲区对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kEBO);
    //上传顶点索引数据到缓冲区对象
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 meshFilter->GetMesh()->indices.size() * sizeof(unsigned short),
                 meshFilter->GetMesh()->indices.data(),
                 GL_STATIC_DRAW);
    //设置VAO
    glBindVertexArray(kVAO);
    {
        //指定当前使用的VBO
        glBindBuffer(GL_ARRAY_BUFFER, kVBO);
        //将Shader变量(a_pos)和顶点坐标VBO句柄进行关联，最后的0表示数据偏移量。
        glVertexAttribPointer(positionLocation, 3, GL_FLOAT, false, sizeof(Vertex), 0);
        //启用顶点Shader属性(a_color)，指定与顶点颜色数据进行关联。
        glVertexAttribPointer(colorLocation, 4, GL_FLOAT, false, sizeof(Vertex), (void *)(sizeof(float) * 3));
        //将Shader变量(a_uv)和顶点UV坐标VBO句柄进行关联。
        glVertexAttribPointer(uvLocation, 2, GL_FLOAT, false, sizeof(Vertex), (void *)(sizeof(float) * (3 + 4)));

        glEnableVertexAttribArray(positionLocation);
        glEnableVertexAttribArray(colorLocation);
        glEnableVertexAttribArray(uvLocation);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kEBO);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

int main()
{
    InitOpengl();

    meshFilter = std::make_shared<MeshFilter>();
    meshFilter->LoadMesh("../asset/model/cube.mesh");

    createTexture("../asset/texture/urban.cpt");

    auto shader = Shader::Find("../asset/shader/unlit");

    mvpLocation = glGetUniformLocation(shader->GetID(), "u_mvp");
    positionLocation = glGetAttribLocation(shader->GetID(), "a_pos");
    colorLocation = glGetAttribLocation(shader->GetID(), "a_color");
    uvLocation = glGetAttribLocation(shader->GetID(), "a_uv");
    textureLocation = glGetUniformLocation(shader->GetID(), "u_diffuse_texture");

    GeneratorVertexArrayObject();
    GeneratorBufferObject();

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        glm::mat4 model, view, projection, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = (float)width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(49.f / 255, 77.f / 255, 121.f / 255, 1.f);

        glm::mat4 trans = glm::translate(glm::vec3(0, 0, 0)); //不移动顶点坐标;

        static float rotate_eulerAngle = 0.f;
        rotate_eulerAngle += 1;
        glm::mat4 rotation = glm::eulerAngleYXZ(glm::radians(rotate_eulerAngle),
                                                glm::radians(rotate_eulerAngle),
                                                glm::radians(rotate_eulerAngle)); //使用欧拉角旋转;

        glm::mat4 scale = glm::scale(glm::vec3(2.0f, 2.0f, 2.0f)); //缩放;
        model = trans * scale * rotation;

        view = glm::lookAt(glm::vec3(0, 0, 10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

        projection = glm::perspective(glm::radians(60.f), ratio, 1.f, 1000.f);

        mvp = projection * view * model;

        glUseProgram(shader->GetID());
        {
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);

            glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

            //贴图设置
            //激活纹理单元0
            glActiveTexture(GL_TEXTURE0);
            //将加载的图片纹理句柄，绑定到纹理单元0的Texture2D上。
            glBindTexture(GL_TEXTURE_2D, texture->GetID());
            //设置Shader程序从纹理单元0读取颜色数据
            glUniform1i(textureLocation, 0);

            glBindVertexArray(kVAO);
            {
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, nullptr);//使用顶点索引进行绘制，最后的0表示数据偏移量。
            }

            glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
