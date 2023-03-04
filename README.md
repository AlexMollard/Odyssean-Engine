# Odyssean-Engine

Odyssean-Engine is a 2D/3D game engine built using C++ and OpenGL/Vulkan API. This engine is designed to provide a modular and flexible framework for game development.

## Contents

The repository contains the following directories and files:

- `.gitignore` - specifies files and directories that Git should ignore
- `README.md` - provides an overview of the repository
- `Resources/Fonts` - contains FiraCode and Manrope fonts used for rendering text
- `Resources/Images` - contains debug images used for testing
- `Resources/Meshes` - contains cube and knot meshes used for testing
- `Resources/Shaders` - contains shader source files and compiled SPIR-V files used for rendering
- `UIEngine` - contains the source code for the engine
- `media/Audio/Desktop` - contains audio files used for sound effects and music

## Features

- 2D and 3D rendering support
- Physically Based Rendering (PBR) in Vulkan API
- Modular architecture for easy customization
- Entity Component System (ECS) architecture for flexible game object management
- ImGui-based debug interface for live updates
- Support for importing 3D models with the Assimp library

## Usage

To use Odyssean-Engine, you will need to install the necessary dependencies using vcpkg:

1. Clone the repository
2. Install vcpkg by following the instructions on the [vcpkg GitHub page](https://github.com/microsoft/vcpkg)
3. Open the `UIEngine.sln` solution file in Visual Studio
4. Build the solution
5. Run the executable

## Contributing

We welcome contributions from the community. If you'd like to contribute to the project, please feel free to just pull its all public.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.

