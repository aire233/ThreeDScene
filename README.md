# README
# 3D Scene Rendering with Collision Detection and Post-Processing Filters

## Table of Contents

- [Introduction](#introduction)
- [Features](#features)
- [Technologies Used](#technologies-used)
- [Installation](#installation)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Scene Configuration](#scene-configuration)
- [Controls](#controls)
- [Collision Detection](#collision-detection)
- [Post-Processing Filters](#post-processing-filters)
- [Contributing](#contributing)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Introduction

This project is a 3D scene renderer developed using OpenGL, GLFW, GLAD, GLM, and ImGui. It showcases a dynamic environment with multiple 3D objects, including collision detection and post-processing effects. The scene is configurable through a `scene.txt` file, allowing easy addition and manipulation of objects.

## Features

- **Scene Construction**: Load and render multiple 3D objects (cubes and spheres) with customizable positions, colors, sizes, and dynamic properties.
- **Camera Controls**: Navigate the scene using keyboard (W, A, S, D) and mouse for an immersive experience.
- **Collision Detection**: Implements Axis-Aligned Bounding Box (AABB) collision detection to prevent dynamic objects from penetrating static objects.
- **Post-Processing Filters**: Applies an inversion filter to the rendered scene using framebuffers and shader programs.
- **User Interface**: Displays real-time collision notifications using ImGui.
- **Skybox**: Surround the scene with a skybox to enhance visual realism.

## Technologies Used

- **C++11**
- **OpenGL 3.3 Core Profile**
- **GLFW**: For window creation and input handling.
- **GLAD**: For managing OpenGL function pointers.
- **GLM**: Mathematics library for graphics software.
- **ImGui**: Immediate Mode GUI for real-time UI elements.
- **stb_image**: For loading textures.
- **CMake**: Build system generator.

## Installation

### Prerequisites

Ensure you have the following installed on your system:

- **CMake** (version 3.3 or higher)
- **GLFW** (3.3)
- **GLM**
- **OpenGL** development libraries
- **Git** (optional, for cloning the repository)

### Steps

1. **Clone the Repository**

   ```bash
   git clone https://github.com/yourusername/ThreeDScene.git
   cd ThreeDScene
	```

1. **Download Dependencies**

   Ensure that you have the necessary dependencies installed. On Ubuntu, you can install them using:

   ```bash
   sudo apt-get update
   sudo apt-get install libglfw3-dev libglm-dev cmake
   ```

2. **Build the Project**

   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **Run the Application**

   ```bash
   ./ThreeDScene
   ```

## Usage

### Scene Configuration

The scene is defined in the `scene.txt` file located at the root of the project. Each line represents an object with the following format:

```
bash


Copy code
type posX posY posZ colorR colorG colorB sizeX sizeY sizeZ isDynamic velX velY velZ
```

- **type**: `"cube"` or `"sphere"`
- **posX posY posZ**: Position coordinates of the object
- **colorR colorG colorB**: Color of the object (values between 0 and 1)
- **sizeX sizeY sizeZ**: Scale factors for the object
- **isDynamic**: `1` for dynamic (moving) objects, `0` for static objects
- **velX velY velZ**: Velocity vector (only applicable if `isDynamic` is `1`)

**Example `scene.txt`:**

```
sqlCopy codecube 0 0 0 1 0 0 1 1 1 0 0 0 0
cube 2 0 0 0 0 1 0 1 1 0 0 0 0
sphere 0 0 -3 0 0 1 1 1 1 0.7071 0 0.7071
```

### Controls

- **W**: Move forward
- **S**: Move backward
- **A**: Move left
- **D**: Move right
- **Mouse Movement**: Look around
- **ESC**: Exit application

### Collision Detection

When dynamic objects collide with static objects, a notification is displayed on the screen using ImGui.

### Post-Processing Filter

An inversion filter is applied to the rendered scene, altering the colors of all objects.

## Project Structure

```
cssCopy codeThreeDScene/
├── CMakeLists.txt
├── include/
│   ├── glad/
│   │   └── glad.h
│   ├── KHR/
│   │   └── khrplatform.h
│   ├── imgui/
│   │   ├── imgui.h
│   │   ├── imgui_internal.h
│   │   ├── imgui_impl_glfw.h
│   │   ├── imgui_impl_opengl3.h
│   │   └── ... (其他 ImGui 头文件)
│   └── stb_image.h
├── shaders/
│   ├── object_vertex.glsl
│   ├── object_fragment.glsl
│   ├── screen_vertex.glsl
│   ├── screen_fragment.glsl
│   ├── skybox_vertex.glsl
│   └── skybox_fragment.glsl
├── skybox/
│   ├── right.jpg
│   ├── left.jpg
│   ├── top.jpg
│   ├── bottom.jpg
│   ├── front.jpg
│   └── back.jpg
├── src/
│   ├── imgui/
│   │   ├── imgui.cpp
│   │   ├── imgui_draw.cpp
│   │   ├── imgui_tables.cpp
│   │   ├── imgui_widgets.cpp
│   │   ├── imgui_impl_glfw.cpp
│   │   └── imgui_impl_opengl3.cpp
│   ├── glad.c
│   └── main.cpp
├── scene.txt
└── README.md
```

## Scene Configuration

To customize the scene, edit the `scene.txt` file with desired objects.

**Example:**

```
sqlCopy code# Static cubes
cube 0 0 0 1 0 0 1 1 1 0 0 0 0
cube 2 0 0 0 0 1 0 1 1 0 0 0 0

# Dynamic sphere
sphere 0 0 -3 0 0 1 1 1 1 0.7071 0 0.7071
```

## Controls

- Camera Movement:

  - **W**: Move forward
  - **S**: Move backward
  - **A**: Move left
  - **D**: Move right
  
- Camera Orientation:

  - **Mouse Movement**: Look around

- Exit:

  - **ESC**: Exit the application

## Collision Detection

The application implements Axis-Aligned Bounding Box (AABB) collision detection. Dynamic objects (with `isDynamic = 1`) will move based on their velocity vectors. When a dynamic object collides with a static object, the movement is halted, and a collision notification is displayed via ImGui.

## Post-Processing Filters

The application utilizes framebuffers and shader programs to apply post-processing effects. Currently, an inversion filter is implemented, which inverts the colors of the rendered scene.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Acknowledgements

- [LearnOpenGL](https://learnopengl.com/) for excellent tutorials.
- [ImGui](https://github.com/ocornut/imgui) for the GUI library.
- [GLFW](https://www.glfw.org/) for window and input management.
- GLM for mathematics.
- [stb_image](https://github.com/nothings/stb) for image loading.

