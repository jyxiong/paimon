#include "paimon/app/panel/ibl_panel.h"

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <nfd.h>
#include <stb_image.h>

#include "paimon/app/application.h"
#include "paimon/core/ecs/components.h"
#include "paimon/core/ecs/scene.h"
#include "paimon/core/log_system.h"

namespace paimon {

IBLPanel::IBLPanel() {}

IBLPanel::~IBLPanel() {}

void IBLPanel::onImGuiRender() {
  ImGui::Begin("IBL");

  // ---- Load HDR section ------------------------------------------------
  ImGui::SeparatorText("Equirectangular Map");

  if (ImGui::Button("Load HDR...")) {
    nfdu8char_t *outPath = nullptr;
    nfdu8filteritem_t filters[2] = {{"HDR Image", "hdr"}, {"All Files", "*"}};
    nfdresult_t result = NFD_OpenDialogU8(&outPath, filters, 2, nullptr);

    if (result == NFD_OKAY) {
      if (loadHDR(outPath)) {
        m_hdrPath = outPath;
        LOG_INFO("IBL: loaded HDR '{}'", m_hdrPath);
        generateIBL(); // run immediately after loading
      }
      NFD_FreePathU8(outPath);
    } else if (result == NFD_ERROR) {
      LOG_ERROR("IBL: NFD error – {}", NFD_GetError());
    }
  }

  // Show the loaded file path
  if (!m_hdrPath.empty()) {
    ImGui::SameLine();
    ImGui::TextUnformatted(m_hdrPath.c_str());
  }

  // ---- Preview ---------------------------------------------------------
  if (m_equirectTexture) {
    ImVec2 avail = ImGui::GetContentRegionAvail();
    float maxW = avail.x;
    float ratio = m_previewHeight / m_previewWidth;
    float dispW = maxW;
    float dispH = dispW * ratio;

    ImGui::Image((ImTextureID)(uintptr_t)m_equirectTexture->get_name(),
                 ImVec2(dispW, dispH),
                 ImVec2(0, 1), // UV top-left  (flip vertically)
                 ImVec2(1, 0)  // UV bottom-right
    );
  }

  ImGui::End();
}

bool IBLPanel::loadHDR(const std::string &path) {
  stbi_set_flip_vertically_on_load(true);

  int width, height, nrChannels;
  float *data = stbi_loadf(path.c_str(), &width, &height, &nrChannels, 0);

  if (!data) {
    LOG_ERROR("IBL: failed to load HDR image '{}': {}", path,
              stbi_failure_reason());
    return false;
  }

  GLenum format = GL_RGB;
  if (nrChannels == 1)
    format = GL_RED;
  else if (nrChannels == 4)
    format = GL_RGBA;

  m_equirectTexture = std::make_unique<Texture>(GL_TEXTURE_2D);
  m_equirectTexture->set_storage_2d(1, GL_RGB32F, width, height);
  m_equirectTexture->set_sub_image_2d(0, 0, 0, width, height, format, GL_FLOAT,
                                      data);
  m_equirectTexture->set(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  m_equirectTexture->set(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  m_equirectTexture->set(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  m_equirectTexture->set(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  m_previewWidth = static_cast<float>(width);
  m_previewHeight = static_cast<float>(height);

  stbi_image_free(data);
  return true;
}

void IBLPanel::generateIBL() {

  m_iblSampler = std::make_unique<IBLSampler>();
  m_iblSampler->execute(*m_equirectTexture);

  // Wire the textures into the scene Environment entity.
  auto &scene = Application::getInstance().getScene();
  auto envEntity = scene.getEnvironment();

  auto &envComp = envEntity.getComponent<ecs::Environment>();
  envComp.irradianceMap = m_iblSampler->getIrradianceMap();
  envComp.prefilteredMap = m_iblSampler->getPrefilteredMap();
  envComp.brdfLUT = m_iblSampler->getBRDFLUT();
}

} // namespace paimon
