#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "paimon/core/sg/scene.h"
#include "paimon/core/sg/node.h"
#include "paimon/core/sg/mesh.h"
#include "paimon/core/sg/material.h"
#include "paimon/core/sg/texture.h"
#include "paimon/core/sg/image.h"
#include "paimon/core/sg/sampler.h"

// Forward declarations
namespace tinygltf {
class Model;
}  // namespace tinygltf

namespace paimon {

// glTF Loader class
class GltfLoader {
public:
    GltfLoader() = default;
    ~GltfLoader() = default;

    // Load glTF file and return a scene
    bool LoadFromFile(const std::string& filepath, sg::Scene& out_scene);

    // Get error messages
    const std::string& GetError() const { return error_message_; }
    const std::string& GetWarning() const { return warning_message_; }

private:
    // Helper functions to process glTF data into scene graph
    void ProcessImages(const tinygltf::Model& model, sg::Scene& scene);
    void ProcessSamplers(const tinygltf::Model& model, sg::Scene& scene);
    void ProcessTextures(const tinygltf::Model& model, sg::Scene& scene);
    void ProcessMaterials(const tinygltf::Model& model, sg::Scene& scene);
    void ProcessMeshes(const tinygltf::Model& model, sg::Scene& scene);
    void ProcessNodes(const tinygltf::Model& model, sg::Scene& scene, 
                      std::vector<std::shared_ptr<sg::Node>>& temp_nodes);
    void ProcessScenes(const tinygltf::Model& model, sg::Scene& scene,
                       const std::vector<std::shared_ptr<sg::Node>>& temp_nodes);

    // Accessor helpers
    template <typename T>
    void ExtractAccessorData(const tinygltf::Model& model,
                             int accessor_index,
                             std::vector<T>& out_data);

    std::string error_message_;
    std::string warning_message_;
};

}  // namespace paimon
