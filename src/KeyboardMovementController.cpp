#include "KeyboardMovementController.h"

void KeyboardMovementController::moveInPlaneXZ(
		GLFWwindow* window, float dt, TransformComponent& transform) {
	glm::vec3 rotate{ 0 };

	if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
	if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
	if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
	if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

	if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
		transform.rotation += lookSpeed * dt * glm::normalize(rotate);
	}

	transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
	transform.rotation.y = glm::mod(transform.rotation.y, glm::two_pi<float>());

	float yaw = transform.rotation.y;
	const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
	const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
	const glm::vec3 upDir{ 0.f, -1.f, 0.f };

	glm::vec3 moveDir{ 0.f };
	if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
	if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
	if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
	if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
	if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
	if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

	if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
		transform.translation += moveSpeed * dt * glm::normalize(moveDir);
	}
}

void KeyboardMovementController::move(
	GLFWwindow* window, float dt, TransformComponent& transform) {
	glm::vec3 rotate{ 0 };

	if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
	if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
	if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
	if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

	if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
		transform.rotation += lookSpeed * dt * glm::normalize(rotate);
	}

	transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
	transform.rotation.y = glm::mod(transform.rotation.y, glm::two_pi<float>());

	float yaw = transform.rotation.y;
	float pitch = transform.rotation.x;
	const glm::vec3 forwardDir{
		sin(yaw) * cos(pitch),
		-sin(pitch),
		cos(yaw) * cos(pitch)
	};
	const glm::vec3 leftDir = glm::normalize(glm::cross(forwardDir, glm::vec3(0.f, 1.f, 0.f)));
	const glm::vec3 upDir = glm::vec3(0.f, 1.f, 0.f);

	glm::vec3 moveDir{ 0.f };
	if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
	if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
	if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir -= leftDir;
	if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir += leftDir;
	if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
	if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

	if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
		transform.translation += moveSpeed * dt * glm::normalize(moveDir);
	}
}