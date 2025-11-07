#pragma once

#include <vector>

#include "paimon/asset/asset.h"

namespace paimon {

/**
 * @brief Model asset for loading and managing 3D models
 * 
 * This is a placeholder for future implementation.
 * Will support formats: OBJ, FBX, GLTF, GLB, etc.
 * 
 * Planned features:
 * - Mesh loading from various formats
 * - Material and texture references
 * - Skeletal animation support
 * - Bounding box and culling data
 * - LOD (Level of Detail) support
 */
class Model : public Asset {
public:
  /**
   * @brief Mesh data structure (simplified)
   */
  struct Mesh {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    std::string materialName;
  };

  /**
   * @brief Material data structure (simplified)
   */
  struct Material {
    std::string name;
    float ambient[3] = {0.2f, 0.2f, 0.2f};
    float diffuse[3] = {0.8f, 0.8f, 0.8f};
    float specular[3] = {1.0f, 1.0f, 1.0f};
    float shininess = 32.0f;
    std::string diffuseTexture;
    std::string normalTexture;
    std::string specularTexture;
  };

public:
  explicit Model(const AssetMetadata &metadata);
  ~Model() override = default;

  bool load() override;
  void unload() override;
  bool isLoaded() const override;

  /**
   * @brief Get all meshes in the model
   */
  const std::vector<Mesh> &getMeshes() const { return m_meshes; }

  /**
   * @brief Get all materials in the model
   */
  const std::vector<Material> &getMaterials() const { return m_materials; }

  /**
   * @brief Get number of vertices across all meshes
   */
  size_t getVertexCount() const;

  /**
   * @brief Get number of triangles across all meshes
   */
  size_t getTriangleCount() const;

private:
  std::vector<Mesh> m_meshes;
  std::vector<Material> m_materials;
  bool m_isLoaded = false;
};

} // namespace paimon
