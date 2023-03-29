#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "camera.h"
#include "constants.hpp"

class Model;

class MousePicker {
public:
	glm::vec3 currentRay;
	glm::vec3 world_ray;

	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	float rayLength = 10000.f;

	Camera* camera = nullptr;

	MousePicker(Camera* camera, glm::mat4 projection) {
		this->camera = camera;
		projectionMatrix = projection;
		viewMatrix = camera->GetViewMatrix();
	}

	void update(float mouse_x, float mouse_y) {
		viewMatrix = camera->GetViewMatrix();
		currentRay = calculateMouseRay(mouse_x, mouse_y);
		world_ray = currentRay * rayLength + camera->Position;
	}

	glm::vec3 calculateMouseRay(float mouse_x, float mouse_y) {
		glm::vec2 norm_coords = getNormalizedDeviceCoords(mouse_x, mouse_y);
		glm::vec4 clip_coords = glm::vec4(norm_coords.x, norm_coords.y, -1.f, 1.f);
		glm::vec4 eye_coords = to_eye_coords(clip_coords);
		glm::vec3 world_ray = to_world_coords(eye_coords);
		return world_ray;
	}

	glm::vec3 to_world_coords(glm::vec4 eye_coords) {
		glm::mat4 inverted = glm::inverse(viewMatrix);
		// here
		glm::vec4 ray_world = inverted * eye_coords;
		return glm::normalize(ray_world);
	}

	glm::vec4 to_eye_coords(glm::vec4 clip_coords) {
		glm::mat4 inverted = glm::inverse(projectionMatrix);
		// here
		glm::vec4 eye_coords = inverted * clip_coords;
		return glm::vec4(eye_coords.x, eye_coords.y, -1.f, 0.f);
	}

	glm::vec2 getNormalizedDeviceCoords(float x, float y) {
		float _x = (2.f * x) / (float)SCR_WIDTH - 1.f;
		float _y = (2.f * y) / (float)SCR_HEIGHT - 1.f;
		// here
		return glm::vec2(_x, -_y);
	}

	bool intersectsModel(Model* model);
};
