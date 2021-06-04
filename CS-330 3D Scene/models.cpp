#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <vector>
#define PI 3.141596

#include "models.h"

namespace glob {
	Shader* universal_shader = nullptr;
	unsigned int number_of_textures = 0;
}

struct vertex {
	float x, y, z;
	float nx, ny, nz;
	float s, t;
};

void models_init() {
	glob::universal_shader = new Shader("shaders/single_texture.vs.glsl", "shaders/single_texture.fs.glsl");
}

unsigned int load_wrap_texture(const char* texture_path) {
	unsigned int texture;

	glGenTextures(1, &texture);																	// Generate texture and set texture1 to new texture's ID number
	glBindTexture(GL_TEXTURE_2D, texture);														// Bind texture1 to the context as a GL_TEXTURE_2D

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);								// Set wrapping parameters for bound texture (texture1)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);								// Set wrapping parameters for bound texture (texture1)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);														// Tell stb_image.h to flip image on y-axis (because of XY -> rowcol shenanigans)
	unsigned char* img_data = stbi_load(texture_path, &width, &height, &nrChannels, 0);			// Load raw image data to img_data and populate width, height and nrChannels
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

void create_model(Model& model, std::vector<vertex> vertices, glm::mat4 model_matrix, const char* texture_path) {
	const int floats_per_vertex = 3;
	const int floats_per_normal = 3;
	const int floats_per_texcoord = 2;

	/**
	 * Generate VAO, VBO and configure attributes for VAO
	 */
	unsigned int VBO;
	int stride = floats_per_vertex + floats_per_normal + floats_per_texcoord;

	glGenVertexArrays(1, &model.VAO);					// Generate a VAO and set switch_VAO to the new VAO's ID number
	glGenBuffers(1, &VBO);								// Generate a VBO and set switch_VBO to the new VBO's ID number

	glBindVertexArray(model.VAO);						// Bind the VAO to the context, which saves the following function calls.

	glBindBuffer(GL_ARRAY_BUFFER, VBO);					// Bind the VBO to the context (and by extension the currently bound VAO).
	glBufferData(GL_ARRAY_BUFFER,
		vertices.size() * sizeof(vertex),
		&vertices[0],
		GL_STATIC_DRAW);								// Copy the data from vertices to the VBO.

	glVertexAttribPointer(0, floats_per_vertex,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)0);										// Tells the context (and by extension the VAO) how to read the first attribute of the vertex buffer.
	glEnableVertexAttribArray(0);						// Enable the above vertex attribute array.

	glVertexAttribPointer(1, floats_per_normal,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)(sizeof(float) * floats_per_vertex));	// Tells the context (and by extension the VAO) how to read the second attribute of the vertex buffer.
	glEnableVertexAttribArray(1);						// Enable the above vertex attribute array.

	glVertexAttribPointer(2, floats_per_texcoord,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)(sizeof(float) * (floats_per_vertex + floats_per_normal))
			);											// Tells the context (and by extension the VAO) how to read the second attribute of the vertex buffer.
	glEnableVertexAttribArray(2);						// Enable the above vertex attribute array.

	glBindVertexArray(0);								// Unbind the VAO from the context.

	/**
	 * Set orange models number of vertices
	 */
	model.number_of_vertices = vertices.size();

	/**
	 * Assign model matrix
	 */
	model.model = model_matrix;

	/**
	 * Set up texture
	 */
	unsigned int console_texture = load_wrap_texture(texture_path);				// Generate texture with wrapping attributes (ideally it will be a web and should not repeat)

	glob::universal_shader->use();												// Bind universal shader to context
	glob::universal_shader->setInt("texture", glob::number_of_textures++);		// Tell shader where to find texture

	model.texture_offset = glob::number_of_textures - 1;						// Assign offset from GL_TEXTURE0 texture unit
	model.texture = console_texture;											// Assing handle to texture
}

Model get_desk_model(const char* texture_path) {
	Model plane;

	/**
	 * Set up VAO and number of vertices
	 */
	const float plane_vertices[] = {
	-1.0f, 0.0f, 1.0f,	0.f, 0.f, 0.f,	0.0f, 0.0f,	// Front left
	-1.0f, 0.0f, -1.0f,	0.f, 0.f, 0.f,	0.0f, 1.0f,	// Back left, brown
	1.0f, 0.0f, -1.0f,	0.f, 0.f, 0.f,	1.0f, 1.0f,	// Back right, brown
	1.0f, 0.0f, -1.0f,	0.f, 0.f, 0.f,	1.0f, 1.0f,	// Back right, brown
	-1.0f, 0.0f, 1.0f,	0.f, 0.f, 0.f,	0.0f, 0.0f,	// Front left, brown
	1.0f, 0.0f, 1.0f,	0.f, 0.f, 0.f,	1.0f, 0.0f	// Front right, brown
	};

	const int floats_per_vertex = 3;
	const int floats_per_normal = 3;
	const int floats_per_texture = 2;
	const int stride = floats_per_vertex + floats_per_normal + floats_per_texture;

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

	glVertexAttribPointer(1, floats_per_normal,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)(sizeof(float) * floats_per_vertex));	// Tells the context (and by extension the VAO) how to read the second attribute of the vertex buffer.
	glEnableVertexAttribArray(1);						// Enable the above vertex attribute array.

	glVertexAttribPointer(2, floats_per_texture,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)(sizeof(float) * (floats_per_vertex + floats_per_normal))
			);											// Tells the context (and by extension the VAO) how to read the second attribute of the vertex buffer.
	glEnableVertexAttribArray(2);						// Enable the above vertex attribute array.

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
	 * Set up texture
	 */
	unsigned int plane_texture = load_wrap_texture(texture_path);									// Generate texture with wrapping attributes

	glob::universal_shader->use();																	// Bind universal shader to context
	glob::universal_shader->setInt("texture", glob::number_of_textures++);							// Tell shader where to find texture

	plane.texture_offset = glob::number_of_textures - 1;											// Assign offset from GL_TEXTURE0 texture unit
	plane.texture = plane_texture;																	// Assing handle to texture

	return plane;
}

Model get_switch_model(const char* texture_path) {
	Model console;
	const float texture_width = 6668.f;
	const float texture_height = 3046.f;
	const float matte_texture_width = 3064.f;

	const float front_face_offset = matte_texture_width / texture_width;
	const float front_face_height = 0.54f;
	const float side_face_length = 0.07;

	/**
	 * Set up VAO and number of vertices
	 */
	const float console_vertices[] = {
		// front face
		-0.5f, 0.5882f, 1.0f,	0.f, 0.f, 0.f,	front_face_offset, front_face_height,	// Front top left
		0.5f, 0.5882f, 1.0f,	0.f, 0.f, 0.f,	1.f, front_face_height, 				// Front top right
		0.5f, -0.5882f, 1.0f,	0.f, 0.f, 0.f,	1.f, 0.0f,								// Front bottom left
		0.5f, -0.5882f, 1.0f,	0.f, 0.f, 0.f,	1.f, 0.0f,								// Front bottom left
		-0.5f, 0.5882f, 1.0f,	0.f, 0.f, 0.f,	front_face_offset, front_face_height,	// Front top right
		-0.5f, -0.5882f, 1.0f,	0.f, 0.f, 0.f,	front_face_offset, 0.f,					// Front bottom right

		// right face
		0.5f, 0.5882f, 1.0f,	0.f, 0.f, 0.f,	side_face_length, 1.f,					// Front top right
		0.5f, 0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 1.f,								// Back top right
		0.5f, -0.5882f, 1.0f,	0.f, 0.f, 0.f,	0.0f, 0.0f,								// Front bottom left
		0.5f, -0.5882f, 1.0f,	0.f, 0.f, 0.f,	0.0f, 0.0f,								// Front bottom left
		0.5f, 0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 1.f,								// Back top right
		0.5f, -0.5882f, 0.93f,	0.f, 0.f, 0.f,	side_face_length, 0.0f,					// Back bottom left

		// back face
		-0.5f, 0.5882f, 0.93f,	0.f, 0.f, 0.f,	front_face_offset, 1.f, 				// Back top left
		0.5f, 0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.f, 1.0f, 								// Back top right
		-0.5f, -0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 0.0f,								// Back bottom right
		-0.5f, -0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 0.0f,								// Back bottom right
		0.5f, 0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 1.0f,								// Back top right
		0.5f, -0.5882f, 0.93f,	0.f, 0.f, 0.f,	front_face_offset, 0.0f,				// Back bottom left

		// left face
		-0.5f, 0.5882f, 1.0f,	0.f, 0.f, 0.f,	side_face_length, 1.f,					// Front top left
		-0.5f, 0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 1.0f,								// Back top left
		-0.5f, -0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 0.0f,								// Back bottom right
		-0.5f, -0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 0.0f,								// Back bottom right
		-0.5f, 0.5882f, 1.0f,	0.f, 0.f, 0.f,	0.0f, 1.0f,							 	// Front top left
		-0.5f, -0.5882f, 1.0f,	0.f, 0.f, 0.f,	side_face_length, 0.0f,					// Front bottom right

		// bottom face
		-0.5f, -0.5882f, 1.0f,	0.f, 0.f, 0.f,	side_face_length, 1.f,					// Front bottom right
		0.5f, -0.5882f, 1.0f,	0.f, 0.f, 0.f,	0.0f, 1.0f,								// Front bottom left
		-0.5f, -0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 0.0f,								// Back bottom right
		-0.5f, -0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 0.0f,								// Back bottom right
		0.5f, -0.5882f, 1.0f,	0.f, 0.f, 0.f,	0.0f, 1.0f,								// Front bottom left
		0.5f, -0.5882f, 0.93f,	0.f, 0.f, 0.f,	side_face_length, 0.0f,				// Back bottom left

		// top face
		-0.5f, 0.5882f, 1.0f,	0.f, 0.f, 0.f,	side_face_length, 1.f,					// Front top left
		-0.5f, 0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 1.0f,								// Back top left
		0.5f, 0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 0.0f,								// Back top right
		0.5f, 0.5882f, 0.93f,	0.f, 0.f, 0.f,	0.0f, 0.0f,								// Back top right
		-0.5f, 0.5882f, 1.0f,	0.f, 0.f, 0.f,	0.0f, 1.0f,								// Front top left
		0.5f, 0.5882f, 1.0f,	0.f, 0.f, 0.f,	side_face_length, 0.0f,				// Front top right

		// stand
		-0.5f + (14.0f / 17.0f), -0.5882f + (6.0f / 10.0f), 0.93f,	0.f, 0.f, 0.f,	front_face_offset, 1.f,		// Stand top left
		-0.5f + (16.0f / 17.0f), -0.5882f + (6.0f / 10.0f), 0.93f,	0.f, 0.f, 0.f,	0.0f, 1.0f,					// Stand top right
		-0.5f + (14.0f / 17.0f), -0.5f, 0.70f,						0.f, 0.f, 0.f,	0.0f, 0.0f,					// Stand bottom left
		-0.5f + (14.0f / 17.0f), -0.5f, 0.70f,						0.f, 0.f, 0.f,	0.0f, 0.0f,					// Stand bottom left
		-0.5f + (16.0f / 17.0f), -0.5882f + (6.0f / 10.0f), 0.93f,	0.f, 0.f, 0.f,	0.0f, 1.0f,					// Stand top right
		-0.5f + (16.0f / 17.0f), -0.5f, 0.70f,						0.f, 0.f, 0.f,	front_face_offset, 0.0f		// Stand bottom right
	};

	const int floats_per_vertex = 3;
	const int floats_per_normal = 3;
	const int floats_per_texture = 2;
	const int stride = floats_per_vertex + floats_per_normal + floats_per_texture;

	unsigned int switch_VAO, switch_VBO;
	glGenVertexArrays(1, &switch_VAO);					// Generate a VAO and set switch_VAO to the new VAO's ID number
	glGenBuffers(1, &switch_VBO);						// Generate a VBO and set switch_VBO to the new VBO's ID number

	glBindVertexArray(switch_VAO);						// Bind the VAO to the context, which saves the following function calls.

	glBindBuffer(GL_ARRAY_BUFFER, switch_VBO);			// Bind the VBO to the context (and by extension the currently bound VAO).
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(console_vertices),
		console_vertices,
		GL_STATIC_DRAW);								// Copy the data from vertices to the VBO.

	glVertexAttribPointer(0, floats_per_vertex,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float), (void*)0);				// Tells the context (and by extension the VAO) how to read the first attribute of the vertex buffer.
	glEnableVertexAttribArray(0);						// Enable the above vertex attribute array.

	glVertexAttribPointer(1, floats_per_normal,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)(sizeof(float) * floats_per_vertex));	// Tells the context (and by extension the VAO) how to read the second attribute of the vertex buffer.
	glEnableVertexAttribArray(1);						// Enable the above vertex attribute array.

	glVertexAttribPointer(2, floats_per_texture,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)(sizeof(float) * (floats_per_vertex + floats_per_normal))
	);											// Tells the context (and by extension the VAO) how to read the second attribute of the vertex buffer.
	glEnableVertexAttribArray(2);						// Enable the above vertex attribute array.

	glBindVertexArray(0);								// Unbind the VAO from the context.

	console.VAO = switch_VAO;							// Assign VAO handle to model
	console.number_of_vertices = sizeof(console_vertices)
		/ (sizeof(float) * stride);						// Assign number_of_vertices to model

	/**
	 * Define switch model matrix.
	 */
	glm::mat4 switch_model = glm::mat4(1.0f);														// Initially set as identity matrix.
	switch_model = glm::translate(switch_model, glm::vec3(0.0f, 0.0f, -0.15f));						// Center model
	switch_model = glm::rotate(switch_model, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// Rotate model 33 deg about X axis.s
	switch_model = glm::scale(switch_model, glm::vec3(0.5f, 0.25f, 0.5f));							// Scale model to half size.

	console.model = switch_model;												// Assign model to Model

	/**
	 * Set up texture
	 */
	unsigned int console_texture = load_wrap_texture(texture_path);				// Generate texture with wrapping attributes (ideally it will be a web and should not repeat)

	glob::universal_shader->use();												// Bind universal shader to context
	glob::universal_shader->setInt("texture", glob::number_of_textures++);		// Tell shader where to find texture

	console.texture_offset = glob::number_of_textures - 1;						// Assign offset from GL_TEXTURE0 texture unit
	console.texture = console_texture;											// Assing handle to texture

	return console;
}

Model get_orange_model(const char* texture_path) {
	using namespace std;

	Model orange;

	/**
	 * Generate vertices for sphere
	 * From:
	 *	http://www.songho.ca/opengl/gl_sphere.html
	 */
	vector<vertex> vertices;

	// change these to change attributes of sphere
	float radius = 1.f;
	int sector_count = 36;
	int stack_count = 36;

	{
		struct vertex to_push;

		float xz;
		float lengthInv = 1.f / radius;

		float sector_step = 2 * PI / sector_count;
		float stack_step = PI / stack_count;
		float sector_angle,							// theta
			stack_angle;							// phi

		/**
		 * Iterate through stacks
		 */
		for (int i = 0; i <= stack_count; ++i) {
			stack_angle = PI / 2.f - i * stack_step;	// starts at PI/2, ends at -PI/2
			xz = radius * cosf(stack_angle);			// r * cos(phi)
			to_push.y = radius * sinf(stack_angle);		// r * sin(phi)

			/**
			 * Iterate through sectors of current stack
			 */
			for (int j = 0; j <= sector_count; ++j) {
				sector_angle = j * sector_step;			// starts at 0, ends at 2*PI

				// calculate vertex position
				to_push.x = xz * cosf(sector_angle);	// r * cos(phi) * cosf(theta)
				to_push.z = xz * sinf(sector_angle);	// r * cos(phi) * sinf(theta)

				// calculate normal: the vector orthonormal to the vertex (for lighting and physics)
				to_push.nx = to_push.x * lengthInv;
				to_push.ny = to_push.y * lengthInv;
				to_push.nz = to_push.z * lengthInv;

				// calculate texture coordinates
				to_push.s = (float)j / sector_count;
				to_push.t = 1.f - (float)i / stack_count;

				vertices.push_back(to_push);
			}
		}
	}

	/**
	 * Populate vertex buffer with vertex data in order to be drawn by glDrawArray
	 * Adapted from:
	 *	http://www.songho.ca/opengl/gl_sphere.html
	 *
	 * k1---k1+1
	 * |   / |
	 * |  /	 |
	 * k2---k2+1
	 */
	vector<vertex> VB;

	{
		int k1, k2;

		/**
		 * Iterate through stacks
		 */
		for (int i = 0; i < stack_count; ++i) {
			k1 = i * (sector_count + 1);	// beginning of current stack
			k2 = k1 + sector_count + 1;		// beginning of next stack

			/**
			 * Iterate through sectors of current stack
			 */
			for (int j = 0; j < sector_count; ++j, ++k1, ++k2) {
				if (i != 0) {
					VB.push_back(vertices.at(k1));
					VB.push_back(vertices.at(k2));
					VB.push_back(vertices.at(k1 + 1));
				}

				if (i != (stack_count - 1)) {
					VB.push_back(vertices.at(k1 + 1));
					VB.push_back(vertices.at(k2));
					VB.push_back(vertices.at(k2 + 1));
				}
			}
		}
	}

	/**
	 * Define orange model matrix
	 */
	glm::mat4 model = glm::mat4(1.0f);												// Initially set as identity matrix
	model = glm::translate(model, glm::vec3(0.25f, 0.f, 0.5f));
	model = glm::scale(model, glm::vec3(0.06f, 0.06f, 0.06f));

	create_model(orange, VB, model, texture_path);

	return orange;
}

Model get_napkin_model(const char* texture_path) {
	Model napkin;

	vertex napkin_vertices[6] = {
		-1.0f, 0.0f, 1.0f, 0.f, 0.f, 0.f, 0.0f, 0.0f,	// Front left
		-1.0f, 0.0f, -1.0f, 0.f, 0.f, 0.f, 0.0f, 1.0f,	// Back left, brown
		1.0f, 0.0f, -1.0f, 0.f, 0.f, 0.f, 1.0f, 1.0f,	// Back right, brown
		1.0f, 0.0f, -1.0f, 0.f, 0.f, 0.f, 1.0f, 1.0f,	// Back right, brown
		-1.0f, 0.0f, 1.0f, 0.f, 0.f, 0.f, 0.0f, 0.0f,	// Front left, brown
		1.0f, 0.0f, 1.0f, 0.f, 0.f, 0.f, 1.0f, 0.0f	// Front right, brown
	};

	std::vector<vertex> vertices(napkin_vertices, napkin_vertices + 6);	// fill vertices with above vector

	/**
	 * Define napkin model matrix
	 */
	glm::mat4 model = glm::mat4(1.f);
	model = glm::translate(model, glm::vec3(0.25f, -0.064, 0.5f));
	model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));


	create_model(napkin, vertices, model, texture_path);

	return napkin;
}

Model get_soda_model(const char* texture_path) {
	using namespace std;

	Model soda;

	/**
	 * Generate vertices for soda can
	 * Adapted from:
	 *	http://www.songho.ca/opengl/gl_sphere.html
	 */

	vector<vertex> vertices;
	vertex lid_middle;
	vertex bottom_middle;

	// change these to change attributes of can
	float radius = 1.f;
	float bevel_width = 0.2;
	float height = 4.f;
	int stacks_per_bevel = 3;
	int sector_count = 12;
	int stack_count = 36;

	// assing positions to center points for lid and bottom
	lid_middle.x = 0.f;
	lid_middle.y = height;
	lid_middle.z = 0.f;
	bottom_middle.x = 0.f;
	bottom_middle.y = 0.f;
	bottom_middle.z = 0.f;

	{
		struct vertex to_push;

		float lengthInv = 1.f / radius;				// warning: definitely wrong, lengthInv should change per stack case

		float sector_step = 2 * PI / sector_count;
		float sector_angle;							// theta

		/**
		 * Iterate through sectors
		 */
		for (int i = 0; i <= stack_count; ++i) {
			to_push.y = height - height * ((float) i / stack_count);

			/**
			 * Iterate through sectors of current stack
			 */
			for (int j = 0; j <= sector_count; ++j) {
				sector_angle = j * sector_step;			// starts at 0, ends at 2*PI

				// calculate vertex position
				if (i <= stacks_per_bevel) {
					float bevel_radius = radius + (bevel_width * ((float) i / stacks_per_bevel));
					to_push.x = bevel_radius * cosf(sector_angle);
					to_push.z = bevel_radius * sinf(sector_angle);
				}										// case: upper bevel
				else if (i < stack_count - stacks_per_bevel) {
					to_push.x = (radius + bevel_width) * cosf(sector_angle);
					to_push.z = (radius + bevel_width) * sinf(sector_angle);
				}										// case: body
				else {
					float bevel_radius = radius + (bevel_width * ((float) (stack_count-i)  / (stacks_per_bevel)));
					to_push.x = bevel_radius * cosf(sector_angle);
					to_push.z = bevel_radius * sinf(sector_angle);
				}										// case: lower bevel

				// calculate normal: the vector orthonormal to the vertex (for lighting and physics)
				to_push.nx = to_push.x * lengthInv;
				to_push.ny = to_push.y * lengthInv;
				to_push.nz = to_push.z * lengthInv;

				// calculate texture coordinates
				to_push.s = 1.f - (float)j / sector_count;
				to_push.t = 1.f - (float)i / stack_count;

				
				vertices.push_back(to_push);
			}
		}

	}

	/**
	 * Populate vertex buffer with vertex data in order to be drawn by glDrawArray
	 * Adapted from:
	 *	http://www.songho.ca/opengl/gl_sphere.html
	 *
	 * k1---k1+1
	 * |   / |
	 * |  /	 |
	 * k2---k2+1
	 */
	vector<vertex> VB;

	{
		int k1, k2;

		/**
		 * Iterate through stacks
		 */
		for (int i = 0; i < stack_count; ++i) {
			k1 = i * (sector_count + 1);	// beginning of current stack
			k2 = k1 + sector_count + 1;		// beginning of next stack

			/**
			 * Iterate through sectors of current stack
			 */
			for (int j = 0; j < sector_count; ++j, ++k1, ++k2) {
				if (i == 0) {
					// set tex_coords
					lid_middle.s = (float)j / sector_count;
					lid_middle.t = 1.f;

					// set normals
					//TODO

					VB.push_back(lid_middle);
					VB.push_back(vertices.at(k1));
					VB.push_back(vertices.at(k1 + 1));
				}	// top lid

				if (i == stack_count - 1) {
					// set tex_coords
					bottom_middle.s = (float)j / sector_count;
					bottom_middle.t = 0.f;

					// set normals
					//TODO

					VB.push_back(bottom_middle);
					VB.push_back(vertices.at(k2));
					VB.push_back(vertices.at(k2 + 1));
				} // bottom lid

				// triangle 1
				VB.push_back(vertices.at(k1));
				VB.push_back(vertices.at(k2));
				VB.push_back(vertices.at(k1 + 1));

				// triangle 2
				VB.push_back(vertices.at(k1 + 1));
				VB.push_back(vertices.at(k2));
				VB.push_back(vertices.at(k2 + 1));

			}
		}
	}

	/**
	 * Define orange model matrix
	 */
	glm::mat4 model = glm::mat4(1.0f);												// Initially set as identity matrix
	model = glm::translate(model, glm::vec3(-.25f, -0.060f, 0.5f));
	model = glm::scale(model, glm::vec3(0.04f, 0.04f, 0.04f));
	model = glm::rotate(model, glm::radians(120.f), glm::vec3(0.f, 1.f, 0.f));


 	create_model(soda, VB, model, texture_path);

	return soda;
}

void draw_model(Model model, glm::mat4 projection, glm::mat4 view) {
	using namespace glob;

	glActiveTexture(GL_TEXTURE0 + model.texture_offset);
	glBindTexture(GL_TEXTURE_2D, model.texture);

	universal_shader->use();
	universal_shader->setInt("texture", model.texture_offset);

	glBindVertexArray(model.VAO);
	universal_shader->setMat4("projection", projection);
	universal_shader->setMat4("view", view);
	universal_shader->setMat4("model", model.model);

	glDrawArrays(GL_TRIANGLES, 0, model.number_of_vertices);
}