# RenderLib — C++23 OpenGL 3.3 Forward Rendering Engine
## With FPS Open World Extensions

Modern, modular rendering library for real-time graphics with emphasis on forward rendering optimization and large-scale terrain systems. Designed for FPS games with open-world environments.

## ✨ Features

### Core Rendering
- **Forward Rendering**: Single-pass lighting with Blinn-Phong model
- **Lighting**: Directional, Point, and Spot lights with separate classes
- **Texturing**: Diffuse, specular, normal maps with bump mapping
- **HDR Pipeline**: RGBA16F framebuffer with tone mapping
- **Shadow Mapping**: 2D + cubemap with PCF, cascade shadows for directional lights

### Advanced Shading
- **Parallax Occlusion Mapping**: Depth-aware UV offset (basic + layered variants)
- **Normal Mapping**: Full TBN-space with tangent calculation
- **Screen-Space Techniques**: SSAO, SSR, FXAA anti-aliasing
- **Postprocessing**: Bloom, volumetric fog, IBL
- **Skybox**: Cubemap with proper depth integration

### 🌍 Open World Systems (NEW)
- **Terrain**: Quad-tree LOD with up to 5 detail levels
- **Water**: Wave animation with Fresnel reflections and depth-based coloring
- **Mirror Water**: Realistic reflections with scene mirroring (NEW)
- **Vegetation**: Grass with wind animation + tree LOD (NEW)
- **Height-based Fog**: Atmospheric effects tied to terrain height
- **LOD Groups**: Distance-based mesh swapping (5 levels including billboards)

### ⚡ Performance Optimization
- **Frustum Culling**: CPU-side plane extraction and AABB testing
- **Hi-Z Occlusion Culling**: Hierarchical Z-buffer pyramid from depth
- **Instancing**: SSBO-based rendering for 10k+ instances
- **Transparency Sorting**: Depth-sorted render queue for correct blending
- **Object LOD**: Automatic quality reduction for distant objects

## 📁 Project Structure

```
RenderLib/
├── CMakeLists.txt                    # Build configuration
├── include/RenderLib/                # Public headers (~24 classes)
├── src/                              # Implementation files
├── shaders/                          # GLSL 3.3 shader collection (~15 shaders)
├── examples/                         # Complete working examples
│   ├── fps_openworld_a.cpp          (Terrain + Water)
│   └── fps_openworld_c.cpp          (Occlusion + LOD)
├── README.md                         # This file
├── ADVANCED_TECHNIQUES.md            # Deep dive documentation
├── FPS_OPENWORLD_INTEGRATION.md      # Setup guide
└── FPS_OPENWORLD_SUMMARY.md          # System overview
```

## 🚀 Quick Start

### Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

### 1. Terrain + Water Scene

```cpp
#include "RenderLib/RenderLib.hpp"

// Initialize
RenderLib::ForwardRenderer renderer;
renderer.init(1920, 1080);

// Create terrain
RenderLib::Terrain terrain;
terrain.generateFromNoise({256, 256, 1.0f, 100.0f});

// Create water
RenderLib::Water water;
water.init({512.0f, 512.0f, 1.0f, {0, 20, 0}});

// Render loop
while (running) {
    renderer.beginFrame();
    
    terrain.updateLOD(cameraPos);
    renderer.renderTerrain(&terrain, projection, view);
    
    water.updateWaves(deltaTime);
    renderer.renderWater(&water, projection, view);
    
    renderer.endFrame();
}
```

### 2. LOD System with Culling

```cpp
// Create LOD group
RenderLib::LODGroup lod;
lod.addLOD(0, meshFull,      0.0f,  50.0f);
lod.addLOD(1, meshMedium,   50.0f, 150.0f);
lod.addLOD(2, meshLow,     150.0f, 300.0f);
lod.addLOD(3, meshBillboard, 300.0f, 2000.0f);

// Per frame
renderer.enableFrustumCulling(true);
renderer.updateFrustum(viewProj);

for (auto& obj : objects) {
    if (!renderer.isMeshInFrustum(obj.aabbMin, obj.aabbMax)) continue;
    if (renderer.isOccluded(obj.aabbMin, obj.aabbMax)) continue;
    
    float dist = glm::distance(obj.pos, cameraPos);
    Mesh* selectedLOD = lod.selectLOD(dist);
    renderer.submitMesh(*selectedLOD, obj.material, obj.transform);
}
```

### 3. Instancing

```cpp
RenderLib::InstanceBuffer buffer;
buffer.create(10000);

std::vector<RenderLib::InstanceData> instances;
for (int i = 0; i < 1000; ++i) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), positions[i]);
    instances.push_back({model, glm::vec4(1.0f)});
}

buffer.updateData(instances);
buffer.bindSSBO(3);
renderer.submitInstanced(mesh, material, positions);
```

## 📊 Performance Characteristics

| Feature | CPU Cost | GPU Cost | Typical Use |
|---------|----------|----------|------------|
| Frustum Culling | ~0.1ms/1000 | 0 | Always-on |
| Hi-Z Occlusion | ~0.2ms/1000 | Depth read | Large terrain |
| Terrain LOD | <0.05ms | Decimated geom | Outdoor scenes |
| Water | <0.01ms | 2-3 texture reads | Single plane |
| Instancing | <0.01ms | 1 draw call | Vegetation, crowds |
| SSAO | 0 | ~2ms@1080p | Ambient shadows |
| Bloom | 0 | ~3ms@1080p | Bright objects |

## 🎓 Key Classes

| Class | Purpose | Header |
|-------|---------|--------|
| `ForwardRenderer` | Main rendering system | `Renderer.hpp` |
| `Terrain` | Quad-tree LOD terrain | `Terrain.hpp` |
| `Water` | Wave-simulated water | `Water.hpp` |
| `HiZBuffer` | Occlusion culling | `HiZBuffer.hpp` |
| `LODGroup` | Distance-based LOD | `LODGroup.hpp` |
| `Frustum` | Frustum culling | `Frustum.hpp` |
| `InstanceBuffer` | GPU instancing | `Instancing.hpp` |
| `RenderQueue` | Render order mgmt | `RenderQueue.hpp` |
| `LightDirectional` | Directional light | `LightDirectional.hpp` |
| `LightPoint` | Point light | `LightPoint.hpp` |
| `LightSpot` | Spot light | `LightSpot.hpp` |

## 📚 Documentation

- **[ADVANCED_TECHNIQUES.md](ADVANCED_TECHNIQUES.md)** - Instancing, culling, transparency, deferred details
- **[FPS_OPENWORLD_INTEGRATION.md](FPS_OPENWORLD_INTEGRATION.md)** - Terrain, water, LOD setup guide
- **[FPS_OPENWORLD_SUMMARY.md](FPS_OPENWORLD_SUMMARY.md)** - Complete system architecture
- **[VEGETATION_MIRRORWATER.md](VEGETATION_MIRRORWATER.md)** - Grass/tree system and mirror water reflections

## 💾 Requirements

- **C++23** compiler (MSVC 2022+, GCC 13+, Clang 16+)
- **OpenGL 3.3+** capable GPU
- **CMake 3.16+**

### Dependencies

- **glad** - OpenGL loader (included)
- **glm** - Math library (header-only)
- **glfw3** - Window management (for examples)

## 🔧 Configuration Examples

### Terrain

```cpp
Terrain::Config config;
config.gridWidth = 256;        // Vertices in X
config.gridHeight = 256;       // Vertices in Z
config.tileSize = 1.0f;        // World units per vertex
config.maxHeight = 100.0f;     // Heightfield range
config.tileResolution = 32;    // Vertices per tile
```

### Water

```cpp
Water::Config config;
config.width = 512.0f;
config.height = 512.0f;
config.shallowColor = {0.1f, 0.6f, 0.8f};  // Light blue
config.deepColor = {0.0f, 0.1f, 0.3f};     // Dark blue
config.depthScale = 50.0f;
config.fresnelPower = 2.0f;
```

## ⚙️ Advanced Usage

### Terrain LOD Levels
```
LOD 0: 32×32 vertices (1024 triangles)
LOD 1: 16×16 vertices (256 triangles)
LOD 2: 8×8 vertices (64 triangles)
LOD 3: 4×4 vertices (16 triangles)
LOD 4: 2×2 vertices (4 triangles)
```

### Hi-Z Occlusion Pipeline
```
1. Build Hi-Z pyramid from depth buffer
2. Project AABB to screen space
3. Sample Hi-Z pyramid at object bounds
4. Conservative test: skip if definitely occluded
```

### Object LOD Distances
```
Close    (0-50m):     Full detail model (50k triangles)
Medium   (50-150m):   50% decimation (25k triangles)
Far      (150-300m):  25% decimation (10k triangles)
Very Far (300-500m):  10% decimation (5k triangles)
Distant  (500m+):     Billboard imposter (2 triangles)
```

## 🐛 Known Limitations

- Terrain LOD transitions can "pop" (no morphing)
- Water reflections use depth proxy (not full scene)
- Hi-Z uses conservative occlusion (may pass occluded)
- Maximum 4 lights per type (forward rendering limit)

## 📖 Examples

See `examples/` for complete working code:

- **fps_openworld_a.cpp**: Terrain generation, water, FPS camera with gravity
- **fps_openworld_c.cpp**: Culling demo, LOD selection, performance stats
- **fps_openworld_vegetation.cpp**: Terrain + water + grass + trees with wind
- **fps_openworld_mirror_water.cpp**: Terrain + mirror water with reflections

## 🎯 Roadmap

**Completed**
- ✅ Forward rendering core
- ✅ All light types + shadows
- ✅ Advanced shading (parallax, SSAO, bloom, SSR)
- ✅ Performance optimization (frustum, instancing, LOD)
- ✅ Open world systems (terrain, water)

**Future**
- 🔲 Compute shader LOD generation
- 🔲 GPU-side Hi-Z building
- 🔲 Terrain tessellation
- 🔲 Water reflections (mirror pass)
- 🔲 Vegetation system (grass)
- 🔲 Imposter/billboard generation
- 🔲 Animation LOD
- 🔲 PBR materials (optional)

## License

RenderLib is provided as-is for educational and commercial use.

---

**Version**: 2.0 (FPS Open World)  
**Last Updated**: November 2025  
**C++ Standard**: C++23  
**Graphics API**: OpenGL 3.3+

