#pragma once

#include "Pipeline.h"

// std
#include <memory>
#include <set>

struct GlobalUbo;
struct FrameInfo;

class System {
public:
	System(Device& device) : device{ device }{ }
	~System() {
		vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
	}

	System(const System&) = delete;
	System& operator=(const System&) = delete;

	virtual void update(FrameInfo& frameInfo, GlobalUbo& ubo) {};
	virtual void render(FrameInfo& frameInfo) {};

	std::set<Entity> entities;
protected:
	virtual void createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {};
	virtual void createPipeline(VkRenderPass renderPass) {};

	Device& device;

	std::unique_ptr<Pipeline> pipeline;
	VkPipelineLayout pipelineLayout;
};