#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <filesystem>
#include <map>

#include "paimon/app/window.h"
#include "paimon/core/io/file.h"
#include "paimon/core/io/gltf.h"
#include "paimon/core/log_system.h"
#include "paimon/opengl/buffer.h"
#include "paimon/opengl/program.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/shader.h"
#include "paimon/opengl/texture.h"
#include "paimon/opengl/vertex_array.h"
#include "paimon/rendering/shader_preprocessor.h"
#include "paimon/rendering/shader_source.h"

using namespace paimon;

namespace {

// Camera state
struct Camera {
  glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
  glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
  float yaw = -90.0f;
  float pitch = 0.0f;
  float fov = 45.0f;
};

Camera g_camera;
float g_lastX = 400.0f;
float g_lastY = 300.0f;
bool g_firstMouse = true;
float g_deltaTime = 0.0f;
float g_lastFrame = 0.0f;

// Helper function to create OpenGL texture from sg::Image
Texture createTextureFromImage(const std::shared_ptr<sg::Image> &image) {
  Texture texture(GL_TEXTURE_2D);

  if (!image || image->data.empty()) {
    // Create a default 1x1 white texture
    std::vector<unsigned char> white = {255, 255, 255, 255};
    texture.set_storage_2d(1, GL_RGBA8, 1, 1);
    texture.set_sub_image_2d(0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                             white.data());
    return texture;
  }

  GLenum internalFormat = GL_RGBA8;
  GLenum format = GL_RGBA;

  if (image->components == 1) {
    internalFormat = GL_R8;
    format = GL_RED;
  } else if (image->components == 2) {
    internalFormat = GL_RG8;
    format = GL_RG;
  } else if (image->components == 3) {
    internalFormat = GL_RGB8;
    format = GL_RGB;
  }

  texture.set_storage_2d(1, internalFormat, image->width, image->height);
  texture.set_sub_image_2d(0, 0, 0, image->width, image->height, format,
                           GL_UNSIGNED_BYTE, image->data.data());
  texture.generate_mipmap();

  return texture;
}

} // namespace

int main() {
  LogSystem::init();
  LOG_INFO("Starting DamagedHelmet Example");

  // Create window
  auto window = Window::create(WindowConfig{
      .title = "Paimon - DamagedHelmet",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 6,
              .profile = ContextProfile::Core,
              .debug = false,
          },
      .width = 1280,
      .height = 720,
      .resizable = true,
      .visible = true,
      .vsync = true,
  });

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);

  // Setup shader preprocessor
  auto assetPath = std::filesystem::current_path().parent_path() / "asset";
  auto shaderPath = assetPath / "shader";

  ShaderPreprocessor preprocessor;
  preprocessor.addIncludePath(shaderPath);

  // Load and process shaders
  ShaderSource vertexSource(shaderPath / "damaged_helmet.vert");
  ShaderSource fragmentSource(shaderPath / "damaged_helmet.frag");

  std::string vertex_source = preprocessor.processShaderSource(vertexSource);
  std::string fragment_source =
      preprocessor.processShaderSource(fragmentSource);

  File::writeText(shaderPath / "damaged_helmet_processed.vert", vertex_source);
  File::writeText(shaderPath / "damaged_helmet_processed.frag",
                  fragment_source);

  // Load glTF model
  GltfLoader loader;
  sg::Scene scene;

  auto assetPModelath =
      std::filesystem::current_path().parent_path() / "asset/model";

  std::string model_path =
      (assetPModelath / "DamagedHelmet/glTF/DamagedHelmet.gltf").string();
  if (!loader.LoadFromFile(model_path, scene)) {
    LOG_ERROR("Failed to load glTF model: {}", loader.GetError());
    return -1;
  }

  if (!loader.GetWarning().empty()) {
    LOG_WARN("glTF warnings: {}", loader.GetWarning());
  }

  LOG_INFO("Loaded glTF model successfully");
  LOG_INFO("Meshes: {}, Materials: {}, Textures: {}, Images: {}",
           scene.meshes.size(), scene.materials.size(), scene.textures.size(),
           scene.images.size());

  // Compile shaders
  Shader vertex_shader(GL_VERTEX_SHADER);
  Shader fragment_shader(GL_FRAGMENT_SHADER);
  Program program;

  if (!vertex_shader.compile(vertex_source)) {
    LOG_ERROR("Vertex shader compilation failed: {}",
              vertex_shader.get_info_log());
    return -1;
  }

  if (!fragment_shader.compile(fragment_source)) {
    LOG_ERROR("Fragment shader compilation failed: {}",
              fragment_shader.get_info_log());
    return -1;
  }

  program.attach(vertex_shader);
  program.attach(fragment_shader);
  if (!program.link()) {
    LOG_ERROR("Program linking failed: {}", program.get_info_log());
    return -1;
  }

  // Process all meshes in the scene
  struct MeshData {
    VertexArray vao;
    Buffer position_buffer;
    Buffer normal_buffer;
    Buffer texcoord_buffer;
    Buffer index_buffer;
    size_t index_count;
    std::shared_ptr<sg::Material> material;
  };

  std::vector<MeshData> mesh_data_list;

  for (const auto &mesh : scene.meshes) {
    for (const auto &primitive : mesh->primitives) {
      MeshData mesh_data;

      // Setup buffers
      if (primitive.attributes.HasPositions()) {
        mesh_data.position_buffer.set_storage(
            sizeof(glm::vec3) * primitive.attributes.positions.size(),
            primitive.attributes.positions.data(), GL_DYNAMIC_STORAGE_BIT);
      }

      if (primitive.attributes.HasNormals()) {
        mesh_data.normal_buffer.set_storage(
            sizeof(glm::vec3) * primitive.attributes.normals.size(),
            primitive.attributes.normals.data(), GL_DYNAMIC_STORAGE_BIT);
      }

      if (primitive.attributes.HasTexCoords0()) {
        mesh_data.texcoord_buffer.set_storage(
            sizeof(glm::vec2) * primitive.attributes.texcoords_0.size(),
            primitive.attributes.texcoords_0.data(), GL_DYNAMIC_STORAGE_BIT);
      }

      if (primitive.HasIndices()) {
        mesh_data.index_buffer.set_storage(
            sizeof(uint32_t) * primitive.indices.size(),
            primitive.indices.data(), GL_DYNAMIC_STORAGE_BIT);
        mesh_data.index_count = primitive.indices.size();
      }

      // Setup VAO
      mesh_data.vao.bind();

      // Position attribute (location 0)
      if (primitive.attributes.HasPositions()) {
        auto &binding_pos = mesh_data.vao.get_binding(0);
        binding_pos.bind_buffer(mesh_data.position_buffer, 0,
                                sizeof(glm::vec3));
        auto &attr_pos = mesh_data.vao.get_attribute(0);
        attr_pos.set_format(3, GL_FLOAT, GL_FALSE, 0);
        attr_pos.bind(binding_pos);
        attr_pos.enable();
      }

      // Normal attribute (location 1)
      if (primitive.attributes.HasNormals()) {
        auto &binding_normal = mesh_data.vao.get_binding(1);
        binding_normal.bind_buffer(mesh_data.normal_buffer, 0,
                                   sizeof(glm::vec3));
        auto &attr_normal = mesh_data.vao.get_attribute(1);
        attr_normal.set_format(3, GL_FLOAT, GL_FALSE, 0);
        attr_normal.bind(binding_normal);
        attr_normal.enable();
      }

      // Texcoord attribute (location 2)
      if (primitive.attributes.HasTexCoords0()) {
        auto &binding_texcoord = mesh_data.vao.get_binding(2);
        binding_texcoord.bind_buffer(mesh_data.texcoord_buffer, 0,
                                     sizeof(glm::vec2));
        auto &attr_texcoord = mesh_data.vao.get_attribute(2);
        attr_texcoord.set_format(2, GL_FLOAT, GL_FALSE, 0);
        attr_texcoord.bind(binding_texcoord);
        attr_texcoord.enable();
      }

      // Element buffer
      if (primitive.HasIndices()) {
        mesh_data.vao.set_element_buffer(mesh_data.index_buffer);
      }

      mesh_data.material = primitive.material;
      mesh_data_list.push_back(std::move(mesh_data));
    }
  }

  // Create textures from materials
  std::map<std::shared_ptr<sg::Texture>, std::unique_ptr<Texture>> texture_map;

  for (const auto &tex_pair : scene.textures) {
    if (tex_pair && tex_pair->image) {
      texture_map[tex_pair] =
          std::make_unique<Texture>(createTextureFromImage(tex_pair->image));
    }
  }

  // Create default textures
  Texture default_white = createTextureFromImage(nullptr);
  std::vector<unsigned char> normal_data = {128, 128, 255,
                                            255}; // default normal (0, 0, 1)
  Texture default_normal(GL_TEXTURE_2D);
  default_normal.set_storage_2d(1, GL_RGBA8, 1, 1);
  default_normal.set_sub_image_2d(0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                                  normal_data.data());

  std::vector<unsigned char> mr_data = {
      0, 255, 0, 255}; // default: no AO, full roughness, no metallic
  Texture default_metallic_roughness(GL_TEXTURE_2D);
  default_metallic_roughness.set_storage_2d(1, GL_RGBA8, 1, 1);
  default_metallic_roughness.set_sub_image_2d(0, 0, 0, 1, 1, GL_RGBA,
                                              GL_UNSIGNED_BYTE, mr_data.data());

  std::vector<unsigned char> black_data = {0, 0, 0, 255};
  Texture default_black(GL_TEXTURE_2D);
  default_black.set_storage_2d(1, GL_RGBA8, 1, 1);
  default_black.set_sub_image_2d(0, 0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,
                                 black_data.data());

  // Create sampler
  Sampler sampler;
  sampler.set(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  sampler.set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  sampler.set(GL_TEXTURE_WRAP_S, GL_REPEAT);
  sampler.set(GL_TEXTURE_WRAP_T, GL_REPEAT);

  LOG_INFO("Setup complete, entering render loop");

  float rotation = 0.0f;

  // Get uniform locations
  GLint u_model = glGetUniformLocation(program.get_name(), "u_model");
  GLint u_view = glGetUniformLocation(program.get_name(), "u_view");
  GLint u_projection = glGetUniformLocation(program.get_name(), "u_projection");
  GLint u_lightPos = glGetUniformLocation(program.get_name(), "u_lightPos");
  GLint u_viewPos = glGetUniformLocation(program.get_name(), "u_viewPos");
  GLint u_baseColorTexture =
      glGetUniformLocation(program.get_name(), "u_baseColorTexture");
  GLint u_metallicRoughnessTexture =
      glGetUniformLocation(program.get_name(), "u_metallicRoughnessTexture");
  GLint u_normalTexture =
      glGetUniformLocation(program.get_name(), "u_normalTexture");
  GLint u_emissiveTexture =
      glGetUniformLocation(program.get_name(), "u_emissiveTexture");
  GLint u_occlusionTexture =
      glGetUniformLocation(program.get_name(), "u_occlusionTexture");
  GLint u_baseColorFactor =
      glGetUniformLocation(program.get_name(), "u_baseColorFactor");
  GLint u_metallicFactor =
      glGetUniformLocation(program.get_name(), "u_metallicFactor");
  GLint u_roughnessFactor =
      glGetUniformLocation(program.get_name(), "u_roughnessFactor");
  GLint u_emissiveFactor =
      glGetUniformLocation(program.get_name(), "u_emissiveFactor");

  // Main render loop
  while (!window->shouldClose()) {
    // Time
    float currentFrame = static_cast<float>(glfwGetTime());
    g_deltaTime = currentFrame - g_lastFrame;
    g_lastFrame = currentFrame;

    // Input
    window->pollEvents();

    // Auto-rotate the model
    rotation += g_deltaTime * 30.0f; // 30 degrees per second

    // Clear
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use program
    program.use();

    // Setup matrices
    glm::mat4 model = glm::mat4(1.0f);
    model =
        glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 view = glm::lookAt(g_camera.position, glm::vec3(0.0f, 0.0f, 0.0f),
                                 g_camera.up);

    glm::mat4 projection = glm::perspective(glm::radians(g_camera.fov),
                                            1280.0f / 720.0f, 0.1f, 100.0f);

    // Set uniforms
    glUniformMatrix4fv(u_model, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(u_view, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(u_projection, 1, GL_FALSE, glm::value_ptr(projection));
    glm::vec3 lightPos(5.0f, 5.0f, 5.0f);
    glUniform3fv(u_lightPos, 1, glm::value_ptr(lightPos));
    glUniform3fv(u_viewPos, 1, glm::value_ptr(g_camera.position));

    // Render each mesh
    for (const auto &mesh_data : mesh_data_list) {
      mesh_data.vao.bind();

      // Bind textures and set material uniforms
      if (mesh_data.material) {
        const auto &mat = mesh_data.material;
        const auto &pbr = mat->pbr_metallic_roughness;

        // Base color
        glUniform4fv(u_baseColorFactor, 1,
                     glm::value_ptr(pbr.base_color_factor));
        if (pbr.base_color_texture &&
            texture_map.count(pbr.base_color_texture)) {
          texture_map.at(pbr.base_color_texture)->bind(0);
        } else {
          default_white.bind(0);
        }
        sampler.bind(0);

        // Metallic roughness
        glUniform1f(u_metallicFactor, pbr.metallic_factor);
        glUniform1f(u_roughnessFactor, pbr.roughness_factor);
        if (pbr.metallic_roughness_texture &&
            texture_map.count(pbr.metallic_roughness_texture)) {
          texture_map.at(pbr.metallic_roughness_texture)->bind(1);
        } else {
          default_metallic_roughness.bind(1);
        }
        sampler.bind(1);

        // Normal
        if (mat->normal_texture && texture_map.count(mat->normal_texture)) {
          texture_map.at(mat->normal_texture)->bind(2);
        } else {
          default_normal.bind(2);
        }
        sampler.bind(2);

        // Emissive
        glUniform3fv(u_emissiveFactor, 1, glm::value_ptr(mat->emissive_factor));
        if (mat->emissive_texture && texture_map.count(mat->emissive_texture)) {
          texture_map.at(mat->emissive_texture)->bind(3);
        } else {
          default_black.bind(3);
        }
        sampler.bind(3);

        // Occlusion
        if (mat->occlusion_texture &&
            texture_map.count(mat->occlusion_texture)) {
          texture_map.at(mat->occlusion_texture)->bind(4);
        } else {
          default_white.bind(4);
        }
        sampler.bind(4);

        glUniform1i(u_baseColorTexture, 0);
        glUniform1i(u_metallicRoughnessTexture, 1);
        glUniform1i(u_normalTexture, 2);
        glUniform1i(u_emissiveTexture, 3);
        glUniform1i(u_occlusionTexture, 4);
      }

      // Draw
      if (mesh_data.index_count > 0) {
        glDrawElements(GL_TRIANGLES,
                       static_cast<GLsizei>(mesh_data.index_count),
                       GL_UNSIGNED_INT, 0);
      }
    }

    // Swap buffers
    window->swapBuffers();
  }

  LOG_INFO("Shutting down");
  window->destroy();
  return 0;
}
