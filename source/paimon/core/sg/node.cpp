#include "paimon/core/sg/node.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace paimon {
namespace sg {

glm::mat4 Node::GetLocalTransform() const {
  if (!use_trs) {
    return matrix;
  }

  // Build TRS matrix: T * R * S
  glm::mat4 transform = glm::translate(glm::mat4(1.0f), translation);
  transform *= glm::mat4_cast(rotation);
  transform = glm::scale(transform, scale);

  return transform;
}

glm::mat4 Node::GetGlobalTransform() const {
  glm::mat4 local_transform = GetLocalTransform();

  if (parent != nullptr) {
    return parent->GetGlobalTransform() * local_transform;
  }

  return local_transform;
}

void Node::SetLocalTransform(const glm::mat4 &transform) {
  matrix = transform;
  use_trs = false;
}

void Node::DecomposeMatrix() {
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(matrix, scale, rotation, translation, skew, perspective);
  use_trs = true;
}

void Node::AddChild(std::shared_ptr<Node> child) {
  if (!child) {
    return;
  }

  // Remove from previous parent if any
  if (child->parent != nullptr) {
    child->parent->RemoveChild(child);
  }

  children.push_back(child);
  child->parent = this;
}

void Node::RemoveChild(std::shared_ptr<Node> child) {
  auto it = std::find(children.begin(), children.end(), child);
  if (it != children.end()) {
    (*it)->parent = nullptr;
    children.erase(it);
  }
}

} // namespace sg
} // namespace paimon
