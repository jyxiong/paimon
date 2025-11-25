#pragma once

#include <memory>
#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace paimon {
namespace sg {

struct Mesh;
struct Camera;
struct PunctualLight;

/// Scene graph node supporting hierarchical transformations
class Node {
public:
  Node() = default;
  Node(const std::string &name) : name(name) {}

  // Node properties
  std::string name;

  // Transform representation (TRS or matrix)
  glm::vec3 translation = glm::vec3(0.0f);
  glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f); // w, x, y, z
  glm::vec3 scale = glm::vec3(1.0f);
  glm::mat4 matrix = glm::mat4(1.0f);

  bool use_trs = true; // If false, use matrix directly

  // Node content (mutually exclusive in practice)
  std::shared_ptr<Mesh> mesh = nullptr;
  std::shared_ptr<Camera> camera = nullptr;
  std::shared_ptr<PunctualLight> light = nullptr; // KHR_lights_punctual

  // Hierarchy
  std::vector<std::shared_ptr<Node>> children;
  Node *parent = nullptr; // Weak reference to avoid circular ownership

  // Transformation methods
  glm::mat4 GetLocalTransform() const;
  glm::mat4 GetGlobalTransform() const;
  void SetLocalTransform(const glm::mat4 &transform);

  // Utility to decompose matrix into TRS
  void DecomposeMatrix();

  // Tree manipulation
  void AddChild(std::shared_ptr<Node> child);
  void RemoveChild(std::shared_ptr<Node> child);

  // Traverse the node tree
  template <typename Func>
  void Traverse(Func &&func) {
    func(this);
    for (auto &child : children) {
      child->Traverse(std::forward<Func>(func));
    }
  }

  template <typename Func>
  void Traverse(Func &&func) const {
    func(this);
    for (const auto &child : children) {
      child->Traverse(std::forward<Func>(func));
    }
  }
};

} // namespace sg
} // namespace paimon
