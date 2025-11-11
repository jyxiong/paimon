#include "paimon/core/sg/scene.h"
#include "paimon/core/sg/node.h"
#include "paimon/core/sg/camera.h"
#include "paimon/core/sg/light.h"
#include "paimon/core/sg/mesh.h"
#include "paimon/core/sg/material.h"
#include "paimon/core/sg/texture.h"
#include "paimon/core/sg/image.h"
#include "paimon/core/sg/sampler.h"

namespace paimon {
namespace sg {

void Scene::AddRootNode(std::shared_ptr<Node> node) {
    root_nodes.push_back(node);
}

void Scene::AddCamera(std::shared_ptr<Camera> camera) {
    cameras.push_back(camera);
}

void Scene::AddLight(std::shared_ptr<PunctualLight> light) {
    lights.push_back(light);
}

void Scene::AddMesh(std::shared_ptr<Mesh> mesh) {
    meshes.push_back(mesh);
}

void Scene::AddMaterial(std::shared_ptr<Material> material) {
    materials.push_back(material);
}

void Scene::AddTexture(std::shared_ptr<Texture> texture) {
    textures.push_back(texture);
}

void Scene::AddImage(std::shared_ptr<Image> image) {
    images.push_back(image);
}

void Scene::AddSampler(std::shared_ptr<Sampler> sampler) {
    samplers.push_back(sampler);
}

std::shared_ptr<Node> Scene::FindNode(const std::string& name) const {
    std::shared_ptr<Node> result = nullptr;
    
    Traverse([&](const Node* node) {
        if (node->name == name && !result) {
            // Need to find the shared_ptr for this node
            // This is a simplified approach - in practice you might want to maintain a name->node map
            result = nullptr;  // TODO: Implement proper lookup
        }
    });
    
    return result;
}

std::vector<std::shared_ptr<Node>> Scene::GetAllNodes() const {
    std::vector<std::shared_ptr<Node>> all_nodes;
    
    Traverse([&](const Node* node) {
        // This won't work perfectly with shared_ptr
        // Better to collect during traversal with shared_ptr version
    });
    
    return all_nodes;
}

size_t Scene::GetNodeCount() const {
    size_t count = 0;
    
    Traverse([&](const Node* /*node*/) {
        ++count;
    });
    
    return count;
}

}  // namespace sg
}  // namespace paimon
