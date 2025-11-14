#include <iostream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "paimon/app/window.h"
#include "paimon/core/fg/frame_graph.h"
#include "paimon/core/fg/frame_graph_texture.h"
#include "paimon/core/fg/transient_resources.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/framebuffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/shader.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/rendering/render_context.h"

using namespace paimon;

// Vertex structure
struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

// Shadow map shader (depth only)
const char *shadow_vertex_shader = R"(
#version 460 core
layout(location = 0) in vec3 aPos;

uniform mat4 uLightSpaceMatrix;
uniform mat4 uModel;

void main() {
  gl_Position = uLightSpaceMatrix * uModel * vec4(aPos, 1.0);
}
)";

const char *shadow_fragment_shader = R"(
#version 460 core
void main() {
  // Depth is automatically written
}
)";

// Scene rendering shader
const char *scene_vertex_shader = R"(
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform mat4 uLightSpaceMatrix;

void main() {
  FragPos = vec3(uModel * vec4(aPos, 1.0));
  Normal = mat3(transpose(inverse(uModel))) * aNormal;
  FragPosLightSpace = uLightSpaceMatrix * vec4(FragPos, 1.0);
  gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}
)";

const char *scene_fragment_shader = R"(
#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uObjectColor;
uniform sampler2D uShadowMap;

float ShadowCalculation(vec4 fragPosLightSpace) {
  vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
  projCoords = projCoords * 0.5 + 0.5;
  
  if(projCoords.z > 1.0)
    return 0.0;
    
  float closestDepth = texture(uShadowMap, projCoords.xy).r;
  float currentDepth = projCoords.z;
  
  vec3 normal = normalize(Normal);
  vec3 lightDir = normalize(uLightPos - FragPos);
  float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
  
  float shadow = 0.0;
  vec2 texelSize = 1.0 / textureSize(uShadowMap, 0);
  for(int x = -1; x <= 1; ++x) {
    for(int y = -1; y <= 1; ++y) {
      float pcfDepth = texture(uShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
      shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
    }
  }
  shadow /= 9.0;
  
  return shadow;
}

void main() {
  vec3 color = uObjectColor;
  vec3 normal = normalize(Normal);
  vec3 lightColor = vec3(1.0);
  
  // Ambient
  vec3 ambient = 0.2 * lightColor;
  
  // Diffuse
  vec3 lightDir = normalize(uLightPos - FragPos);
  float diff = max(dot(lightDir, normal), 0.0);
  vec3 diffuse = diff * lightColor;
  
  // Specular
  vec3 viewDir = normalize(uViewPos - FragPos);
  vec3 halfwayDir = normalize(lightDir + viewDir);
  float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
  vec3 specular = spec * lightColor;
  
  // Shadow
  float shadow = ShadowCalculation(FragPosLightSpace);
  vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
  
  FragColor = vec4(lighting, 1.0);
}
)";

// Generate cube vertices
std::vector<Vertex> generateCubeVertices() {
  std::vector<Vertex> vertices;

  // Cube vertices with normals
  glm::vec3 positions[] = {
      // Front face
      {-0.5f, -0.5f, 0.5f},
      {0.5f, -0.5f, 0.5f},
      {0.5f, 0.5f, 0.5f},
      {0.5f, 0.5f, 0.5f},
      {-0.5f, 0.5f, 0.5f},
      {-0.5f, -0.5f, 0.5f},
      // Back face
      {0.5f, -0.5f, -0.5f},
      {-0.5f, -0.5f, -0.5f},
      {-0.5f, 0.5f, -0.5f},
      {-0.5f, 0.5f, -0.5f},
      {0.5f, 0.5f, -0.5f},
      {0.5f, -0.5f, -0.5f},
      // Top face
      {-0.5f, 0.5f, 0.5f},
      {0.5f, 0.5f, 0.5f},
      {0.5f, 0.5f, -0.5f},
      {0.5f, 0.5f, -0.5f},
      {-0.5f, 0.5f, -0.5f},
      {-0.5f, 0.5f, 0.5f},
      // Bottom face
      {-0.5f, -0.5f, -0.5f},
      {0.5f, -0.5f, -0.5f},
      {0.5f, -0.5f, 0.5f},
      {0.5f, -0.5f, 0.5f},
      {-0.5f, -0.5f, 0.5f},
      {-0.5f, -0.5f, -0.5f},
      // Right face
      {0.5f, -0.5f, 0.5f},
      {0.5f, -0.5f, -0.5f},
      {0.5f, 0.5f, -0.5f},
      {0.5f, 0.5f, -0.5f},
      {0.5f, 0.5f, 0.5f},
      {0.5f, -0.5f, 0.5f},
      // Left face
      {-0.5f, -0.5f, -0.5f},
      {-0.5f, -0.5f, 0.5f},
      {-0.5f, 0.5f, 0.5f},
      {-0.5f, 0.5f, 0.5f},
      {-0.5f, 0.5f, -0.5f},
      {-0.5f, -0.5f, -0.5f},
  };

  glm::vec3 normals[] = {
      {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},
      {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},  {0.0f, 0.0f, 1.0f},
      {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
      {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, -1.0f},
      {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},
      {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},  {0.0f, 1.0f, 0.0f},
      {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
      {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},
      {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},  {1.0f, 0.0f, 0.0f},
      {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
      {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
  };

  for (int i = 0; i < 36; ++i) {
    vertices.push_back({positions[i], normals[i]});
  }

  return vertices;
}

// Generate plane vertices
std::vector<Vertex> generatePlaneVertices() {
  // Make sure the winding order is counter-clockwise when viewed from above
  // Using a slightly higher position and smaller size for better visibility
  return {
      {{-3.0f, -0.5f, 3.0f}, {0.0f, 1.0f, 0.0f}},
      {{3.0f, -0.5f, 3.0f}, {0.0f, 1.0f, 0.0f}},
      {{3.0f, -0.5f, -3.0f}, {0.0f, 1.0f, 0.0f}},

      {{3.0f, -0.5f, -3.0f}, {0.0f, 1.0f, 0.0f}},
      {{-3.0f, -0.5f, -3.0f}, {0.0f, 1.0f, 0.0f}},
      {{-3.0f, -0.5f, 3.0f}, {0.0f, 1.0f, 0.0f}},
  };
}

// Global rendering data
struct RenderData {
  // Buffers
  Buffer cubeVBO;
  Buffer planeVBO;
  VertexArray cubeVAO;
  VertexArray planeVAO;

  // Shaders
  Program shadowProgram;
  Program sceneProgram;

  // Shadow map
  Framebuffer shadowFBO;
  Texture shadowMap;

  // Scene parameters
  glm::vec3 lightPos = glm::vec3(3.0f, 4.0f, 2.0f);
  glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 5.0f);
  glm::mat4 lightSpaceMatrix;
  glm::mat4 view;
  glm::mat4 projection;

  int shadowMapSize = 2048;
  int screenWidth = 800;
  int screenHeight = 600;

  // Constructor to initialize Texture with target
  RenderData() : shadowMap(GL_TEXTURE_2D) {}
};

void setupGeometry(RenderData &rd) {
  // Generate vertex data
  auto cubeVertices = generateCubeVertices();
  auto planeVertices = generatePlaneVertices();

  std::cout << "Cube vertices: " << cubeVertices.size() << std::endl;
  std::cout << "Plane vertices: " << planeVertices.size() << std::endl;
  std::cout << "Plane vertex 0: (" << planeVertices[0].position.x << ", "
            << planeVertices[0].position.y << ", "
            << planeVertices[0].position.z << ")" << std::endl;

  // Setup cube VBO and VAO
  rd.cubeVBO.set_storage(cubeVertices.size() * sizeof(Vertex),
                         cubeVertices.data(), 0);

  auto &cubeBinding = rd.cubeVAO.get_binding(0);
  cubeBinding.bind_buffer(rd.cubeVBO, 0, sizeof(Vertex));

  auto &cubePos = rd.cubeVAO.get_attribute(0);
  cubePos.set_format(3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  cubePos.bind(cubeBinding);
  cubePos.enable();

  auto &cubeNormal = rd.cubeVAO.get_attribute(1);
  cubeNormal.set_format(3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
  cubeNormal.bind(cubeBinding);
  cubeNormal.enable();

  // Setup plane VBO and VAO
  rd.planeVBO.set_storage(planeVertices.size() * sizeof(Vertex),
                          planeVertices.data(), 0);

  auto &planeBinding = rd.planeVAO.get_binding(0);
  planeBinding.bind_buffer(rd.planeVBO, 0, sizeof(Vertex));

  auto &planePos = rd.planeVAO.get_attribute(0);
  planePos.set_format(3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
  planePos.bind(planeBinding);
  planePos.enable();

  auto &planeNormal = rd.planeVAO.get_attribute(1);
  planeNormal.set_format(3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
  planeNormal.bind(planeBinding);
  planeNormal.enable();
}

void setupShaders(RenderData &rd) {
  // Compile shadow shaders
  Shader shadowVS(GL_VERTEX_SHADER);
  Shader shadowFS(GL_FRAGMENT_SHADER);

  if (!shadowVS.compile(shadow_vertex_shader)) {
    std::cerr << "Shadow vertex shader error: " << shadowVS.get_info_log()
              << std::endl;
  }
  if (!shadowFS.compile(shadow_fragment_shader)) {
    std::cerr << "Shadow fragment shader error: " << shadowFS.get_info_log()
              << std::endl;
  }

  rd.shadowProgram.attach(shadowVS);
  rd.shadowProgram.attach(shadowFS);
  if (!rd.shadowProgram.link()) {
    std::cerr << "Shadow program link error: "
              << rd.shadowProgram.get_info_log() << std::endl;
  }

  // Compile scene shaders
  Shader sceneVS(GL_VERTEX_SHADER);
  Shader sceneFS(GL_FRAGMENT_SHADER);

  if (!sceneVS.compile(scene_vertex_shader)) {
    std::cerr << "Scene vertex shader error: " << sceneVS.get_info_log()
              << std::endl;
  }
  if (!sceneFS.compile(scene_fragment_shader)) {
    std::cerr << "Scene fragment shader error: " << sceneFS.get_info_log()
              << std::endl;
  }

  rd.sceneProgram.attach(sceneVS);
  rd.sceneProgram.attach(sceneFS);
  if (!rd.sceneProgram.link()) {
    std::cerr << "Scene program link error: " << rd.sceneProgram.get_info_log()
              << std::endl;
  }
}

void setupShadowMap(RenderData &rd) {
  // Create shadow map texture
  rd.shadowMap.set_storage_2d(1, GL_DEPTH_COMPONENT24, rd.shadowMapSize,
                              rd.shadowMapSize);
  rd.shadowMap.set(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  rd.shadowMap.set(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  rd.shadowMap.set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  rd.shadowMap.set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  // Setup framebuffer
  rd.shadowFBO.attachTexture(GL_DEPTH_ATTACHMENT, &rd.shadowMap, 0);
  rd.shadowFBO.setReadBuffer(GL_NONE);
  GLenum drawBuffers[] = {GL_NONE};
  rd.shadowFBO.setDrawBuffers(1, drawBuffers);

  if (!rd.shadowFBO.isComplete(GL_FRAMEBUFFER)) {
    std::cerr << "Shadow framebuffer is not complete!" << std::endl;
  }
}

void renderScene(RenderData &rd, Program &program, bool isShadowPass) {
  // Render cube
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));

  GLint modelLoc = glGetUniformLocation(program.get_name(), "uModel");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  if (!isShadowPass) {
    GLint colorLoc = glGetUniformLocation(program.get_name(), "uObjectColor");
    glUniform3f(colorLoc, 1.0f, 0.0f, 0.0f);
  }

  rd.cubeVAO.bind();
  glDrawArrays(GL_TRIANGLES, 0, 36);

  // Render plane
  model = glm::mat4(1.0f);
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  if (!isShadowPass) {
    GLint colorLoc = glGetUniformLocation(program.get_name(), "uObjectColor");
    glUniform3f(colorLoc, 0.5f, 0.5f, 0.5f);
    std::cout << "Drawing plane with " << 6 << " vertices" << std::endl;
  }

  rd.planeVAO.bind();
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

int main() {
  auto window = Window::create(WindowConfig{
      .title = "Frame Graph Shadow Mapping Example",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 6,
              .profile = ContextProfile::Core,
              .debug = false,
          },
      .width = 800,
      .height = 600,
      .resizable = false,
      .visible = true,
      .vsync = true,
  });

  RenderData renderData;

  // Setup geometry and shaders
  setupGeometry(renderData);
  setupShaders(renderData);
  setupShadowMap(renderData);

  // Setup matrices
  renderData.projection = glm::perspective(glm::radians(45.0f),
                                           (float)renderData.screenWidth /
                                               (float)renderData.screenHeight,
                                           0.1f, 100.0f);
  renderData.view =
      glm::lookAt(renderData.cameraPos, glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f));

  // Light space matrix for shadow mapping
  glm::mat4 lightProjection =
      glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 15.0f);
  glm::mat4 lightView = glm::lookAt(renderData.lightPos, glm::vec3(0.0f),
                                    glm::vec3(0.0f, 1.0f, 0.0f));
  renderData.lightSpaceMatrix = lightProjection * lightView;

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);
  // Temporarily disable culling to debug plane rendering
  // glEnable(GL_CULL_FACE);

  std::cout << "Camera position: " << renderData.cameraPos.x << ", "
            << renderData.cameraPos.y << ", " << renderData.cameraPos.z
            << std::endl;
  std::cout << "Light position: " << renderData.lightPos.x << ", "
            << renderData.lightPos.y << ", " << renderData.lightPos.z
            << std::endl;

  FrameGraph fg;

  // Define pass data structures
  struct ShadowPassData {
    NodeId shadow_map;
  };

  struct ScenePassData {
    NodeId shadow_input;
  };

  // Shadow map pass
  const auto &shadow_pass = fg.create_pass<ShadowPassData>(
      "Shadow Pass",
      [&](FrameGraph::Builder &builder, ShadowPassData &data) {
        data.shadow_map = builder.create<FrameGraphTexture>(
            "ShadowMap",
            {
                .width = static_cast<uint32_t>(renderData.shadowMapSize),
                .height = static_cast<uint32_t>(renderData.shadowMapSize),
                .format = GL_DEPTH_COMPONENT24,
            });
        data.shadow_map = builder.write(data.shadow_map);
      },
      [&renderData](FrameGraphResources &resources, void *context) {
        std::cout << "Executing Shadow Pass\n";

        // Bind shadow framebuffer
        renderData.shadowFBO.bind();
        glViewport(0, 0, renderData.shadowMapSize, renderData.shadowMapSize);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Use shadow shader
        renderData.shadowProgram.use();

        GLint lightSpaceLoc = glGetUniformLocation(
            renderData.shadowProgram.get_name(), "uLightSpaceMatrix");
        glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE,
                           glm::value_ptr(renderData.lightSpaceMatrix));

        // Render scene from light's perspective
        glCullFace(GL_FRONT);
        renderScene(renderData, renderData.shadowProgram, true);
        glCullFace(GL_BACK);

        Framebuffer::unbind();
      });

  // Scene rendering pass
  const auto &scene_pass = fg.create_pass<ScenePassData>(
      "Scene Pass",
      [&](FrameGraph::Builder &builder, ScenePassData &data) {
        data.shadow_input = builder.read(shadow_pass.shadow_map);
      },
      [&renderData](FrameGraphResources &resources, void *context) {
        std::cout << "Executing Scene Pass\n";

        // Render to default framebuffer
        glViewport(0, 0, renderData.screenWidth, renderData.screenHeight);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use scene shader
        renderData.sceneProgram.use();

        // Set uniforms
        GLint viewLoc =
            glGetUniformLocation(renderData.sceneProgram.get_name(), "uView");
        GLint projLoc = glGetUniformLocation(renderData.sceneProgram.get_name(),
                                             "uProjection");
        GLint lightSpaceLoc = glGetUniformLocation(
            renderData.sceneProgram.get_name(), "uLightSpaceMatrix");
        GLint lightPosLoc = glGetUniformLocation(
            renderData.sceneProgram.get_name(), "uLightPos");
        GLint viewPosLoc = glGetUniformLocation(
            renderData.sceneProgram.get_name(), "uViewPos");
        GLint shadowMapLoc = glGetUniformLocation(
            renderData.sceneProgram.get_name(), "uShadowMap");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE,
                           glm::value_ptr(renderData.view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE,
                           glm::value_ptr(renderData.projection));
        glUniformMatrix4fv(lightSpaceLoc, 1, GL_FALSE,
                           glm::value_ptr(renderData.lightSpaceMatrix));
        glUniform3fv(lightPosLoc, 1, glm::value_ptr(renderData.lightPos));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(renderData.cameraPos));
        glUniform1i(shadowMapLoc, 0);

        // Bind shadow map
        renderData.shadowMap.bind(0);

        // Render scene with shadows
        renderScene(renderData, renderData.sceneProgram, false);
      });

  std::cout << "\n=== Compiling Frame Graph ===\n";
  fg.compile();

  // Export visualizations
  std::cout << "\n=== Exporting Visualizations ===\n";
  fg.exportToDot("frame_graph.dot");
  fg.exportExecutionOrderToDot("frame_graph_execution.dot");
  std::cout << "Generated frame_graph.dot and frame_graph_execution.dot\n";

  // Main render loop
  while (!window->shouldClose()) {
    window->pollEvents();

    RenderContext rc;
    TransientResources allocator(rc);
    fg.execute(&rc, &allocator);

    window->swapBuffers();
  }

  window->destroy();
  return 0;
}