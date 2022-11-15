#pragma once

#include <memory>
#include <string>
#include <vector>

#include "rapidxml/rapidxml.hpp"

namespace Paimon
{
class Pass;
class Texture2D;

class Technique
{

public:
    Technique() = default;
    ~Technique() = default;

    void Parse(rapidxml::xml_node<>* techniqueNode);

    std::vector<std::shared_ptr<Pass>> GetPasses() { return m_passes; }

    void SetTexture(const std::string &name, const std::shared_ptr<Texture2D> &texture);

private:
    std::vector<std::shared_ptr<Pass>> m_passes;

}; // class Technique

} // namespace Paimon

