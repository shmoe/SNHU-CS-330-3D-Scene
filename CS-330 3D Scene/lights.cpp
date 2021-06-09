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

RadiantLight get_point_light() {
	RadiantLight point_light;

	vertex point[1] = {
		0.f, 0.f, 0.f,
		1.f, 1.f, 1.f
	};
	const int floats_per_vertex = 3;
	const int floats_per_color = 3;
	int stride = floats_per_vertex + floats_per_color;

	glm::vec3 position = glm::vec3(point[0].x, point[0].y, point[0].z);
	glm::vec3 color = glm::vec3(point[0].r, point[0].g, point[0].b);

	point_light.position = position;
	point_light.color = color;
	point_light.number_of_vertices = 1;

	unsigned VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(point),
		&point[0],
		GL_STATIC_DRAW);

	glVertexAttribPointer(0, floats_per_vertex,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, floats_per_color,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)(sizeof(float) * floats_per_vertex));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	point_light.VAO = VAO;

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-1.f, 1.f, -1.f)); // back left corner of scene;
	
	point_light.model = model;

	return point_light;
};

void draw_radiant_light(RadiantLight light, glm::mat4 projection, glm::mat4 view) {
	using namespace glob;

	radiant_light_shader->use();

	glBindVertexArray(light.VAO);
	radiant_light_shader->setMat4("projection", projection);
	radiant_light_shader->setMat4("view", view);
	radiant_light_shader->setMat4("model", light.model);

	glDrawArrays(GL_TRIANGLES, 0, light.number_of_vertices);
}