#include "paimon/app/panel/menu_panel.h"

#include <imgui.h>
#include <nfd.h>

#include "paimon/app/application.h"
#include "paimon/app/event/application_event.h"
#include "paimon/core/ecs/scene.h"
#include "paimon/core/log_system.h"

namespace paimon {

MenuPanel::MenuPanel() {
  // Initialize NFD
  NFD_Init();
}

MenuPanel::~MenuPanel() {
  // Cleanup NFD
  NFD_Quit();
}

void MenuPanel::onImGuiRender() {
  showMainMenuBar();
}

void MenuPanel::showMainMenuBar() {
  if (ImGui::BeginMainMenuBar()) {
    showFileMenu();
    ImGui::EndMainMenuBar();
  }
}

void MenuPanel::showFileMenu() {
  if (ImGui::BeginMenu("File")) {
    if (ImGui::MenuItem("New Scene", "Ctrl+N")) {
      auto& scene = Application::getInstance().getScene();
      // Clear current scene
      scene.clear();
    }
    
    if (ImGui::MenuItem("Load Model...", "Ctrl+O")) {
      nfdu8char_t* outPath = nullptr;
      nfdu8filteritem_t filters[2] = {{"glTF Model", "gltf,glb"}, {"All Files", "*"}};
      nfdresult_t result = NFD_OpenDialogU8(&outPath, filters, 2, nullptr);
      
      if (result == NFD_OKAY) {
        auto& scene = Application::getInstance().getScene();
        LOG_INFO("Loading model: {}", outPath);
        scene.load(outPath);
        NFD_FreePathU8(outPath);
      } else if (result == NFD_ERROR) {
        LOG_ERROR("Error opening file dialog: {}", NFD_GetError());
      }
    }
    
    ImGui::Separator();
    
    if (ImGui::MenuItem("Exit", "Alt+F4")) {
      WindowCloseEvent event;
      Application::getInstance().onEvent(event);
    }
    
    ImGui::EndMenu();
  }
}

} // namespace paimon
