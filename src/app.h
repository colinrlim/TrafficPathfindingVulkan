#pragma once

#include "Descriptors.h"
#include "Device.h"
#include "Renderer.h"
#include "Window.h"

// std
#include <memory>
#include <vector>

class App {
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	App();
	~App();

	App(const App&) = delete;
	App& operator=(const App&) = delete;

	void run();
private:
	void loadGameObjects();

	Window window{ WIDTH, HEIGHT, "Hello Vulkan!" };
	Device device{ window };
	Renderer renderer{ window, device };

	// note: order of declaration matters
	std::unique_ptr<DescriptorPool> globalPool{};
};