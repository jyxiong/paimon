#include "paimon/core/log_system.h"
#include "paimon/opengl/state.h"
#include "paimon/platform/window.h"
#include <array>

using namespace paimon;

int main() {
  LogSystem::init();

  auto window = Window::create(WindowConfig{
      .title = "State Setting Unified Example",
      .format =
          ContextFormat{
              .majorVersion = 4,
              .minorVersion = 5,
              .profile = ContextProfile::Core,
          },
      .width = 800,
      .height = 600,
  });

  // --- Build example state objects ---------------------------------------
  DepthState depth;
  depth.depthTestEnable = true;
  depth.depthWriteEnable = true;
  depth.depthCompareOp = GL_LESS;

  RasterizationState rast;
  rast.cullEnable = true;
  rast.cullMode = GL_BACK;
  rast.polygonMode = GL_FILL;
  rast.frontFace = GL_CCW;

  ViewportState vp;
  vp.viewport.x = 0;
  vp.viewport.y = 0;
  vp.viewport.width = 800;
  vp.viewport.height = 600;
  vp.depthRange.near = 0.0f;
  vp.depthRange.far = 1.0f;

  ScissorState sc;
  sc.enable = false;
  sc.scissor = {0, 0, 800, 600};

  ColorBlendState blend;
  blend.logicOpEnable = false;
  blend.blendConstants = {0, 0, 0, 0};
  ColorBlendAttachment att;
  att.blendEnabled = true;
  att.blendFactor.srcRGBFactor = GL_SRC_ALPHA;
  att.blendFactor.dstRGBFactor = GL_ONE_MINUS_SRC_ALPHA;
  att.blendFactor.srcAlphaFactor = GL_ONE;
  att.blendFactor.dstAlphaFactor = GL_ONE_MINUS_SRC_ALPHA;
  att.blendEquation.rgbBlendOp = GL_FUNC_ADD;
  att.blendEquation.alphaBlendOp = GL_FUNC_ADD;
  blend.attachments.push_back(att);

  StencilState stencil;
  stencil.enable = false;

  MultisampleState ms;
  ms.sampleShadingEnable = false;

  InputAssemblyState ia;
  ia.primitiveTopology = GL_TRIANGLES;

  TessellationState tess;
  tess.patchControlPoints = 3;

  // --- Use tracker classes to apply states -------------------------------
  PipelineTracker pipeline;
  pipeline.depth.apply(depth);
  pipeline.rasterization.apply(rast);
  pipeline.viewport.apply({vp});
  pipeline.scissor.apply({sc});
  pipeline.colorBlend.apply(blend);
  pipeline.stencil.apply(stencil);
  pipeline.multisample.apply(ms);
  pipeline.inputAssembly.apply(ia);
  pipeline.tessellation.apply(tess);

  // --- Minimal render loop ------------------------------------------------
  while (!window->shouldClose()) {
    window->pollEvents();

    // Define colors for each quadrant
    const std::array<std::array<float, 4>, 4> colors = {
        std::array<float, 4>{1.0f, 0.0f, 0.0f, 1.0f}, // Red
        std::array<float, 4>{0.0f, 1.0f, 0.0f, 1.0f}, // Green
        std::array<float, 4>{0.0f, 0.0f, 1.0f, 1.0f}, // Blue
        std::array<float, 4>{1.0f, 1.0f, 0.0f, 1.0f}  // Yellow
    };

    // Clear the entire screen first
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Divide the window into four quadrants
    int halfWidth = 800 / 2;
    int halfHeight = 600 / 2;

    std::vector<ViewportState> viewports = {
        {{0, halfHeight, halfWidth, halfHeight}, {0.0f, 1.0f}}, // Top-left
        {{halfWidth, halfHeight, halfWidth, halfHeight},
         {0.0f, 1.0f}},                                       // Top-right
        {{0, 0, halfWidth, halfHeight}, {0.0f, 1.0f}},        // Bottom-left
        {{halfWidth, 0, halfWidth, halfHeight}, {0.0f, 1.0f}} // Bottom-right
    };

    std::vector<ScissorState> scissors = {
        {true, {0, halfHeight, halfWidth, halfHeight}},         // Top-left
        {true, {halfWidth, halfHeight, halfWidth, halfHeight}}, // Top-right
        {true, {0, 0, halfWidth, halfHeight}},                  // Bottom-left
        {true, {halfWidth, 0, halfWidth, halfHeight}}           // Bottom-right
    };

    for (size_t i = 0; i < viewports.size(); ++i) {
      // Apply viewport and scissor for each quadrant
      pipeline.viewport.apply({viewports[i]});
      pipeline.scissor.apply({scissors[i]});

      // Clear the quadrant with the corresponding color
      const auto &color = colors[i];
      glClearColor(color[0], color[1], color[2], color[3]);
      glClear(GL_COLOR_BUFFER_BIT);
    }

    window->swapBuffers();
  }

  return 0;
}