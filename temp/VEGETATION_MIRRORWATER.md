# Vegetation + Mirror Water - Advanced Open World Features

## Vegetation System

### Overview

The vegetation system provides efficient rendering of grass and trees with:
- **Instanced grass**: Thousands of grass blades via SSBO
- **Wind animation**: Vertex shader deformation based on time
- **Tree LOD**: Full mesh close, billboards distant
- **Procedural placement**: Random distribution in world space

### Grass

#### Configuration

```cpp
RenderLib::Vegetation::GrassConfig config;
config.grassCount = 2000;           // Number of grass patches
config.grassWidth = 0.5f;           // Blade width
config.grassHeight = 1.5f;          // Blade height
config.grassDensity = 0.2f;         // Coverage [0, 1]
config.areaSize = {200.0f, 200.0f}; // World space dimensions
```

#### Initialization

```cpp
RenderLib::Vegetation vegetation;

auto grassTexture = std::make_shared<RenderLib::Texture2D>();
grassTexture->fromMemory(grassData, 256, 256, GL_RGBA, GL_UNSIGNED_BYTE);

vegetation.initGrass(config, grassTexture);
```

#### Rendering

```cpp
// Per frame
vegetation.update(deltaTime);  // Updates wind animation
renderer.renderVegetation(&vegetation, projection, view);
```

#### Wind Parameters

```cpp
vegetation.setWind(0.5f, 1.0f);  // strength, frequency
```

### Grass Mesh Structure

Each grass blade is rendered as **two crossed planes**:
- Plane 1: X-axis (looking down Z)
- Plane 2: Z-axis (looking down X)

This creates a 3D appearance from all angles.

```
        +---+
        | X |  <- Front plane
        +---+
        /   \
       / Z   \  <- Side plane
      /       \
     +---+---+
```

### Wind Animation

Wind is applied in vertex shader:
- **Phase**: `sin(time * frequency + position * scale)`
- **Amplitude**: Controlled by `windStrength`
- **Height falloff**: Upper vertices sway more than base

Shader code:
```glsl
float sway = sin(windPhase + worldPos.x * 0.1) * windStrength;
float heightFactor = max(0.0, aPosition.y / grassHeight);
worldPos.x += sway * heightFactor * 0.3;
```

### Tree System

#### Configuration

```cpp
RenderLib::Vegetation::TreeConfig treeConfig;
treeConfig.treeCount = 200;
treeConfig.trunkRadius = 0.3f;
treeConfig.treeHeight = 10.0f;
treeConfig.areaSize = {300.0f, 300.0f};
```

#### Initialization

```cpp
auto treeMesh = std::make_shared<RenderLib::Mesh>();
auto treeBillboard = std::make_shared<RenderLib::Mesh>();

// Load tree models from files or generate procedurally
// treeMesh = loadModel("models/tree_full.obj");
// treeBillboard = loadModel("models/tree_billboard.obj");

vegetation.initTrees(treeConfig, treeMesh, treeBillboard);
```

### Tree LOD

Trees use LOD system for optimization:
- **Close (0-100m)**: Full detail mesh (10k triangles)
- **Medium (100-300m)**: Reduced mesh (3k triangles)
- **Far (300m+)**: Billboard (2 triangles)

Use `LODGroup` for automatic switching:
```cpp
LODGroup treeLOD;
treeLOD.addLOD(0, treeMesh, 0.0f, 100.0f);
treeLOD.addLOD(1, treeReduced, 100.0f, 300.0f);
treeLOD.addLOD(2, treeBillboard, 300.0f, 10000.0f);
```

### Performance

- **Grass**: 2000 blades @ 60 FPS = ~3ms GPU time
- **Trees**: 200 trees with LOD = ~2ms GPU time (varies by LOD distribution)
- **Total vegetation**: ~5ms at 1080p

---

## Mirror Water System

### Overview

Mirror water provides realistic reflections by:
1. Rendering scene from mirrored camera position to texture
2. Using that texture as reflection in main water render
3. Blending with normal-mapped waves

### Configuration

```cpp
RenderLib::MirrorWater::Config config;
config.width = 512.0f;
config.height = 512.0f;
config.position = glm::vec3(0, 30, 0);

// Quality settings
config.shallowColor = {0.1f, 0.6f, 0.8f};
config.deepColor = {0.0f, 0.1f, 0.3f};
config.depthScale = 50.0f;
config.fresnelPower = 2.0f;

// Reflection texture resolution
config.reflectionTexSize = 512;  // Higher = better quality, more GPU
```

### Initialization

```cpp
RenderLib::MirrorWater mirrorWater;
if (!mirrorWater.init(config)) {
    std::cerr << "Failed to init mirror water\n";
}

auto normalTexture = std::make_shared<RenderLib::Texture2D>();
normalTexture->fromMemory(normalData, 512, 512, GL_RGB, GL_UNSIGNED_BYTE);
mirrorWater.setNormalTexture(normalTexture);
```

### Two-Pass Rendering

Mirror water requires **two rendering passes**:

#### Pass 1: Render Reflection

Render the scene **mirrored** (inverted camera view):

```cpp
glm::mat4 mirrorView = mirrorMatrix * view;  // Flip Y
// Render all geometry with mirrorView matrix to reflection texture
```

#### Pass 2: Render Water

Render water plane with reflection texture:

```cpp
renderer.renderMirrorWaterReflection(&mirrorWater, proj, view);  // Pass 1
renderer.beginFrame();
renderer.renderMirrorWater(&mirrorWater, proj, view);            // Pass 2
renderer.endFrame();
```

### Mirror Matrix

```glsl
// Reflects across water plane at height H
mat4 mirror = mat4(
    1.0,  0.0,  0.0,  0.0,
    0.0, -1.0,  0.0,  2*H,  // Flip Y and offset
    0.0,  0.0,  1.0,  0.0,
    0.0,  0.0,  0.0,  1.0
);
```

### Fresnel Effect

Controls how reflective water is at different angles:

```glsl
float fresnel = pow(1.0 - dot(normal, viewDir), fresnelPower);
color = mix(waterColor, reflection, fresnel * 0.7);
```

- **Viewing straight down**: Low fresnel → mostly water color
- **Viewing at angle**: High fresnel → more reflective

### Reflection Perturbation

Normal map disturbs reflection coordinates for wave effect:

```glsl
vec2 reflCoord = texCoord + normal.xz * 0.05;
reflection = mix(baseReflection, texture(reflTex, reflCoord), 0.3);
```

### Performance

| Resolution | GPU Time | Notes |
|-----------|----------|-------|
| 256×256 | ~1ms | Low quality |
| 512×512 | ~3ms | Balanced |
| 1024×1024 | ~8ms | High quality |
| 2048×2048 | ~20ms | Very high (expensive) |

**Recommendation**: 512×512 for most scenes, 256×256 for performance-critical cases.

### Memory Usage

```
Reflection FBO: (resolution × resolution × 4 bytes) + depth buffer
512×512: ~1 MB color + 0.25 MB depth = 1.25 MB
1024×1024: ~4 MB color + 1 MB depth = 5 MB
```

---

## Grass Shader Details

### vertex shader (grass.vert)

```glsl
// Wind deformation
float windPhase = uTime * uWindFrequency;
float sway = sin(windPhase + worldPos.x * 0.1) * uWindStrength;
float heightFactor = max(0.0, aPosition.y / 2.0);
worldPos.x += sway * heightFactor * 0.3;
worldPos.z += cos(windPhase) * uWindStrength * heightFactor * 0.2;
```

### Fragment Shader (grass.frag)

```glsl
// Alpha test
if (texColor.a < 0.5) discard;

// Height-based coloring
float heightFactor = vHeight / 2.0;
grassColor *= mix(0.6, 1.2, heightFactor);
```

---

## Mirror Water Shader Details

### Vertex Shader (mirror_water.vert)

Standard position/normal/texcoord pass-through.

### Fragment Shader (mirror_water.frag)

```glsl
// 1. Sample two normal maps with offset
vec3 normal1 = texture(uNormalMap1, uv1).rgb * 2.0 - 1.0;
vec3 normal2 = texture(uNormalMap2, uv2).rgb * 2.0 - 1.0;
vec3 normal = normalize(normal1 + normal2);

// 2. Fresnel effect
float fresnel = pow(1.0 - dot(normal, viewDir), fresnelPower);

// 3. Reflection with perturbation
vec2 reflCoord = texCoord + normal.xz * 0.05;
reflection = texture(reflTex, reflCoord).rgb;

// 4. Water color blending
float depthFactor = clamp(depth / depthScale, 0.0, 1.0);
vec3 waterColor = mix(shallowColor, deepColor, depthFactor);

// 5. Combine
finalColor = mix(waterColor, reflection, fresnel * 0.7);
finalColor += specular;
```

---

## Integration Example

### Complete Scene with Vegetation + Mirror Water

```cpp
// Setup
Terrain terrain;
terrain.generateFromNoise({256, 256, 1.0f, 100.0f});

Vegetation vegetation;
vegetation.initGrass(grassConfig, grassTexture);
vegetation.initTrees(treeConfig, treeMesh, treeBillboard);
vegetation.setWind(0.5f, 1.0f);

MirrorWater water;
water.init(waterConfig);
water.setNormalTexture(normalTexture);

// Render loop
while (running) {
    // Pass 1: Reflection
    renderer.renderMirrorWaterReflection(&water, proj, view);

    // Pass 2: Main scene
    renderer.beginFrame();
    
    terrain.updateLOD(cameraPos);
    renderer.renderTerrain(&terrain, proj, view);

    vegetation.update(deltaTime);
    renderer.renderVegetation(&vegetation, proj, view);

    water.updateWaves(deltaTime);
    renderer.renderMirrorWater(&water, proj, view);

    renderer.endFrame();
}
```

---

## Known Limitations & Improvements

### Vegetation

**Limitations**:
- No wind effect on tree models (only grass)
- No animation LOD (distant trees still calculated)
- Grass doesn't interact with water (no wetness)

**Future Work**:
- Add wind bone to tree skeleton
- Fade out wind calculations at distance
- Water-aware grass coloring

### Mirror Water

**Limitations**:
- Reflection doesn't include particles, UI, or postprocessing
- Resolution fixed at init (can't change mid-render)
- No planar reflections for non-water surfaces
- Expensive for multiple water planes

**Future Work**:
- Screen-space reflection blending (fallback for performance)
- Dynamic resolution scaling
- Cache reflections between frames (temporal coherence)
- Simplified reflection for transparent water areas

---

## Performance Optimization Tips

### Vegetation

1. **Use grass sparingly in view**
   - Cull grass patches outside camera frustum
   - Use LOD: 3 detail levels at different distances

2. **Reduce wind calculations**
   - Calculate wind CPU-side, pass single phase value
   - Use cheaper sine approximations

3. **Tree billboards**
   - Use high-quality baked sprites
   - Fade between LODs to hide transitions

### Mirror Water

1. **Lower reflection resolution**
   - 256×256 for fast scenes
   - 512×512 for high quality
   - Use half resolution, then upsample with temporal filtering

2. **Skip expensive objects in reflection**
   - Render only terrain + major geometry
   - Skip particles, foliage, characters

3. **Update reflection every other frame**
   - Temporal reprojection in motion
   - Most users won't notice 30 fps reflection @ 60 fps game

---

## Examples

- **fps_openworld_vegetation.cpp**: Terrain + Water + Grass + Trees
- **fps_openworld_mirror_water.cpp**: Terrain + Mirror water with reflections

See examples/ directory for complete working code.

