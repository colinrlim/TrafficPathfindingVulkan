#include "app.h"

#include "KeyboardMovementController.h"
#include "Buffer.h"
#include "Camera.h"
#include "SpatialSystemManager.h"

#include "Texture.h"
#include "MapGraph.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include "pathfinding.h"
#include <unordered_set>

App::App() {
    globalPool = DescriptorPool::Builder(device)
        .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
        .build();

	loadGameObjects();
}

App::~App() {}

bool leftClickPressed = false;
bool rightClickPressed = false;
double lastX, lastY;

// GLFW callback function for mouse button events
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftClickPressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        } else if (action == GLFW_RELEASE) {
            //leftClickPressed = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightClickPressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        }
        else if (action == GLFW_RELEASE) {
            rightClickPressed = false;
        }
    }
}

void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (rightClickPressed) {
        const float sensitivity = 0.002f;

        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        float deltaX = static_cast<float>(xpos - lastX);
        float deltaY = static_cast<float>(ypos - lastY);

        TransformComponent& viewerObjectTransform = ecs_global->getComponent<TransformComponent>(viewer_obj_global);
        viewerObjectTransform.rotation.x = glm::clamp(viewerObjectTransform.rotation.x - deltaY * sensitivity, -1.5f, 1.5f);
        viewerObjectTransform.rotation.y = glm::mod(viewerObjectTransform.rotation.y + deltaX * sensitivity, glm::two_pi<float>());
        glfwSetCursorPos(window, lastX, lastY);
    }
}

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1*, T2*>& pair) const {
        auto hash1 = std::hash<T1*>{}(pair.first);
        auto hash2 = std::hash<T2*>{}(pair.second);
        return hash1 ^ hash2;
    }
};

void App::run() {
    EntityComponentSystem ecs{};
    Entity viewerObject{};

    std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<Buffer>(
            device,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        uboBuffers[i]->map();
    }

    auto globalSetLayout = DescriptorSetLayout::Builder(device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
        .addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
        .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }


    ecs.init();

    ecs.registerComponent<TransformComponent>();
    ecs.registerComponent<ModelComponent>();
    ecs.registerComponent<InactiveComponent>();
    ecs.registerComponent<ActiveComponent>();
    ecs.registerComponent<OptimalComponent>();

    SpatialSystemManager spatialSystemManager{ ecs, device, renderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

    MapGraph graph{ "files/map.osm" };

    PathfindingSolution solution{};
    solution.endTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    std::cout << graph.edges.size() << " " << graph.nodes.size() << std::endl;

    Entity map = ecs.createEntity();
    std::shared_ptr<Model> model = Model::createModelFromEdges(device, graph.edges, glm::vec3(45.f / 2550.f, 12.f / 2550.f, 12.f / 2550.f), 0.0015f);
    TransformComponent mapTransform{};
    mapTransform.translation = { 0.f, 0.f, 0.f };
    mapTransform.scale = glm::vec3{ 1.f, 1.f, 1.f };
    ecs.addComponent<ModelComponent>(map, { model });
    ecs.addComponent<TransformComponent>(map, mapTransform);
    ecs.addComponent<InactiveComponent>(map, { });

    Node* from = nullptr;
    Node* to = nullptr;

    Entity activePaths = ecs.createEntity();
    Entity optimalPath = ecs.createEntity();
    bool activePathsCreated = true;
    bool optimalPathCreated = true;

    Camera camera{};

    viewerObject = ecs.createEntity();
    TransformComponent viewerObjectTransform{};
    viewerObjectTransform.translation.z = -2.5f;
    ecs.addComponent<TransformComponent>(viewerObject, viewerObjectTransform);

    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    // Set GLFW callbacks
    glfwSetMouseButtonCallback(window.getGLFWwindow(), mouseButtonCallback);
    glfwSetCursorPosCallback(window.getGLFWwindow(), cursorPosCallback);

    TransformComponent& cameraTransform = ecs.getComponent<TransformComponent>(viewerObject);

	while (!window.shouldClose()) {
		glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        //frameTime = glm::min(frameTime, MAX_FRAME_TIME);
        cameraController.move(window.getGLFWwindow(), frameTime, cameraTransform);
        camera.setViewYXZ(cameraTransform.translation, cameraTransform.rotation);

        float aspect = renderer.getAspectRatio();
        // camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

		if (auto commandBuffer = renderer.beginFrame()) {
            int frameIndex = renderer.getFrameIndex();
            FrameInfo frameInfo{
                frameIndex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSets[frameIndex],
                ecs};

            // update
            GlobalUbo ubo{};
            ubo.projection = camera.getProjection();
            ubo.view = camera.getView();
            ubo.inverseView = camera.getInverseView();
            ubo.timeSinceAnimationStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - solution.endTimestamp;

            if (leftClickPressed) {
                leftClickPressed = false;

                glm::vec4 mouseClipSpace(1.0f - 2.0f * lastX / window.getWidth(),
                    1.0f - 2.0f * lastY / window.getHeight(),
                    1.0f, 1.0f);

                glm::vec4 mouseCameraSpace = glm::inverse(ubo.projection) * mouseClipSpace;
                mouseCameraSpace.z = 1.0f;
                mouseCameraSpace.w = 0.0f;

                glm::vec3 mouseWorldSpace = glm::inverse(ubo.view) * mouseCameraSpace;

                glm::vec3 cameraPos = glm::inverse(ubo.view)[3];

                float t = cameraPos.z / mouseWorldSpace.z;
                glm::vec3 worldPoint = cameraPos + t * mouseWorldSpace;
                worldPoint.z = 0.0;

                Node* closestNode = graph.nodes.front();
                double closestDist = glm::distance(glm::vec3(closestNode->x, closestNode->y, 0.0), worldPoint);
                for (size_t i = 1; i < graph.nodes.size(); i++) {
                    Node* node = graph.nodes[i];
                    const double dist = glm::distance(glm::vec3(node->x, node->y, 0.0), worldPoint);
                    if (dist < closestDist) {
                        closestNode = node;
                        closestDist = dist;
                    }
                }

                if ((from == nullptr && to == nullptr) || (from != nullptr && to != nullptr && to != closestNode)) {
                    from = closestNode;
                    to = nullptr;

                    if (activePathsCreated) {
                        ecs.destroyEntity(activePaths);
                        activePathsCreated = false;
                    }
                    if (optimalPathCreated) {
                        ecs.destroyEntity(optimalPath);
                        optimalPathCreated = false;
                    }
                } else if (from != nullptr && to == nullptr && from != closestNode) {
                    to = closestNode;

                    solution = pathfinding::dijkstra(graph, from, to);
                    ubo.indexCount = solution.checked.size() * 6;

                    model = Model::createModelFromEdges(device, solution.checked, glm::vec3(248.f / 2550.f, 201.f / 2550.f, 38.f / 2550.f), 0.002f);
                    activePaths = ecs.createEntity();
                    activePathsCreated = true;
                    TransformComponent activePathsTransform{};
                    activePathsTransform.translation = { 0.f, 0.f, -0.0001f };
                    activePathsTransform.scale = glm::vec3{ 1.f, 1.f, 1.f };
                    ecs.addComponent<ModelComponent>(activePaths, { model });
                    ecs.addComponent<TransformComponent>(activePaths, activePathsTransform);
                    ecs.addComponent<ActiveComponent>(activePaths, { });

                    if (solution.path.size() > 0) {
                        model = Model::createModelFromEdges(device, solution.path, glm::vec3(1.f, 1.f, 1.f), 0.0025f);
                        optimalPath = ecs.createEntity();
                        optimalPathCreated = true;
                        TransformComponent optimalPathTransform{};
                        optimalPathTransform.translation = { 0.f, 0.f, -0.0002f };
                        optimalPathTransform.scale = glm::vec3{ 1.f, 1.f, 1.f };
                        ecs.addComponent<ModelComponent>(optimalPath, { model });
                        ecs.addComponent<TransformComponent>(optimalPath, optimalPathTransform);
                        ecs.addComponent<OptimalComponent>(optimalPath, { });
                    }

                    solution.endTimestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                }
            }

            spatialSystemManager.update(frameInfo, ubo);
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            // render
			renderer.beginSwapChainRenderPass(commandBuffer);
            
            // order here matters
            spatialSystemManager.render(frameInfo);

			renderer.endSwapChainRenderPass(commandBuffer);
			renderer.endFrame();
		}
	}

	vkDeviceWaitIdle(device.device());

    ecs.clear();
}

void App::loadGameObjects() {
}