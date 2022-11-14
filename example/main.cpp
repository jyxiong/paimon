#include "component/Transform.h"
#include "utils/application.h"

#include "Scene.h"

using namespace Paimon;

int main()
{
    Application::SetDataPath("../asset");
    Application::InitOpengl();

    auto renderer = EntityManager::CreateEntity("Renderer");
    renderer->AddComponent<Scene>();
    renderer->AddComponent<Transform>();

    Application::Run();

    return 0;
}