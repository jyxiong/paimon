#pragma once

#include <memory>
#include <string>

namespace paimon {
namespace sg {

struct Image;
struct Sampler;

/// Texture combining image and sampler
struct Texture {
    std::string name;
    
    // References to image and sampler
    std::shared_ptr<Image> image = nullptr;
    std::shared_ptr<Sampler> sampler = nullptr;
    
    Texture() = default;
    Texture(const std::string& name) : name(name) {}
};

}  // namespace sg
}  // namespace paimon
