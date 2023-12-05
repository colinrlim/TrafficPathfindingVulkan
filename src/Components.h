#pragma once

#include "Model.h"

// std
#include <functional>

// libs
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent {
	std::shared_ptr<Model> model;

	glm::vec3 translation{};
	glm::vec3 scale{ 1.f, 1.f, 1.f };
	glm::vec3 rotation{};

	// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
	// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
	// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
	glm::mat4 mat4();
	glm::mat3 normalMatrix();
};

struct ModelComponent {
	std::shared_ptr<Model> model;
};

struct InactiveComponent {

};

struct ActiveComponent {

};

struct OptimalComponent {

};