#pragma once

// Core rendering
#include "RenderLib/Shader.hpp"
#include "RenderLib/Texture2D.hpp"
#include "RenderLib/Mesh.hpp"
#include "RenderLib/Material.hpp"
#include "RenderLib/Renderer.hpp"

// Lighting
#include "RenderLib/LightDirectional.hpp"
#include "RenderLib/LightPoint.hpp"
#include "RenderLib/LightSpot.hpp"
#include "RenderLib/CascadeShadow.hpp"

// Optimization
#include "RenderLib/Frustum.hpp"
#include "RenderLib/RenderQueue.hpp"
#include "RenderLib/Instancing.hpp"
#include "RenderLib/HiZBuffer.hpp"
#include "RenderLib/LODGroup.hpp"

// Open World
#include "RenderLib/Terrain.hpp"
#include "RenderLib/Water.hpp"
#include "RenderLib/MirrorWater.hpp"
#include "RenderLib/Vegetation.hpp"

// Phase 6: Advanced Features
#include "RenderLib/VolumetricLighting.hpp"
#include "RenderLib/DecalSystem.hpp"
#include "RenderLib/ShadowScheduler.hpp"
#include "RenderLib/WindSystem.hpp"
#include "RenderLib/TemporalSSAO.hpp"
#include "RenderLib/DynamicResolution.hpp"
#include "RenderLib/TemporalFilter.hpp"
#include "RenderLib/ImposterGenerator.hpp"
