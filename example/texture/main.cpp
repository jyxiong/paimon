#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform2.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "renderer/texture2d.h"

#include "vertexData.h"
#include "shaderSource.h"

static void errorCallback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

GLFWwindow *window;
GLuint vertexShader, fragmentShader, program;
GLint mvpLocation, positionLocation, colorLocation, uvLocation, textureLocation;

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

void createTexture(const std::string &path)
{
    auto texture = Paimon::Texture2D::LoadFromFile(path);
}

int main()
{
    initOpengl();

    createTexture("../asset/texture/urban.jpg");

    compileShader();

    mvpLocation = glGetUniformLocation(program, "u_mvp");
    positionLocation = glGetAttribLocation(program, "a_pos");
    uvLocation = glGetAttribLocation(program, "a_uv");
    textureLocation= glGetUniformLocation(program, "u_diffuse_texture");

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

            glEnableVertexAttribArray(positionLocation);
            glVertexAttribPointer(positionLocation, 3, GL_FLOAT, false, sizeof(glm::vec3), kPositions);

            glEnableVertexAttribArray(uvLocation);
            glVertexAttribPointer(uvLocation, 2, GL_FLOAT, false, sizeof(glm::vec2), kUvs);

            glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, &mvp[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}
