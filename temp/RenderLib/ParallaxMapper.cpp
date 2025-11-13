#include "RenderLib/ParallaxMapper.hpp"

namespace RenderLib {

bool ParallaxMapper::globallyEnabled_ = true;

ParallaxMapper::ParallaxMapper() = default;

ParallaxMapper::~ParallaxMapper() = default;

bool ParallaxMapper::isEnabled() {
    return globallyEnabled_;
}

void ParallaxMapper::setEnabled(bool enable) {
    globallyEnabled_ = enable;
}

} // namespace RenderLib
