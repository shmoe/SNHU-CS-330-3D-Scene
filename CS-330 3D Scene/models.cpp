#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "models.h"

namespace glob {
	Shader* universal_shader = nullptr;
	unsigned int number_of_textures = 0;
}

void models_init() {
	glob::universal_shader = new Shader("shaders/single_texture.vs.glsl", "shaders/single_texture.fs.glsl");
}

unsigned int load_wrap_texture(const char* texture_path) {
	unsigned int texture;

	glGenTextures(1, &texture);																// Generate texture and set texture1 to new texture's ID number
	glBindTexture(GL_TEXTURE_2D, texture);														// Bind texture1 to the context as a GL_TEXTURE_2D

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);								// Set wrapping parameters for bound texture (texture1)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);								// Set wrapping parameters for bound texture (texture1)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);														// Tell stb_image.h to flip image on y-axis (because of XY -> rowcol shenanigans)
	unsigned char* img_data = stbi_load(texture_path, &width, &height, &nrChannels, 0);	// Load raw image data to img_data and populate width, height and nrChannels
	if (img_data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, img_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}																							// On sucess, bind img_data to currently bound texture (texture1)
	else
	{
		std::cerr << "ERROR::TEXTURE::DATA::LOADING_FAILED" << std::endl;
	}																							// On sucess, print to stderr
	stbi_image_free(img_data);																	// Free img_data from RAM, as it has been copied to VRAM

	return texture;
}

Model get_desk_model(const char* texture_path) {
	Model plane;

	/**
	 * Set up VAO and number of vertices
	 */
	const float plane_vertices[] = {
	-1.0f, 0.0f, 1.0f,		0.0f, 0.0f,	// Front left
	-1.0f, 0.0f, -1.0f,		0.0f, 1.0f,	// Back left, brown
	1.0f, 0.0f, -1.0f,		1.0f, 1.0f,	// Back right, brown
	1.0f, 0.0f, -1.0f,		1.0f, 1.0f,	// Back right, brown
	-1.0f, 0.0f, 1.0f,		0.0f, 0.0f,	// Front left, brown
	1.0f, 0.0f, 1.0f,		1.0f, 0.0f	// Front right, brown
	};

	const int floats_per_vertex = 3;
	const int floats_per_texture = 2;
	const int stride = floats_per_texture + floats_per_vertex;

	unsigned int plane_VAO, plane_VBO;

	glGenVertexArrays(1, &plane_VAO);					// Generate a VAO and set plane_VAO to the new VAO's ID number
	glGenBuffers(1, &plane_VBO);						// Generate a VBO and set plane_VBO to the new VBO's ID number

	glBindVertexArray(plane_VAO);						// Bind the VAO to the context, which saves the following function calls.

	glBindBuffer(GL_ARRAY_BUFFER, plane_VBO);			// Bind the VBO to the context (and by extension the currently bound VAO).
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(plane_vertices),
		plane_vertices,
		GL_STATIC_DRAW);								// Copy the data from vertices to the VBO.

	glVertexAttribPointer(0, floats_per_vertex,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float), (void*)0);				// Tells the context (and by extension the VAO) how to read the first attribute of the vertex buffer.
	glEnableVertexAttribArray(0);						// Enable the above vertex attribute array.

	glVertexAttribPointer(1, floats_per_texture,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)(sizeof(float) * floats_per_vertex));	// Tells the context (and by extension the VAO) how to read the second attribute of the vertex buffer.
	glEnableVertexAttribArray(1);						// Enable the above vertex attribute array.

	glBindVertexArray(0);								// Unbind the VAO from the context.

	plane.VAO = plane_VAO;								// Assign VAO handle to Model
	plane.number_of_vertices = sizeof(plane_vertices)
							/ (sizeof(float) * stride);	// Assign number_of_vertices to Model

	/**
	 * Define plane model matrix.
	 */
	glm::mat4 plane_model = glm::mat4(1.0f);														// Initially set as identity matrix.
	plane_model = glm::translate(plane_model, glm::vec3(0.0f, -0.065f, 0.0f));						// Center model
	//plane_model = glm::rotate(plane_model, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// Rotate model
	plane_model = glm::scale(plane_model, glm::vec3(2.0f, 1.0f, 1.0f));								// Scale model

	plane.model = plane_model;																		// Assign model to Model

	/**
	 * set up texture
	 */
	unsigned int plane_texture = load_wrap_texture(texture_path);									// Generate texture with wrapping attributes

	glob::universal_shader->use();																	// Bind universal shader to context
	glob::universal_shader->setInt("texture", 0);													// Tell shader where to find texture (TODO find out if value of "0" works with multiple textures loaded)

	plane.texture = plane_texture;

	return plane;
}

void draw_model(Model model, glm::mat4 projection, glm::mat4 view) {
	using namespace glob;

	glActiveTexture(GL_TEXTURE0);	//TODO see if this works with multiple textures loaded, see other TODO
	glBindTexture(GL_TEXTURE_2D, model.texture);

	universal_shader->use();

	glBindVertexArray(model.VAO);
	universal_shader->setMat4("projection", projection);
	universal_shader->setMat4("view", view);
	universal_shader->setMat4("model", model.model);

	glDrawArrays(GL_TRIANGLES, 0, model.number_of_vertices);
}