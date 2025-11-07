#include "paimon/asset/model.h"

#include "paimon/core/log_system.h"

namespace paimon {

Model::Model(const AssetMetadata &metadata)
    : Asset(metadata) {
  m_metadata.type = AssetType::Model;
}

bool Model::load() {
  if (m_isLoaded) {
    LOG_WARN("Model already loaded: {}", m_metadata.filePath.string());
    return true;
  }

  // TODO: Implement model loading using Assimp, tinygltf, or similar library
  // For now, this is a placeholder
  
  LOG_ERROR("Model::load() not yet implemented for: {}", 
            m_metadata.filePath.string());
  return false;
}

void Model::unload() {
  if (!m_isLoaded) {
    return;
  }

  m_meshes.clear();
  m_materials.clear();
  m_isLoaded = false;
  m_metadata.memorySize = 0;
  
  LOG_INFO("Unloaded model: {}", m_metadata.filePath.string());
}

bool Model::isLoaded() const {
  return m_isLoaded;
}

size_t Model::getVertexCount() const {
  size_t count = 0;
  for (const auto &mesh : m_meshes) {
    count += mesh.vertices.size() / 3; // Assuming 3 floats per vertex
  }
  return count;
}

size_t Model::getTriangleCount() const {
  size_t count = 0;
  for (const auto &mesh : m_meshes) {
    count += mesh.indices.size() / 3; // Assuming 3 indices per triangle
  }
  return count;
}

} // namespace paimon
