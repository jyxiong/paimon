#include "Technique.h"

#include "Pass.h"

using namespace Paimon;

void Technique::Parse(rapidxml::xml_node<> *techniqueNode)
{
    if (techniqueNode == nullptr)
    {
        return;
    }

    // 解析pass
    rapidxml::xml_node<> *passNode = techniqueNode->first_node("pass");
    while (passNode != nullptr)
    {
        auto pass = std::make_shared<Pass>();
        pass->Parse(passNode);
        m_passes.push_back(pass);

        passNode = passNode->next_sibling("pass");
    }
}

void Technique::SetTexture(const std::string &name, const std::shared_ptr<Texture2D> &texture)
{
    for (const auto &pass : m_passes)
        pass->SetTexture(name, texture);
}
