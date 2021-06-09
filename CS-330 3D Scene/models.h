#pragma once
#ifndef __MODELS_H__
#define __MODELS_H__

#include <glm/glm.hpp>

#include "shader.h"

struct tex_mesh {
	unsigned int texture;
	unsigned int texture_offset;
	unsigned int VAO;
	unsigned int number_of_vertices;
	glm::mat4 model;
};
typedef struct tex_mesh Model;


void models_init();

Model get_desk_model(const char* texture_path);

Model get_switch_model(const char* texture_path);

Model get_orange_model(const char* texture_path);

Model get_napkin_model(const char* texture_path);

Model get_soda_model(const char* texture_path);

void draw_model(Model model, glm::mat4 projection, glm::mat4 view, glm::vec3 lightPos, glm::vec3 lightColor);
#endif//__MODELS_H__