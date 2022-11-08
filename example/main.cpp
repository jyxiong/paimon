#include "component/Transform.h"
#include "utils/application.h"

#include "Renderer.h"

using namespace Paimon;

int main()
{
    Application::SetDataPath("../asset");
    Application::InitOpengl();

    auto renderer = EntityManager::CreateEntity("Renderer");
    renderer->AddComponent<Renderer>();
    renderer->AddComponent<Transform>();

    Application::Run();

    return 0;
}