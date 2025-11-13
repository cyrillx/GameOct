#include "RenderLib/IrradianceVolume.hpp"
#include <iostream>

namespace RenderLib {

IrradianceVolume::IrradianceVolume() = default;

IrradianceVolume::~IrradianceVolume() {
    destroy();
}

bool IrradianceVolume::init(const Config& cfg) {
    config_ = cfg;
    totalProbes_ = cfg.probeCounts.x * cfg.probeCounts.y * cfg.probeCounts.z;
    cpuData_.assign(totalProbes_ * 3, 0.0f);

    glGenTextures(1, &irradianceTex_);
    glBindTexture(GL_TEXTURE_3D, irradianceTex_);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, cfg.probeCounts.x, cfg.probeCounts.y, cfg.probeCounts.z, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_3D, 0);
    return true;
}

void IrradianceVolume::destroy() {
    if (irradianceTex_) glDeleteTextures(1, &irradianceTex_);
    irradianceTex_ = 0;
    cpuData_.clear();
}

void IrradianceVolume::setProbeIrradiance(int x, int y, int z, const glm::vec3& rgb) {
    if (x < 0 || y < 0 || z < 0) return;
    if (x >= config_.probeCounts.x || y >= config_.probeCounts.y || z >= config_.probeCounts.z) return;
    int idx = (z * config_.probeCounts.y * config_.probeCounts.x + y * config_.probeCounts.x + x) * 3;
    cpuData_[idx + 0] = rgb.r;
    cpuData_[idx + 1] = rgb.g;
    cpuData_[idx + 2] = rgb.b;
}

void IrradianceVolume::uploadToGPU() {
    if (!irradianceTex_) return;
    glBindTexture(GL_TEXTURE_3D, irradianceTex_);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, config_.probeCounts.x, config_.probeCounts.y, config_.probeCounts.z, GL_RGB, GL_FLOAT, cpuData_.data());
    glBindTexture(GL_TEXTURE_3D, 0);
}

void IrradianceVolume::bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_3D, irradianceTex_);
}

} // namespace RenderLib
