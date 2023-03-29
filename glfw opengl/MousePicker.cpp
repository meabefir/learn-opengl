#pragma once
#include "MousePicker.hpp"

#include "model.hpp"

bool MousePicker::intersectsModel(Model* model) {
	/*std::cout << world_ray.x << ' ' << world_ray.y << ' ' << world_ray.z << '\n';
	std::cout << camera->Position.x << ' ' << camera->Position.y << ' ' << camera->Position.z << '\n';
	*/
	return (model->intersectsSegment(camera->Position, world_ray) || model->snap_idx != -1);
}