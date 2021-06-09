#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include "lights.h"

namespace glob {
	Shader* radiant_light_shader = nullptr;
};

struct vertex {
	float x, y, z;
	float r, g, b;
};

void lights_init()
{
	glob::radiant_light_shader = new Shader("shaders/radiant_light.vs.glsl", "shaders/radiant_light.fs.glsl");
}