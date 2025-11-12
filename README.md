
# Paimon

## Overview
Paimon is a cross-platform C++20 library for modern OpenGL 4.6 development with a focus on efficient rendering, shader management, and scene graph organization.

## Features
- **Modern OpenGL 4.6** - Full support for DSA (Direct State Access), compute shaders, and advanced rendering techniques
- **Cross-Platform** - Windows (WGL), Linux (X11/Wayland/GLX), and headless EGL contexts
- **Frame Graph System** - Automatic render pass dependency management and resource lifetime optimization
- **Scene Graph** - Complete scene management with cameras, lights, materials, and mesh hierarchies
- **Shader Preprocessing** - Include system and shader variant generation
- **glTF 2.0 Support** - Full PBR material and model loading via tinygltf
- **Advanced OpenGL Abstractions** - Type-safe wrappers for buffers, textures, framebuffers, queries, and more

## Project Structure

```
paimon/
├── asset/          # Assets (models, shaders, textures)
├── example/        # Example applications demonstrating library features
├── external/       # Third-party dependencies (GLFW, GLM, Glad, spdlog, etc.)
├── source/         # Core library source code
│   └── paimon/     # Main library namespace
│       ├── core/       # Core systems (frame graph, scene graph, I/O)
│       ├── opengl/     # OpenGL abstraction layer
│       ├── platform/   # Platform-specific context creation
│       └── rendering/  # High-level rendering utilities
├── CMakeLists.txt  # Root CMake configuration
├── LICENSE         # MIT License
└── README.md       # This file
```

## Examples

The project includes comprehensive examples demonstrating various OpenGL features:

| Example | Description |
|---------|-------------|
| `hello_world` | Basic window creation and OpenGL context setup |
| `texture` | Texture loading and sampling |
| `framebuffer` | Offscreen rendering and post-processing |
| `geometry` | Geometry shader demonstrations |
| `compute_shader` | Compute shader usage and SSBO operations |
| `state` | OpenGL state management |
| `query` | Query objects for GPU timing and statistics |
| `debug_message` | OpenGL debug output callbacks |
| `separate_program` | Separable shader programs and program pipelines |
| `process_shader` | Shader preprocessing with includes |
| `frame_graph` | Frame graph system with automatic resource management |
| `damaged_helmet` | Full PBR rendering with glTF model loading |

## Build Instructions

### Prerequisites
- CMake 3.20 or higher
- C++20 compatible compiler (GCC 10+, Clang 11+, MSVC 2019+)
- OpenGL 4.6 compatible GPU and drivers
- X11 development libraries (Linux)

### Build Steps

1. Clone the repository with submodules:
   ```sh
   git clone https://github.com/jyxiong/paimon.git --recursive
   cd paimon
   ```

2. Create build directory and configure:
   ```sh
   mkdir build
   cd build
   cmake ..
   ```

3. Build the project:
   ```sh
   cmake --build . -j$(nproc)
   ```

4. Run examples:
   ```sh
   # Basic examples
   ./example/hello_world/hello_world
   ./example/texture/texture
   
   # Advanced examples
   ./example/frame_graph/frame_graph
   ./example/damaged_helmet/damaged_helmet
   ./example/compute_shader/compute_shader
   ```

### Build Options

- `BUILD_WITH_GLFW=ON/OFF` - Enable/disable GLFW window support (default: ON)
  - Set to OFF for headless EGL rendering on Linux

```sh
cmake .. -DBUILD_WITH_GLFW=OFF  # Headless mode
```

## Key Components

### Frame Graph
Automatic render pass ordering and resource lifetime management:
```cpp
fg::FrameGraph frame_graph;
fg::TransientResources resources;

auto& pass = frame_graph.AddPass("MyPass", [](fg::PassNode::Builder& builder) {
    auto color = builder.Create<fg::FrameGraphTexture>("ColorBuffer", desc);
    builder.Write(color);
}, [](const fg::PassNode::Resource& res, fg::RenderContext& ctx) {
    // Rendering code
});

frame_graph.Compile();
frame_graph.Execute(resources, render_context);
```

### Scene Graph
Hierarchical scene organization with PBR materials:
```cpp
sg::Scene scene;
auto& node = scene.CreateNode();
node.SetMesh(mesh);
node.SetMaterial(pbr_material);
node.SetTransform(transform);

sg::Camera camera(width, height);
camera.SetPerspective(45.0f, aspect, 0.1f, 100.0f);
```

### OpenGL Abstractions
Type-safe, RAII-based OpenGL wrappers:
```cpp
// Buffer creation
gl::Buffer vbo(gl::Buffer::Target::Array);
vbo.Storage(vertices.size() * sizeof(Vertex), vertices.data(), 0);

// Texture creation
gl::Texture2D texture;
texture.Storage(levels, GL_RGBA8, width, height);
texture.SubImage(0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

// Shader compilation and linking
gl::Shader vert(gl::Shader::Type::Vertex, vert_source);
gl::Shader frag(gl::Shader::Type::Fragment, frag_source);
gl::Program program({vert, frag});
```

### Shader Preprocessing
Include system for modular shader development:
```glsl
// common/lighting.glsl
vec3 CalculatePBR(/* params */) { /* ... */ }

// main.frag
#version 460 core
#include "common/lighting.glsl"

void main() {
    vec3 color = CalculatePBR(/* ... */);
    FragColor = vec4(color, 1.0);
}
```

## Dependencies
- [glad](https://github.com/Dav1dde/glad) - OpenGL function loader (4.6 Core)
- [glfw](https://github.com/glfw/glfw) - Window and input management
- [glm](https://github.com/g-truc/glm) - Mathematics library (vectors, matrices)
- [spdlog](https://github.com/gabime/spdlog) - Fast C++ logging library
- [tinygltf](https://github.com/syoyo/tinygltf) - glTF 2.0 loader
- [stb](https://github.com/nothings/stb) - Image loading (stb_image)
- [nlohmann/json](https://github.com/nlohmann/json) - JSON parsing

All dependencies are included as git submodules in the `external/` directory.

## Contributing
Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes with clear messages
4. Ensure code follows C++20 best practices
5. Add examples if introducing new features
6. Submit a pull request

For major changes, please open an issue first to discuss the proposal.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Reference & Resources

### OpenGL Documentation
- [OpenGL Wiki](https://wikis.khronos.org/opengl) - Official OpenGL documentation
- [History of OpenGL](https://wikis.khronos.org/opengl/History_of_OpenGL) - Evolution of the API
- [Guide to Modern OpenGL Functions](https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions) - DSA and modern patterns

### Rendering & Graphics
- [Learn OpenGL](https://learnopengl.com/) - Comprehensive OpenGL tutorials
- [OpenGL 4.6 Quick Reference](https://www.khronos.org/files/opengl46-quick-reference-guide.pdf) - API quick reference
- [PBR Theory](https://learnopengl.com/PBR/Theory) - Physically Based Rendering basics
- [glTF 2.0 Specification](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html) - Asset format specification

### Frame Graph Resources
- [FrameGraph: Extensible Rendering Architecture in Frostbite](https://www.gdcvault.com/play/1024612/) - GDC 2017 talk
- [Render Graphs and Vulkan](https://themaister.net/blog/2017/08/15/render-graphs-and-vulkan-a-deep-dive/) - Advanced concepts

---

**Author**: [jyxiong](https://github.com/jyxiong)  
**Repository**: [github.com/jyxiong/paimon](https://github.com/jyxiong/paimon)
