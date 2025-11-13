# FPS Open World Rendering - Integration Guide

## Overview
This module extends RenderLib with terrain, water, and optimization systems for large open-world FPS games.

### Phase A: Terrain + Water
- **Terrain**: Quad-tree LOD system with 5 levels per tile
- **Water**: Reflection + normal-mapped wave animation
- **Fog**: Height-based exponential fog

### Phase C: Occlusion Culling + Object LOD
- **Hi-Z Occlusion**: Hierarchical Z-buffer pyramid for visibility testing
- **LOD Groups**: Distance-based mesh switching (5 levels: full → billboard)
- **Combined Culling**: Frustum + occlusion for maximum performance

---

## A) Terrain + Water

### Terrain Class

#### Initialization
```cpp
RenderLib::Terrain terrain;
RenderLib::Terrain::Config config;
config.gridWidth = 256;        // Vertices in X
config.gridHeight = 256;       // Vertices in Z
config.tileSize = 1.0f;        // World unit per vertex
config.maxHeight = 100.0f;     // Heightfield range
config.tileResolution = 32;    // Vertices per tile (always square)

// Generate from noise
terrain.generateFromNoise(config);

// OR load from array
std::vector<float> heightmap(256*256);
// ... fill heightmap
terrain.generateFromHeightmap(config, heightmap);
```

#### Set Textures
```cpp
auto diffuse = std::make_shared<RenderLib::Texture2D>();
diffuse->fromMemory(imageData, width, height, GL_RGB, GL_UNSIGNED_BYTE);
terrain.setDiffuseTexture(diffuse);

auto normalMap = std::make_shared<RenderLib::Texture2D>();
normalMap->fromMemory(normalData, width, height, GL_RGB, GL_UNSIGNED_BYTE);
terrain.setNormalTexture(normalMap);
```

#### Update & Render
```cpp
// Per frame
terrain.updateLOD(cameraPos);  // Select LOD for each tile based on distance

// Rendering (in render loop)
renderer.renderTerrain(&terrain, projection, view);

// Query height at any world position
float heightAtPos = terrain.getHeightAt(worldX, worldZ);
```

#### Terrain Structure
```
Total Grid: 256x256 vertices
  ↓
Tiles (32x32 vertices each) = 8x8 tiles
  ↓
Each Tile has 5 LODs:
  - LOD 0: 32x32 (decimation=1)
  - LOD 1: 16x16 (decimation=2)
  - LOD 2: 8x8   (decimation=4)
  - LOD 3: 4x4   (decimation=8)
  - LOD 4: 2x2   (decimation=16)
```

**Rendering Cost**: Decimation = fewer vertices, fewer draw calls for distant tiles

### Water Class

#### Initialization
```cpp
RenderLib::Water water;
RenderLib::Water::Config config;
config.width = 512.0f;
config.height = 512.0f;
config.position = glm::vec3(0, 20, 0);
config.shallowColor = glm::vec3(0.1f, 0.6f, 0.8f);   // Light blue
config.deepColor = glm::vec3(0.0f, 0.1f, 0.3f);       // Dark blue
config.depthScale = 50.0f;   // Distance until fully deep
config.fresnelPower = 2.0f;  // Reflection intensity vs viewing angle

water.init(config);
```

#### Set Textures
```cpp
auto normalMap = std::make_shared<RenderLib::Texture2D>();
normalMap->fromMemory(normalData, 512, 512);
water.setNormalTexture(normalMap);

auto reflectionTex = std::make_shared<RenderLib::Texture2D>();
water.setReflectionTexture(reflectionTex);

auto depthTex = std::make_shared<RenderLib::Texture2D>();
water.setDepthTexture(depthTex);  // Scene depth for depth-based coloring
```

#### Update & Render
```cpp
// Per frame - update wave scrolling
water.updateWaves(deltaTime, 0.1f);  // Speed = 0.1 units/sec

// Rendering
renderer.renderWater(&water, projection, view);
```

#### Water Features
- **Normal Maps**: Two layers with different scroll speeds (create wave effect)
- **Fresnel**: Edge of water more reflective than center
- **Depth Blending**: Shallow areas lighter, deep areas darker
- **Specular**: Simple directional highlight

### Fog Integration

Add to terrain shader:
```glsl
// Height-based exponential fog
float heightDiff = vWorldPos.y - uFogHeight;
float fogFactor = exp(-uFogDensity * heightDiff);
fogFactor = clamp(fogFactor, 0.0, 1.0);

vec3 finalColor = mix(uFogColor, radiance, fogFactor);
```

Set in C++:
```cpp
glm::vec3 fogColor(0.7, 0.8, 1.0);  // Light blue
float fogDensity = 0.01f;
float fogHeight = 0.0f;  // Fog base level
```

---

## C) Occlusion Culling + Object LOD

### Hi-Z Buffer

#### Build from Depth
```cpp
// After rendering scene, read back depth buffer
std::vector<float> depthData(width * height);
glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depthData.data());

// Build Hi-Z pyramid
renderer.updateHiZ(depthData.data());
```

#### Pyramid Structure
```
Level 0: 1920x1080 (full resolution, max depth sampled)
Level 1: 960x540   (2x decimation)
Level 2: 480x270   (4x decimation)
Level 3: 240x135   (8x decimation)
Level 4: 120x68    (16x decimation)
... continues until 1x1
```

**Memory**: ~1.3x original depth buffer (small overhead)

#### Visibility Testing
```cpp
glm::vec3 aabbMin(-5, 0, -5);
glm::vec3 aabbMax(5, 10, 5);

bool visible = !renderer.isOccluded(aabbMin, aabbMax);
if (visible) {
    // Render object
}
```

### LOD Groups

#### Create LOD Levels
```cpp
RenderLib::LODGroup lodGroup;

// Load mesh models at different detail levels
auto meshFull = loadMesh("model_lod0.obj");     // 50k triangles
auto mesh50pct = loadMesh("model_lod1.obj");   // 25k triangles
auto mesh25pct = loadMesh("model_lod2.obj");   // 10k triangles
auto mesh10pct = loadMesh("model_lod3.obj");   // 5k triangles
auto meshBillboard = loadMesh("model_billboard.obj");  // 2 triangles (quad)

// Assign to LOD group with distance ranges
lodGroup.addLOD(0, meshFull, 0.0f, 50.0f);          // 0-50m: full detail
lodGroup.addLOD(1, mesh50pct, 50.0f, 150.0f);       // 50-150m: 50%
lodGroup.addLOD(2, mesh25pct, 150.0f, 300.0f);      // 150-300m: 25%
lodGroup.addLOD(3, mesh10pct, 300.0f, 500.0f);      // 300-500m: 10%
lodGroup.addLOD(4, meshBillboard, 500.0f, 2000.0f); // 500+m: billboard
```

#### Select LOD
```cpp
float distanceToCamera = glm::distance(objectPos, cameraPos);
RenderLib::Mesh* selectedMesh = lodGroup.selectLOD(distanceToCamera);

if (selectedMesh) {
    renderer.submitMesh(*selectedMesh, material, model);
}
```

#### LOD Performance Impact
```
Close (LOD 0):     50k triangles × 1000 objects = 50M tri/frame → bottleneck
Medium (LOD 2):    10k triangles × 1000 objects = 10M tri/frame → fine
Far (LOD 4):       2 triangles × 1000 objects = 2k tri/frame → negligible
```

### Combined Culling Pipeline

#### Per-Frame Visibility
```cpp
for (auto& object : scene) {
    // 1. Frustum culling (CPU, ~0.1ms per 1000 objects)
    if (!renderer.isMeshInFrustum(object.aabbMin, object.aabbMax)) {
        continue;  // Skip off-screen
    }

    // 2. Occlusion culling (CPU, ~0.2ms per 1000 objects)
    if (renderer.isOccluded(object.aabbMin, object.aabbMax)) {
        continue;  // Skip behind hills/buildings
    }

    // 3. Select LOD (CPU, ~0.01ms)
    float dist = glm::distance(object.pos, cameraPos);
    Mesh* mesh = object.lods.selectLOD(dist);

    // 4. Submit for rendering
    renderer.submitMesh(*mesh, object.material, object.transform);
}
```

#### Expected Performance
- **Without culling**: 1000 objects = 1000 draw calls, ~30M triangles
- **With frustum**: 500 visible = 500 draw calls, ~15M triangles (30% faster)
- **With frustum + occlusion**: 250 visible = 250 draw calls, ~8M triangles (70% faster)
- **With LOD**: 250 visible with LOD = 250 draws, ~3M triangles (90% faster)

### Imposter/Billboard Technique

For very distant objects, replace mesh with 2-triangle billboard:

```glsl
// In distant LOD vertex shader
gl_Position = projection * view * vec4(objectPos, 1.0);

// Billboard faces camera
normal = -viewDir;
```

Vertex shader for LOD 4 (billboard):
```glsl
// Create quad perpendicular to view
vec3 right = normalize(cross(viewDir, worldUp));
vec3 up = normalize(cross(right, viewDir));

vec3 vertexWorldPos = objectPos + right * uvx * scale + up * uvy * scale;
gl_Position = projection * view * vec4(vertexWorldPos, 1.0);
```

Use high-quality sprite texture of object for convincing appearance.

---

## Integration Checklist

### For Terrain + Water (Phase A)
- [ ] Load terrain heightmap or use procedural generation
- [ ] Create diffuse and normal map textures
- [ ] Set up terrain shader (terrain.vert/frag)
- [ ] Call `terrain.updateLOD()` each frame
- [ ] Call `renderer.renderTerrain()` each frame
- [ ] Implement water reflection rendering (mirror matrix)
- [ ] Set water normal maps with scrolling
- [ ] Call `water.updateWaves()` each frame
- [ ] Call `renderer.renderWater()` each frame

### For Occlusion + LOD (Phase C)
- [ ] Read back depth buffer each frame
- [ ] Call `renderer.updateHiZ()` with depth data
- [ ] Create LODGroup for each object type
- [ ] Load LOD meshes (or generate LODs from full model)
- [ ] Call `selectLOD()` based on distance
- [ ] Test occlusion before rendering
- [ ] Profile performance (culling CPU cost vs GPU savings)

---

## Shader Files Summary

| Shader | Purpose |
|--------|---------|
| `terrain.vert/frag` | Terrain rendering with LOD, normal mapping, height fog |
| `water.vert/frag` | Water plane with reflections, waves, Fresnel |
| `instanced.vert/frag` | Instanced rendering via SSBO (reuse from Phase 5) |
| `transparent.frag` | Alpha-blended objects (reuse from Phase 5) |

---

## Known Limitations & TODOs

1. **Hi-Z Precision**: Current implementation uses conservative test; real occlusion might pass visible objects behind hills
   - TODO: Implement screen-space AABB projection, finer Hi-Z sampling

2. **Water Reflections**: Currently placeholder; should render scene reflected
   - TODO: Implement mirror view matrix, render to reflection texture

3. **Terrain Transitions**: LOD transitions can cause popping
   - TODO: Implement morph targets, smooth vertex blending between LODs

4. **Material Blending**: Terrain transitions abruptly between textures
   - TODO: Add triplanar texturing or height-based layer blending

5. **Animation LOD**: Objects far away should skip skeletal animation
   - TODO: Add animation LOD property to LODGroup

---

## Example Scene

See `examples/fps_openworld_a.cpp` and `examples/fps_openworld_c.cpp` for complete examples.

