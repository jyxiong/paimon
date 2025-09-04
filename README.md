
# Paimon

## Overview
Paimon is a cross-platform C++20 library for modern OpenGL4.6 development.

## Features
- Modern OpenGL support
- Cross-platform (Windows, Linux, macOS)
- Easy integration with external libraries

## Structure

```
example/        # Example applications
external/       # Third-party libraries (GLFW, GLM, Glad, spdlog)
source/         # Core source code
```

## Build Instructions

1. Clone the repository:
	```sh
	git clone https://github.com/jyxiong/paimon.git --re
	```
2. Create a build directory and run CMake:
	```sh
	mkdir build
	cd build
	cmake ..
	cmake --build .
	```
3. Run example applications:
	```sh
	./example/hello_world/hello_world.exe
	./example/texture/texture.exe
	```

## Usage
- Modify or add source files in `source/`.
- Add new examples in `example/`.
- Link additional libraries in `external/` as needed.

## Dependencies
- [glad](https://github.com/Dav1dde/glad)
- [glfw](https://github.com/glfw/glfw)
- [glm](https://github.com/g-truc/glm)
- [spdlog](https://github.com/gabime/spdlog)

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request. For major changes, open an issue first to discuss your ideas.

## Reference
- [History of OpenGL](https://www.khronos.org/opengl/wiki/History_of_OpenGL)
- [Guide to Modern OpenGL Functions](https://github.com/fendevel/Guide-to-Modern-OpenGL-Functions)
- [OpenGL Wiki](https://www.khronos.org/opengl/wiki)
