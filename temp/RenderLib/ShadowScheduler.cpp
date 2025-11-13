#include "RenderLib/ShadowScheduler.hpp"
#include "RenderLib/LightDirectional.hpp"
#include "RenderLib/LightPoint.hpp"
#include "RenderLib/LightSpot.hpp"
#include <iostream>

namespace RenderLib {

ShadowScheduler::ShadowScheduler() {}
ShadowScheduler::~ShadowScheduler() {}

uint32_t ShadowScheduler::generateLightID_() {
    return nextLightID_++;
}

ShadowScheduler::UpdateFrequency ShadowScheduler::getFrequencyFromPriority_(Priority p) const {
    switch (p) {
        case Priority::Critical: return UpdateFrequency::EveryFrame;
        case Priority::High: return UpdateFrequency::EveryFrame;
        case Priority::Medium: return UpdateFrequency::EveryOtherFrame;
        case Priority::Low: return UpdateFrequency::Every6Frames;
    }
    return UpdateFrequency::EveryFrame;
}

void ShadowScheduler::registerDirectionalLight(DirectionalLight* light, Priority priority) {
    if (!light) return;
    
    uint32_t id = generateLightID_();
    ShadowEntry entry;
    entry.lightID = id;
    entry.priority = priority;
    entry.frequency = getFrequencyFromPriority_(priority);
    entry.needsUpdate = true;
    entry.isVisible = true;
    
    lightIDToIndex_[id] = entries_.size();
    entries_.push_back(entry);
}

void ShadowScheduler::registerPointLight(PointLight* light, Priority priority) {
    if (!light) return;
    
    uint32_t id = generateLightID_();
    ShadowEntry entry;
    entry.lightID = id;
    entry.priority = priority;
    entry.frequency = getFrequencyFromPriority_(priority);
    entry.needsUpdate = true;
    entry.isVisible = true;
    
    lightIDToIndex_[id] = entries_.size();
    entries_.push_back(entry);
}

void ShadowScheduler::registerSpotLight(SpotLight* light, Priority priority) {
    if (!light) return;
    
    uint32_t id = generateLightID_();
    ShadowEntry entry;
    entry.lightID = id;
    entry.priority = priority;
    entry.frequency = getFrequencyFromPriority_(priority);
    entry.needsUpdate = true;
    entry.isVisible = true;
    
    lightIDToIndex_[id] = entries_.size();
    entries_.push_back(entry);
}

void ShadowScheduler::setLightVisible(uint32_t lightID, bool visible) {
    auto it = lightIDToIndex_.find(lightID);
    if (it != lightIDToIndex_.end()) {
        entries_[it->second].isVisible = visible;
    }
}

void ShadowScheduler::update(uint32_t frameCount) {
    frameCount_ = frameCount;
    stats_.totalScheduledLights = entries_.size();
    stats_.shadowUpdatesThisFrame = 0;
    stats_.estimatedGPUTimeSavings = 0.0f;

    for (auto& entry : entries_) {
        entry.needsUpdate = false;

        // Always update visible critical lights
        if (entry.priority == Priority::Critical && entry.isVisible) {
            entry.needsUpdate = true;
            entry.framesSinceUpdate = 0;
            stats_.shadowUpdatesThisFrame++;
            continue;
        }

        // Skip invisible lights
        if (!entry.isVisible) {
            entry.framesSinceUpdate++;
            continue;
        }

        // Check frequency-based scheduling
        uint32_t updateInterval = 1;
        switch (entry.frequency) {
            case UpdateFrequency::EveryFrame: updateInterval = 1; break;
            case UpdateFrequency::EveryOtherFrame: updateInterval = 2; break;
            case UpdateFrequency::Every4Frames: updateInterval = 4; break;
            case UpdateFrequency::Every6Frames: updateInterval = 6; break;
        }

        if (entry.framesSinceUpdate >= updateInterval - 1) {
            entry.needsUpdate = true;
            entry.framesSinceUpdate = 0;
            stats_.shadowUpdatesThisFrame++;
        } else {
            entry.framesSinceUpdate++;
        }
    }

    // Estimate GPU time savings
    if (stats_.totalScheduledLights > 0) {
        float potentialUpdates = stats_.totalScheduledLights;
        float actualUpdates = stats_.shadowUpdatesThisFrame;
        stats_.estimatedGPUTimeSavings = (1.0f - actualUpdates / potentialUpdates) * 100.0f;
    }
}

bool ShadowScheduler::shouldUpdateShadow(uint32_t lightID) const {
    auto it = lightIDToIndex_.find(lightID);
    if (it != lightIDToIndex_.end()) {
        return entries_[it->second].needsUpdate;
    }
    return false;
}

void ShadowScheduler::forceUpdate(uint32_t lightID) {
    auto it = lightIDToIndex_.find(lightID);
    if (it != lightIDToIndex_.end()) {
        entries_[it->second].needsUpdate = true;
        entries_[it->second].framesSinceUpdate = 0;
    }
}

}  // namespace RenderLib
