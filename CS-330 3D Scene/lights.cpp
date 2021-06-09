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

void draw_radiant_light(RadiantLight light, glm::mat4 projection, glm::mat4 view) {
	using namespace glob;

	radiant_light_shader->use();

	glBindVertexArray(light.VAO);
	radiant_light_shader->setMat4("projection", projection);
	radiant_light_shader->setMat4("view", view);
	radiant_light_shader->setMat4("model", light.model);

	glDrawArrays(GL_TRIANGLES, 0, light.number_of_vertices);
}