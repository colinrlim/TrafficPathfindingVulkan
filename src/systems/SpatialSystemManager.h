#pragma once

#include "PathRenderSystem.h"
#include "ActivePathRenderSystem.h"
#include "OptimalPathRenderSystem.h"

// std
#include <memory>
#include <vector>

class SpatialSystemManager {
public:
	SpatialSystemManager(
		EntityComponentSystem& ecs, Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);

	void update(FrameInfo& frameInfo, GlobalUbo& ubo);
	void render(FrameInfo& frameInfo);
private:
	std::shared_ptr<PathRenderSystem> pathRenderSystem;
	std::shared_ptr<ActivePathRenderSystem> activePathRenderSystem;
	std::shared_ptr<OptimalPathRenderSystem> optimalPathRenderSystem;
};