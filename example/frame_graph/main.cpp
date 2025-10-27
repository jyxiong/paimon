#include "paimon/core/fg/frame_graph.h"

#include <iostream>

using namespace paimon;

int main() {
  FrameGraph fg;

  // Define frame graph resources
  struct FGTexture {
    struct Descriptor {
      uint32_t width = 1920;
      uint32_t height = 1080;
      uint32_t format = 0; // RGBA8
      std::string name;
    };

    void create(void* allocator, const Descriptor& desc) {
      std::cout << "Creating texture: " << desc.name 
                << " (" << desc.width << "x" << desc.height << ")\n";
    }
    
    void destroy(void* allocator, const Descriptor& desc) {
      std::cout << "Destroying texture: " << desc.name << "\n";
    }
    
    void preRead(void* context, const Descriptor& desc, uint32_t flags) {
      std::cout << "Pre-read texture: " << desc.name << "\n";
    }
    
    void preWrite(void* context, const Descriptor& desc, uint32_t flags) {
      std::cout << "Pre-write texture: " << desc.name << "\n";
    }
  };

  struct FGBuffer {
    struct Descriptor {
      size_t size;
      std::string name;
    };

    void create(void* allocator, const Descriptor& desc) {
      std::cout << "Creating buffer: " << desc.name 
                << " (size: " << desc.size << " bytes)\n";
    }
    
    void destroy(void* allocator, const Descriptor& desc) {
      std::cout << "Destroying buffer: " << desc.name << "\n";
    }
    
    void preRead(void* context, const Descriptor& desc, uint32_t flags) {
      std::cout << "Pre-read buffer: " << desc.name << "\n";
    }
    
    void preWrite(void* context, const Descriptor& desc, uint32_t flags) {
      std::cout << "Pre-write buffer: " << desc.name << "\n";
    }
  };

  // Define pass data structures
  struct GeometryPassData {
    NodeId color_target;
    NodeId depth_target;
    NodeId vertex_buffer;
  };

  struct ShadowPassData {
    NodeId shadow_map;
    NodeId depth_buffer;
  };

  struct LightingPassData {
    NodeId final_target;
    NodeId color_input;
    NodeId depth_input;
    NodeId shadow_input;
  };

  struct PresentPassData {
    NodeId backbuffer;
  };

  // Shadow map pass
  const auto& shadow_pass = fg.create_pass<ShadowPassData>(
    "Shadow Pass",
    [&](FrameGraph::Builder& builder, ShadowPassData& data) {
      // Create shadow map texture
      data.shadow_map = builder.create<FGTexture>("ShadowMap", {
        .width = 1024,
        .height = 1024,
        .format = 1, // DEPTH24
        .name = "ShadowMap"
      });

      // Create depth buffer for shadow pass
      data.depth_buffer = builder.create<FGTexture>("ShadowDepth", {
        .width = 1024,
        .height = 1024,
        .format = 1,
        .name = "ShadowDepth"
      });

      // Mark as outputs
      data.shadow_map = builder.write(data.shadow_map);
      data.depth_buffer = builder.write(data.depth_buffer);
    },
    [](FrameGraphResources& resources, void* context) {
      std::cout << "Executing Shadow Pass\n";
      // In a real implementation, this would:
      // 1. Set up shadow map render target
      // 2. Render scene from light's perspective
      // 3. Generate shadow map
    }
  );

  // Geometry pass (G-Buffer generation)
  const auto& geometry_pass = fg.create_pass<GeometryPassData>(
    "Geometry Pass",
    [&](FrameGraph::Builder& builder, GeometryPassData& data) {
      // Create color render target
      data.color_target = builder.create<FGTexture>("GBuffer_Color", {
        .width = 1920,
        .height = 1080,
        .format = 0,
        .name = "GBuffer_Color"
      });

      // Create depth buffer
      data.depth_target = builder.create<FGTexture>("GBuffer_Depth", {
        .width = 1920,
        .height = 1080,
        .format = 1,
        .name = "GBuffer_Depth"
      });

      // Create vertex buffer
      data.vertex_buffer = builder.create<FGBuffer>("VertexBuffer", {
        .size = 1024 * 1024,
        .name = "VertexBuffer"
      });

      // Mark as outputs
      data.color_target = builder.write(data.color_target);
      data.depth_target = builder.write(data.depth_target);
      data.vertex_buffer = builder.write(data.vertex_buffer);
    },
    [](FrameGraphResources& resources, void* context) {
      std::cout << "Executing Geometry Pass\n";
      // In a real implementation, this would:
      // 1. Set up G-Buffer render targets
      // 2. Render geometry to G-Buffer
      // 3. Generate albedo, normal, material properties
    }
  );

  // Lighting pass
  const auto& lighting_pass = fg.create_pass<LightingPassData>(
    "Lighting Pass",
    [&](FrameGraph::Builder& builder, LightingPassData& data) {
      // Create final color target
      data.final_target = builder.create<FGTexture>("FinalColor", {
        .width = 1920,
        .height = 1080,
        .format = 0,
        .name = "FinalColor"
      });

      // Read from previous passes
      data.color_input = builder.read(geometry_pass.color_target);
      data.depth_input = builder.read(geometry_pass.depth_target);
      data.shadow_input = builder.read(shadow_pass.shadow_map);

      // Mark final target as output
      data.final_target = builder.write(data.final_target);
    },
    [](FrameGraphResources& resources, void* context) {
      std::cout << "Executing Lighting Pass\n";
      // In a real implementation, this would:
      // 1. Read G-Buffer textures
      // 2. Read shadow map
      // 3. Compute lighting with shadows
      // 4. Output final lit color
    }
  );

  // Present pass
  const auto& present_pass = fg.create_pass<PresentPassData>(
    "Present Pass",
    [&](FrameGraph::Builder& builder, PresentPassData& data) {
      // Import external backbuffer
      data.backbuffer = fg.import<FGTexture>("Backbuffer", {
        .width = 1920,
        .height = 1080,
        .format = 0,
        .name = "Backbuffer"
      }, FGTexture{});

      // Read final color and write to backbuffer
      builder.read(lighting_pass.final_target);
      data.backbuffer = builder.write(data.backbuffer);
    },
    [](FrameGraphResources& resources, void* context) {
      std::cout << "Executing Present Pass\n";
      // In a real implementation, this would:
      // 1. Copy final color to backbuffer
      // 2. Present to screen
    }
  );

  std::cout << "\n=== Compiling Frame Graph ===\n";
  fg.compile();

  // Export visualizations
  std::cout << "\n=== Exporting Visualizations ===\n";
  fg.exportToDot("frame_graph.dot");
  fg.exportExecutionOrderToDot("frame_graph_execution.dot");
  std::cout << "Generated frame_graph.dot and frame_graph_execution.dot\n";
  std::cout << "To generate images, run:\n";
  std::cout << "  dot -Tpng frame_graph.dot -o frame_graph.png\n";
  std::cout << "  dot -Tpng frame_graph_execution.dot -o frame_graph_execution.png\n";

  std::cout << "\n=== Executing Frame Graph ===\n";
  void* allocator = nullptr;
  void* context = nullptr;
  fg.execute(context, allocator);

  std::cout << "\n=== Frame Graph Execution Complete ===\n";

  return 0;
}