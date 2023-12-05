#pragma once

#include "Device.h"
#include "SwapChain.h"
#include "Window.h"

// std
#include <memory>
#include <vector>
#include <cassert>

class Renderer {
public:
	Renderer(Window& window, Device& device);
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	bool isFrameInProgress() const { return isFrameStarted; }

	VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
	float getAspectRatio() const { return swapChain->extentAspectRatio(); }
	VkCommandBuffer getCurrentCommandBuffer() const {
		assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
		return commandBuffers[currentFrameIndex];
	}

	int getFrameIndex() const {
		assert(isFrameStarted && "Cannot get frame index when frame not in progress");
		return currentFrameIndex;
	}

	VkCommandBuffer beginFrame();
	void endFrame();
	void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
	void createCommandBuffers();
	void freeCommandBuffers();
	void recreateSwapChain();

	Window& window;
	Device& device;
	std::unique_ptr<SwapChain> swapChain;
	std::vector<VkCommandBuffer> commandBuffers;

	uint32_t currentImageIndex;
	int currentFrameIndex;
	bool isFrameStarted{ false };
};