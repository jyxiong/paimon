#include "renderer.h"

#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/gtc/matrix_transform.hpp>

#include "paimon/core/fg/frame_graph.h"
#include "paimon/core/fg/transient_resources.h"
#include "paimon/core/log_system.h"
#include "paimon/core/io/gltf.h"
#include "paimon/rendering/shader_manager.h"

#include "color_pass.h"
#include "final_pass.h"

namespace paimon {

Renderer::Renderer() : m_camera{}, m_size{1280, 720}, m_rotation(0.0f) {
  m_renderContext = std::make_unique<RenderContext>();
}

bool Renderer::initialize(const glm::ivec2 &size, const std::filesystem::path &assetPath) {
  m_size = size;
  m_assetPath = assetPath;

  auto shaderPath = m_assetPath / "shader";
  auto &shaderManager = ShaderManager::getInstance();
  shaderManager.load(shaderPath);

  // Create UBOs
  m_transformUBO = std::make_unique<Buffer>();
  m_transformUBO->set_storage(sizeof(TransformUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
  m_transformUBO->bind_base(GL_UNIFORM_BUFFER, 0);

  m_materialUBO = std::make_unique<Buffer>();
  m_materialUBO->set_storage(sizeof(MaterialUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
  m_materialUBO->bind_base(GL_UNIFORM_BUFFER, 1);

  m_lightingUBO = std::make_unique<Buffer>();
  m_lightingUBO->set_storage(sizeof(LightingUBO), nullptr, GL_DYNAMIC_STORAGE_BIT);
  m_lightingUBO->bind_base(GL_UNIFORM_BUFFER, 2);
  
  // Create passes
  m_colorPass = std::make_unique<ColorPass>(*m_renderContext, m_assetPath);
  m_finalPass = std::make_unique<FinalPass>(*m_renderContext, m_assetPath);

  LOG_INFO("Renderer initialized successfully");
  return true;
}

bool Renderer::loadModel(const std::string &modelPath) {
  GltfLoader loader;
  
  if (!loader.LoadFromFile(modelPath, m_scene)) {
    LOG_ERROR("Failed to load glTF model: {}", loader.GetError());
    return false;
  }

  if (!loader.GetWarning().empty()) {
    LOG_WARN("glTF warnings: {}", loader.GetWarning());
  }

  setupModel();
  LOG_INFO("Model loaded successfully: {}", modelPath);
  return true;
}

void Renderer::setupModel() {
  m_meshDataList.clear();
  m_textureMap.clear();

  // Process all meshes in the scene
  std::vector<MeshData> colorPassMeshData;
  for (const auto &mesh : m_scene.meshes) {
    for (const auto &primitive : mesh->primitives) {
      MeshData meshData;

      // Setup buffers
      if (primitive.attributes.HasPositions()) {
        meshData.position_buffer.set_storage(
            sizeof(glm::vec3) * primitive.attributes.positions.size(),
            primitive.attributes.positions.data(), GL_DYNAMIC_STORAGE_BIT);
      }

      if (primitive.attributes.HasNormals()) {
        meshData.normal_buffer.set_storage(
            sizeof(glm::vec3) * primitive.attributes.normals.size(),
            primitive.attributes.normals.data(), GL_DYNAMIC_STORAGE_BIT);
      }

      if (primitive.attributes.HasTexCoords0()) {
        meshData.texcoord_buffer.set_storage(
            sizeof(glm::vec2) * primitive.attributes.texcoords_0.size(),
            primitive.attributes.texcoords_0.data(), GL_DYNAMIC_STORAGE_BIT);
      }

      if (primitive.HasIndices()) {
        meshData.index_buffer.set_storage(
            sizeof(uint32_t) * primitive.indices.size(),
            primitive.indices.data(), GL_DYNAMIC_STORAGE_BIT);
        meshData.index_count = primitive.indices.size();
      }

      meshData.material = primitive.material;
      colorPassMeshData.push_back(std::move(meshData));
    }
  }

  // Save mesh data for frame graph registration
  m_meshDataList = std::move(colorPassMeshData);

  // Create textures from materials
  m_textureMap.clear();
  for (const auto &texPair : m_scene.textures) {
    if (texPair && texPair->image) {
      m_textureMap[texPair] = std::make_unique<Texture>(createTextureFromImage(texPair->image));
    }
  }
}

void Renderer::setCamera(const Camera &camera) {
  m_camera = camera;
}

void Renderer::setLightPosition(const glm::vec3 &lightPos) {
  m_lightPos = lightPos;
}

void Renderer::render(float deltaTime) {
  FrameGraph fg;
  TransientResources tr(*m_renderContext);

  // Auto-rotate the model
  m_rotation += deltaTime * 30.0f; // 30 degrees per second

  // Setup transformation matrices
  TransformUBO transformData;
  transformData.model = glm::mat4(1.0f);
  transformData.model = glm::rotate(transformData.model,
                                   glm::radians(m_rotation),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
  transformData.view = glm::lookAt(m_camera.position,
                                  glm::vec3(0.0f, 0.0f, 0.0f),
                                  m_camera.up);
  transformData.projection = glm::perspective(
      glm::radians(m_camera.fov),
      static_cast<float>(m_size.x) / m_size.y,
      0.1f, 100.0f);

  // Update transform UBO
  m_transformUBO->set_sub_data(0, sizeof(TransformUBO), &transformData);

  // Setup lighting
  LightingUBO lightingData;
  lightingData.lightPos = m_lightPos;
  lightingData.viewPos = m_camera.position;

  // Update lighting UBO
  m_lightingUBO->set_sub_data(0, sizeof(LightingUBO), &lightingData);

    // Prepare texture map with raw pointers
  std::map<std::shared_ptr<sg::Texture>, Texture*> texturePtrMap;
  for (const auto &[key, value] : m_textureMap) {
    texturePtrMap[key] = value.get();
  }

  // Register ColorPass and get its output
  NodeId colorOutput = m_colorPass->registerPass(
      fg, 
      m_size,
      m_meshDataList,
      texturePtrMap,
      m_transformUBO.get(),
      m_materialUBO.get(),
      m_lightingUBO.get());

  // Register FinalPass using ColorPass output
  m_finalPass->registerPass(fg, colorOutput, m_size);

  // Compile the frame graph
  fg.compile();

  // Execute frame graph
  fg.execute(m_renderContext.get(), &tr);
}

void Renderer::resize(const glm::ivec2 &newSize) {
  if (newSize == m_size) {
    return;
  }

  m_size = newSize;
  LOG_INFO("Renderer resized to {}x{}", m_size.x, m_size.y);
}

Texture Renderer::createTextureFromImage(const std::shared_ptr<sg::Image> &image) {
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

} // namespace paimon
