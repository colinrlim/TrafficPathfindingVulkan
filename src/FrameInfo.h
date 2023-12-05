#pragma once

#include "EntityComponentSystem.h"
#include "Camera.h"

// lib
#include <vulkan/vulkan.h>

struct GlobalUbo {
	glm::mat4 projection{ 1.f };
	glm::mat4 view{ 1.f };
	glm::mat4 inverseView{ 1.f };
	glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f }; // w is intensity
	int timeSinceAnimationStart;
	int indexCount;
};

struct FrameInfo {
	int frameIndex;
	float frameTime;
	VkCommandBuffer commandBuffer;
	Camera& camera;
	VkDescriptorSet globalDescriptorSet;
	EntityComponentSystem& ecs;
	int nodes;
};