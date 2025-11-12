#include <cmath>

#include <glad/gl.h>
#include <stb_image_write.h>

#include "paimon/core/log_system.h"
#include "paimon/opengl/program_pipeline.h"
#include "paimon/opengl/sampler.h"
#include "paimon/opengl/shader_program.h"
#include "paimon/opengl/texture.h"
#include "paimon/platform/context.h"

using namespace paimon;

const char *compSrc = R"(
#version 450 core

layout(local_size_x = 16, local_size_y = 16) in;

layout(binding = 0) uniform sampler3D uVolume;
layout(binding = 1, rgba8) uniform writeonly image2D outImage;

void main() {
  ivec2 pix = ivec2(gl_GlobalInvocationID.xy);

  ivec2 size = imageSize(outImage);

  if (pix.x >= size.x || pix.y >= size.y) 
  {
    return;
  }
  
  vec2 uv = (vec2(pix) + vec2(0.5)) / vec2(size);
  vec4 c = texture(uVolume, vec3(uv, 0.5));

  imageStore(outImage, pix, vec4(c.rgb, 1.0));
}
)";

int main() {
  LogSystem::init();

  auto context = Context::create(
    ContextFormat{
      .majorVersion = 4,
      .minorVersion = 5,
    }
  );
  context->makeCurrent();

  // Volume and output dimensions
  const int volW = 64, volH = 64, volD = 64;
  const int outW = 512, outH = 512;

  std::vector<uint8_t> volData(volW * volH * volD * 4);
  for (int z = 0; z < volD; ++z) {
    for (int y = 0; y < volH; ++y) {
      for (int x = 0; x < volW; ++x) {
        int idx = ((z * volH + y) * volW + x) * 4;
        float fx = float(x) / float(std::max(1, volW - 1));
        float fy = float(y) / float(std::max(1, volH - 1));
        float fz = float(z) / float(std::max(1, volD - 1));
        volData[idx + 0] = uint8_t(std::round(fx * 255.0f));
        volData[idx + 1] = uint8_t(std::round(fy * 255.0f));
        volData[idx + 2] = uint8_t(std::round(fz * 255.0f));
        volData[idx + 3] = 255;
      }
    }
  }

  // Create and populate 3D texture using paimon wrapper
  Texture tex3D(GL_TEXTURE_3D);
  tex3D.set_storage_3d(1, GL_RGBA8, volW, volH, volD);
  tex3D.set_sub_image_3d(
    0, 0, 0, 0, volW, volH, volD, GL_RGBA, GL_UNSIGNED_BYTE, volData.data()
  );

  // Create sampler for 3D texture
  Sampler sampler3D;
  sampler3D.set<GLenum>(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  sampler3D.set<GLenum>(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  sampler3D.set<GLenum>(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  sampler3D.set<GLenum>(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  sampler3D.set<GLenum>(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // Create output 2D texture using paimon wrapper (no sampler parameters)
  Texture outTex(GL_TEXTURE_2D);
  outTex.set_storage_2d(1, GL_RGBA8, outW, outH);

  // Create compute shader program using paimon wrapper
  ShaderProgram computeProgram(GL_COMPUTE_SHADER, compSrc);
  ProgramPipeline pipeline;
  pipeline.use_program_stages(GL_COMPUTE_SHADER_BIT, computeProgram.get_name());
  if (!pipeline.validate()) {
    LOG_ERROR("Program pipeline validation failed");
  }

  // Bind texture to unit 0 and sampler to same unit
  tex3D.bind(0);
  sampler3D.bind(0);
  glBindImageTexture(
    1, outTex.get_name(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8
  );

  // Dispatch compute
  pipeline.bind();

  const int localSizeX = 16, localSizeY = 16;
  int groupsX = (outW + localSizeX - 1) / localSizeX;
  int groupsY = (outH + localSizeY - 1) / localSizeY;
  glDispatchCompute(groupsX, groupsY, 1);
  glMemoryBarrier(
    GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT
  );

  // Read back pixels
  std::vector<uint8_t> pixels(outW * outH * 4);  
  outTex.get_image(
    0, GL_RGBA, GL_UNSIGNED_BYTE, static_cast<GLsizei>(pixels.size()),
    pixels.data()
  );
  
  const char *outPath = "compute_output.png";
  if (!stbi_write_png(outPath, outW, outH, 4, pixels.data(), outW * 4)) {
    LOG_ERROR("Failed to write PNG");
    return EXIT_FAILURE;
  }
  LOG_INFO("Wrote output image: {}", outPath);

  return EXIT_SUCCESS;
}
