#include "SpatialSystemManager.h"

SpatialSystemManager::SpatialSystemManager(
	EntityComponentSystem& ecs, Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) {
	pathRenderSystem = ecs.registerSystem<PathRenderSystem>(device, renderPass, globalSetLayout);
	activePathRenderSystem = ecs.registerSystem<ActivePathRenderSystem>(device, renderPass, globalSetLayout);
	optimalPathRenderSystem = ecs.registerSystem<OptimalPathRenderSystem>(device, renderPass, globalSetLayout);

	Signature pathRenderSignature{};
	pathRenderSignature.set(ecs.getComponentType<ModelComponent>(), true);
	pathRenderSignature.set(ecs.getComponentType<TransformComponent>(), true);
	pathRenderSignature.set(ecs.getComponentType<InactiveComponent>(), true);
	ecs.setSystemSignature<PathRenderSystem>(pathRenderSignature);
	Signature activePathRenderSignature{};
	activePathRenderSignature.set(ecs.getComponentType<ModelComponent>(), true);
	activePathRenderSignature.set(ecs.getComponentType<TransformComponent>(), true);
	activePathRenderSignature.set(ecs.getComponentType<ActiveComponent>(), true);
	ecs.setSystemSignature<ActivePathRenderSystem>(activePathRenderSignature);
	Signature optimalPathRenderSignature{};
	optimalPathRenderSignature.set(ecs.getComponentType<ModelComponent>(), true);
	optimalPathRenderSignature.set(ecs.getComponentType<TransformComponent>(), true);
	optimalPathRenderSignature.set(ecs.getComponentType<OptimalComponent>(), true);
	ecs.setSystemSignature<OptimalPathRenderSystem>(optimalPathRenderSignature);
}

void SpatialSystemManager::update(FrameInfo& frameInfo, GlobalUbo& ubo) {
}

void SpatialSystemManager::render(FrameInfo& frameInfo) {
	pathRenderSystem->render(frameInfo);
	activePathRenderSystem->render(frameInfo);
	optimalPathRenderSystem->render(frameInfo);
}