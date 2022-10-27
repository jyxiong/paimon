#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "renderer/Texture2D.h"

#include "VertexData.h"
#include "ShaderSource.h"

using namespace Paimon;

static void errorCallback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow *window;
GLuint vertexShader, fragmentShader, program;
GLint mvpLocation, positionLocation, colorLocation, uvLocation, textureLocation;
std::shared_ptr<Texture2D> texture = nullptr;
GLuint kVBO, kEBO;
GLuint kVAO;

void initOpengl()
{
    glfwSetErrorCallback(errorCallback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

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

void compileShader()
{
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderText, nullptr);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderText, nullptr);
    glCompileShader(fragmentShader);

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
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
                 vertices.size() * sizeof(Vertex),
                 &vertices[0],
                 GL_STATIC_DRAW);

    //在GPU上创建缓冲区对象
    glGenBuffers(1, &kEBO);
    //将缓冲区对象指定为顶点索引缓冲区对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, kEBO);
    //上传顶点索引数据到缓冲区对象
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 indices.size() * sizeof(unsigned short),
                 &indices[0],
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
    VertexRemoveDuplicate();

    initOpengl();

    std::filesystem::path filePath("../asset/texture/urban.jpg");
    std::filesystem::path savePath("../asset/texture/urban.cpt");
    Texture2D::CompressImageFile(filePath, savePath);

    createTexture("../asset/texture/urban.cpt");

    compileShader();

    mvpLocation = glGetUniformLocation(program, "u_mvp");
    positionLocation = glGetAttribLocation(program, "a_pos");
    colorLocation = glGetAttribLocation(program, "a_color");
    uvLocation = glGetAttribLocation(program, "a_uv");
    textureLocation = glGetUniformLocation(program, "u_diffuse_texture");

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

        glUseProgram(program);
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
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);//使用顶点索引进行绘制，最后的0表示数据偏移量。
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
