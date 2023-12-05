#pragma once

#include "Camera.h"
#include "Device.h"
#include "FrameInfo.h"
#include "Pipeline.h"
#include "System.h"

// std
#include <memory>
#include <vector>

class ActivePathRenderSystem : public System {
public:
	ActivePathRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : System{ device } {
		createPipelineLayout(globalSetLayout);
		createPipeline(renderPass);
	}

	void render(FrameInfo& frameInfo) override;
protected:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout) override;
	void createPipeline(VkRenderPass renderPass) override;
};