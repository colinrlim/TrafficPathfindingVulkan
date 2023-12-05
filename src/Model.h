#pragma once

#include "Texture.h"
#include "Buffer.h"
#include "Device.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include "MapGraph.h"

#include <memory>
#include <vector>

class Model {
public:
	struct Vertex {
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		bool operator==(const Vertex& rhs) const {
			return position == rhs.position && color == rhs.color && normal == rhs.normal &&
				uv == rhs.uv;
		}
	};

	struct Data {
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
		std::vector<std::unique_ptr<Texture>> textures{};

		void loadEdges(const std::vector<Edge*>& edges, glm::vec3 color, float width);
	};

	Model(Device& device, const Model::Data& data);
	~Model();

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	static std::unique_ptr<Model> createModelFromEdges(Device& device, const std::vector<Edge*>& edges, glm::vec3 color, float width);

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);
private:
	void createVertexBuffers(const std::vector<Vertex>& vertices);
	void createIndexBuffers(const std::vector<uint32_t>& indices);

	Device& device;

	std::unique_ptr<Buffer> vertexBuffer;
	uint32_t vertexCount;

	bool hasIndexBuffer = false;
	std::unique_ptr<Buffer> indexBuffer;
	uint32_t indexCount;
};