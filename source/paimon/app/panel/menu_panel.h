#pragma once

namespace paimon {

class MenuPanel {
public:
  MenuPanel();
  ~MenuPanel();

  void onImGuiRender();

private:
  void showMainMenuBar();
  void showFileMenu();
};

} // namespace paimon
