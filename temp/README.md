# RenderLib — Forward Rendering Library (C++23 + OpenGL 3.3)

A minimal, header-rich rendering library for forward rendering using OpenGL 3.3 and C++23. Designed as a library-only (no window management) with support for advanced techniques like shadow mapping, normal mapping, bloom, and screen-space reflections.

## Features

- **Forward Rendering**: Single-pass lighting with Blinn-Phong model
- **Lighting**: Up to 4 directional, 4 point, and 4 spot lights (separate classes per light type)
- **Texturing**: Diffuse, specular, and normal maps with bump mapping support
- **Shadow Mapping**:
  - Directional & Spot lights: 2D depth maps with PCF filtering
  - Point lights: Cubemap depth with soft shadows
  - All calculations in fragment shader (no vertex attributes needed)
- **Volumetric Fog**: Distance-based with configurable height zone
- **Postprocessing**:
  - HDR rendering with Reinhard tone mapping
  - Bloom (bright-pass extraction + separable Gaussian blur)
  - Screen-Space Reflections (SSR approximation)
- **Efficient**: Uses `std140` layout UBOs, sampler objects, and minimal CPU-GPU sync

## Project Structure

```
RenderLib/
├── CMakeLists.txt
├── include/RenderLib/
│   ├── Shader.hpp
│   ├── Texture.hpp
│   ├── Sampler.hpp
│   ├── UBO.hpp
│   ├── Mesh.hpp
│   ├── Renderer.hpp
│   ├── LightDirectional.hpp
│   ├── LightPoint.hpp
│   └── LightSpot.hpp
├── src/
│   ├── Shader.cpp
│   ├── Texture.cpp
│   ├── Sampler.cpp
│   ├── UBO.cpp
│   ├── Mesh.cpp
│   ├── Renderer.cpp
│   ├── LightDirectional.cpp
│   ├── LightPoint.cpp
│   └── LightSpot.cpp
└── shaders/
    ├── basic.vert / basic.frag        (main forward pass)
    ├── shadow_dir.vert / shadow_depth.frag
    ├── shadow_point.vert / shadow_depth.frag
    ├── blur.frag                       (separable Gaussian)
    ├── extract_bright.frag             (bright-pass for bloom)
    ├── post_bloom.frag                 (tone map + blend)
    └── ssr.frag                        (screen-space reflections)
```

## Building

### Prerequisites

- **C++23 compiler** (MSVC, Clang, GCC with C++23 support)
- **glad**: OpenGL loader (you provide source/headers)
- **GLM**: Header-only math library
- **CMake** 3.15 or later

### Build Steps

```bash
mkdir build
cd build
cmake -S .. -B . -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

## Usage

Since this is a library-only package, you must:

1. **Create an OpenGL context** (via GLFW, SDL, or your framework)
2. **Initialize glad**: `gladLoadGL()`
3. **Compile shaders** and load textures
4. **Create lights** and initialize shadow maps
5. **Use ForwardRenderer** to render

### Minimal Example (Pseudocode)

```cpp
#include "RenderLib/Renderer.hpp"
#include "RenderLib/LightDirectional.hpp"
#include "RenderLib/Mesh.hpp"
#include <glm/glm.hpp>

int main() {
    // Assume OpenGL context and glad initialized
    
    // Initialize renderer with window dimensions
    RenderLib::ForwardRenderer renderer;
    renderer.init(1920, 1080);
    
    // Create a directional light and initialize shadow map
    RenderLib::DirectionalLight dirLight;
    dirLight.direction = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    dirLight.color = glm::vec3(1.0f);
    dirLight.intensity = 1.0f;
    dirLight.initShadowMap();  // creates 2048x2048 depth map
    
    // Load shader
    RenderLib::Shader mainShader;
    mainShader.compileFromFiles("shaders/basic.vert", "shaders/basic.frag");
    
    // Create mesh (example: cube)
    std::vector<RenderLib::Vertex> vertices = { /* ... */ };
    std::vector<unsigned int> indices = { /* ... */ };
    RenderLib::Mesh mesh;
    mesh.create(vertices, indices);
    
    // Create material with textures
    RenderLib::Texture2D diffuseTex;
    diffuseTex.fromMemory(512, 512, GL_RGB, diffuse_data);
    
    RenderLib::Material material;
    material.diffuse = &diffuseTex;
    material.specular = &specularTex;
    material.normal = &normalTex;
    material.shininess = 32.0f;
    
    // Rendering loop
    while(/* not done */) {
        renderer.beginFrame();
        
        mainShader.use();
        // Set view/proj matrices, camera position, light UBO data
        glUniformMatrix4fv(locView, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(locProj, 1, GL_FALSE, &proj[0][0]);
        glUniform3fv(locViewPos, 1, &camPos.x);
        
        // Submit mesh for rendering
        glm::mat4 model = glm::mat4(1.0f);
        renderer.submitMesh(mesh, material, model);
        
        renderer.endFrame();
    }
    
    return 0;
}
```

## UBO Layout (std140)

The `Lights` uniform block (binding point 1) follows this layout:

```glsl
layout(std140) uniform Lights {
    DirLight dirLights[4];      // 32 bytes each (vec3 + 1 pad, vec3 + intensity)
    PointLight pointLights[4];  // 48 bytes each
    SpotLight spotLights[4];    // 64 bytes each
    mat4 dirLightMatrices[4];   // 64 bytes each
    mat4 spotLightMatrices[4];  // 64 bytes each
};
```

Update this UBO via:
```cpp
RenderLib::UBO lights;
lights.create("Lights", 1, 16 * 1024);
lights.update(offset, size, data);
```

## Notes

- **No window creation**: Consumers must provide OpenGL context
- **Image loading**: Use `stb_image` (not bundled) to load textures
- **Shadowing**: All shadow-map generation and sampling is CPU-managed and shader-executed
- **Postprocessing**: Bloom and SSR are post-passes over the HDR color buffer
- **Extensibility**: Shader sources are in `shaders/` directory for easy customization

## License

Public Domain / Unlicensed (MIT compatible)
