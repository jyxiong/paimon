#pragma once

#include <memory>
#include <string>
#include <vector>

#include "paimon/core/sg/node.h"

namespace paimon {
namespace sg {

// Forward declarations
class Node;
struct Camera;
class PunctualLight;
struct Image;
struct Sampler;
struct Texture;
struct Material;
struct Mesh;

/// Scene containing the root nodes and all resources
class Scene {
public:
    Scene() = default;
    Scene(const std::string& name) : name(name) {}
    
    std::string name;
    
    // Root nodes of the scene graph
    std::vector<std::shared_ptr<Node>> root_nodes;
    
    // Global resource lists (referenced by nodes)
    std::vector<std::shared_ptr<Camera>> cameras;
    std::vector<std::shared_ptr<PunctualLight>> lights;  // KHR_lights_punctual
    std::vector<std::shared_ptr<Mesh>> meshes;
    std::vector<std::shared_ptr<Material>> materials;
    std::vector<std::shared_ptr<Texture>> textures;
    std::vector<std::shared_ptr<Image>> images;
    std::vector<std::shared_ptr<Sampler>> samplers;
    
    // Helper methods to add resources
    void AddRootNode(std::shared_ptr<Node> node);
    void AddCamera(std::shared_ptr<Camera> camera);
    void AddLight(std::shared_ptr<PunctualLight> light);
    void AddMesh(std::shared_ptr<Mesh> mesh);
    void AddMaterial(std::shared_ptr<Material> material);
    void AddTexture(std::shared_ptr<Texture> texture);
    void AddImage(std::shared_ptr<Image> image);
    void AddSampler(std::shared_ptr<Sampler> sampler);
    
    // Traverse all nodes in the scene (defined in scene_inl.h after Node is complete)
template <typename Func>
void Traverse(Func&& func) {
    for (auto& node : root_nodes) {
        node->Traverse(std::forward<Func>(func));
    }
}

template <typename Func>
void Traverse(Func&& func) const {
    for (const auto& node : root_nodes) {
        node->Traverse(std::forward<Func>(func));
    }
}
    
    // Find nodes by name
    std::shared_ptr<Node> FindNode(const std::string& name) const;
    
    // Get all nodes (flattened)
    std::vector<std::shared_ptr<Node>> GetAllNodes() const;
    
    // Get scene statistics
    size_t GetNodeCount() const;
    size_t GetMeshCount() const { return meshes.size(); }
    size_t GetMaterialCount() const { return materials.size(); }
    size_t GetLightCount() const { return lights.size(); }
    size_t GetTextureCount() const { return textures.size(); }
    size_t GetImageCount() const { return images.size(); }
};

}  // namespace sg
}  // namespace paimon
