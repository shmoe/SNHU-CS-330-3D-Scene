/**
 * Griffin Nelson
 * 5/6/2021
 *
 * "main.cpp" - Contains the "main" function, the initial entry point for code execution,
 *		which uses GLFW to initialize a OpenGL context and then the standard OpenGL functions
 *		mapped by GLAD to do operations on said context.
 */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

/**
* Forward declares all functions in "main.cpp" for unit testing
* purposes.
*/
#include "main.h"

/**
* Contains callbacks for GLFW events.
*/
#include "events.h"

/**
 * Contains the "mesh" class from OpenGLSample
 */
#include "mesh.h"

/**
* All global variables (primarily for the camera)
*/
namespace glob {
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	float cameraSpeed = 2.5f;

	bool firstMouse = true;
	float yaw = -90.0f;
	float pitch = 0.0f;
	float lastX = 800.0f / 2.0;
	float lastY = 600.0 / 2.0;
	float fov = 45.0f;

	//timing
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

	bool orthographic = false;
	bool wireframe = false;
	bool zoom = false;
}

/**
 * Declare function prototypes for GLFW callbacks that rely on global variables
 */

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

/**
 * main(int argc, char* argv[]) - Initializes the necessary libraries for ppening an OpenGL context and houses the
 *	main render loop for the created GLFW window.
 *
 *		argc - the number of arguements used to launch this program, including the binary name
 *
 *		argv - an ordered list of the arguements used to launch this program, starting with the binary name
 */
int main(int argc, char* argv[]) {
	GLFWwindow* window;	// Main render window

	/**
	* Initialize GLFW and create the main render window. Safely end execution
	* on failure.
	*/
	if ((window = create_glfw_window()) == nullptr) {
		return -1;			// Safely end execution with a bad value
	}

	/**
	 * Initialize GLAD, which manages function pointers for OpenGL and allows
	 * us to ignore ignore implementation specific details and just use the
	 * OpenGL API to utilize the OpenGL conext. If initilization fails, log to
	 * stderr and terminate.
	 */
	if (init_glad()) {
		glfwTerminate();	// Safely terminate GLFW
		return -1;			// Safely end execution with a bad value
	}

	/**
	 * Tell the context the portion of the rendering window to draw to (in this
	 * case the full window.)
	 *
	 * Also set clear color
	 */
	glViewport(0, 0, GLFW_WINDOW_WIDTH, GLFW_WINDOW_HEIGHT);	// Set viewport dimensions to top-left (0,0) through bottom-right (800,600) corners
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);						// Set clear color to black

	/**
	 * Enable depth testing and set how OpenGL responds to depth.
	 */
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	/**
	 * Set callback functions.
	 */
	glfwSetFramebufferSizeCallback(window, events::framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/**
	 * Tell GLFW to capture mouse.
	 */
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	/**
	 * Generate a shader program using the following vertext and fragment shaders.
	 */
	const char* vertex_shader = "#version 330 core\n"											// Set OpenGL version (3.3) and profile (core).
		"layout (location = 0) in vec3 aPos;\n"													// Define the input parameter (the current vertex coordinate) and its index.
		"layout (location = 1) in vec3 VBOColor;\n"
		"out vec3 vertexColor;\n"																// Define the output parameter (taken by the fragment shader to color the fragment).
		"uniform mat4 model;\n"																	// Model matrix (uniform input)
		"uniform mat4 view;\n"																	// View matrix (uniform input)
		"uniform mat4 projection;\n"															// Projection matrix (uniform input)
		"void main()\n"
		"{\n"
		"   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"		// Map the input vec3 to a vec4 and set it to gl_Position. 
		"	vertexColor = VBOColor;\n"
		"}\0";

	const char* fragment_shader = "#version 330 core\n"								// Set OpenGL version (3.3) and profile (core).
		"in vec3 vertexColor;\n"													// Define the input (position of the point being drawn)
		"out vec4 FragColor;\n"														// Define the output (the color for the current pixel).
		"void main()\n"
		"{\n"
		"   FragColor = vec4(vertexColor.x, vertexColor.y, vertexColor.z, 1.0f);\n" // Map the input vec3 to vec4 and set to FragColor
		"}\n\0";

	unsigned int shader_program = gen_shader_program(vertex_shader, fragment_shader);

	const int floats_per_vertex = 3;
	const int floats_per_color = 3;
	const int stride = floats_per_vertex + floats_per_vertex;

	/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~																		Define Switch model																			~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **/
	/**
	 * Generate a Vertex Array Object using the following vertices and indices.
	 */
	const float switch_vertices[] = {
		-0.5f, 0.5882f, 1.0f,		1.0f, 0.0f, 0.0f,	// Front top left
		0.5f, 0.5882f, 1.0f,		0.0f, 1.0f, 0.0f,	// Front top right
		-0.5f, -0.5882f, 1.0f,		0.0f, 0.0f, 1.0f,	// Front bottom right
		0.5f, -0.5882f, 1.0f,		1.0f, 0.0f, 0.0f,	// Front bottom left

		-0.5f, 0.5882f, 0.93f,		1.0f, 1.0f, 0.0f,	// Back top left
		0.5f, 0.5882f, 0.93f,		1.0f, 0.0f, 1.0f,	// Back top right
		-0.5f, -0.5882f, 0.93f,		0.0f, 1.0f, 1.0f,	// Back bottom right
		0.5f, -0.5882f, 0.93f,		1.0f, 1.0f, 0.0f,	// Back bottom left

		-0.5f + (14.0f / 17.0f), -0.5882f + (6.0f / 10.0f), 0.93f,	0.0f, 1.0f, 1.0f,		// Stand top left
		-0.5f + (16.0f / 17.0f), -0.5882f + (6.0f / 10.0f), 0.93f,	0.0f, 1.0f, 1.0f,		// Stand top right
		-0.5f + (14.0f / 17.0f), -0.5f, 0.70f,						0.0f, 1.0f, 1.0f,		// Stand bottom left
		-0.5f + (16.0f / 17.0f), -0.5f, 0.70f,						0.0f, 1.0f, 1.0f		// Stand bottom right
	};								// Vertices that make up our Switch body. Bound to the VBO

	const unsigned int switch_indices[] = {
		0, 1, 2, 2, 1, 3,		// front face
		1, 5, 3, 3, 5, 7,		// right face
		4, 5, 6, 6, 5, 7,		// back face
		0, 4, 6, 6, 0, 2,		// left face
		2, 3, 6, 6, 3, 7,		// bottom face
		0, 4, 5, 5, 0, 1,		// top face

		8, 9, 10, 10, 9, 11		// stand
	};								// Order by which to draw our vertices. Bound to the EBO

	unsigned int switch_VAO, switch_VBO, switch_EBO;
	glGenVertexArrays(1, &switch_VAO);					// Generate a VAO and set switch_VAO to the new VAO's ID number
	glGenBuffers(1, &switch_VBO);						// Generate a VBO and set switch_VBO to the new VBO's ID number
	glGenBuffers(1, &switch_EBO);						// Generate a EBO and set switch_EBO to the new EBO's ID number

	glBindVertexArray(switch_VAO);						// Bind the VAO to the context, which saves the following function calls.

	glBindBuffer(GL_ARRAY_BUFFER, switch_VBO);			// Bind the VBO to the context (and by extension the currently bound VAO).
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(switch_vertices),
		switch_vertices,
		GL_STATIC_DRAW);								// Copy the data from vertices to the VBO.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, switch_EBO);	// Bind the EBO to the context (and by extension the currently bound VAO).
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(switch_indices),
		switch_indices,
		GL_STATIC_DRAW);								// Copy the data from indices to the EBO.

	glVertexAttribPointer(0, floats_per_vertex,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float), (void*)0);				// Tells the context (and by extension the VAO) how to read the first attribute of the vertex buffer.
	glEnableVertexAttribArray(0);						// Enable the above vertex attribute array.

	glVertexAttribPointer(1, floats_per_color,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)(sizeof(float) * 3));					// Tells the context (and by extension the VAO) how to read the second attribute of the vertex buffer.
	glEnableVertexAttribArray(1);						// Enable the above vertex attribute array.

	glBindVertexArray(0);								// Unbind the VAO from the context.

	/**
	 * Define switch model matrix.
	 */
	glm::mat4 switch_model = glm::mat4(1.0f);														// Initially set as identity matrix.
	switch_model = glm::translate(switch_model, glm::vec3(0.0f, 0.0f, -0.15f));						// Center model
	switch_model = glm::rotate(switch_model, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// Rotate model 33 deg about X axis.s
	switch_model = glm::scale(switch_model, glm::vec3(0.5f, 0.25f, 0.5f));							// Scale model to half size.

	/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~																		Define Plane mesh																			~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ **/
	const float plane_vertices[] = {
		-1.0f, 0.0f, -1.0f,		118.f / 255.f, 80.f / 255.f, 60.f / 255.f,	// Back left, brown
		-1.0f, 0.0f, 1.0f,		118.f / 255.f, 80.f / 255.f, 60.f / 255.f,	// Front left, brown
		1.0f, 0.0f, -1.0f,		118.f / 255.f, 80.f / 255.f, 60.f / 255.f,	// Back right, brown
		1.0f, 0.0f, 1.0f,		118.f / 255.f, 80.f / 255.f, 60.f / 255.f	// Front right, brown
	};

	const unsigned int plane_indices[] = {
		0, 1, 2, 2, 1, 3							// Plane
	};

	unsigned int plane_VAO, plane_VBO, plane_EBO;

	glGenVertexArrays(1, &plane_VAO);					// Generate a VAO and set plane_VAO to the new VAO's ID number
	glGenBuffers(1, &plane_VBO);						// Generate a VBO and set plane_VBO to the new VBO's ID number
	glGenBuffers(1, &plane_EBO);						// Generate a EBO and set plane_EBO to the new EBO's ID number

	glBindVertexArray(plane_VAO);						// Bind the VAO to the context, which saves the following function calls.

	glBindBuffer(GL_ARRAY_BUFFER, plane_VBO);			// Bind the VBO to the context (and by extension the currently bound VAO).
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(plane_vertices),
		plane_vertices,
		GL_STATIC_DRAW);								// Copy the data from vertices to the VBO.

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, plane_EBO);	// Bind the EBO to the context (and by extension the currently bound VAO).
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(plane_indices),
		plane_indices,
		GL_STATIC_DRAW);								// Copy the data from indices to the EBO.

	glVertexAttribPointer(0, floats_per_vertex,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float), (void*)0);				// Tells the context (and by extension the VAO) how to read the first attribute of the vertex buffer.
	glEnableVertexAttribArray(0);						// Enable the above vertex attribute array.

	glVertexAttribPointer(1, floats_per_color,
		GL_FLOAT, GL_FALSE,
		stride * sizeof(float),
		(void*)(sizeof(float) * 3));					// Tells the context (and by extension the VAO) how to read the second attribute of the vertex buffer.
	glEnableVertexAttribArray(1);						// Enable the above vertex attribute array.

	glBindVertexArray(0);								// Unbind the VAO from the context.

	/**
	 * Define plane model matrix.
	 */
	glm::mat4 plane_model = glm::mat4(1.0f);														// Initially set as identity matrix.
	plane_model = glm::translate(plane_model, glm::vec3(0.0f, -0.065f, 0.0f));						// Center model
	//plane_model = glm::rotate(plane_model, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));	// Rotate model
	plane_model = glm::scale(plane_model, glm::vec3(2.0f, 1.0f, 1.0f));								// Scale model

	/**
	 * Main rendering loop
	 */
	while (!glfwWindowShouldClose(window)) {
		float curr_time = glfwGetTime();
		glob::deltaTime = curr_time - glob::lastFrame;
		glob::lastFrame = curr_time;

		/**
		 * Deal with keypresses
		 */
		processInput(window);

		/**
		 * Clear color and depth buffers before rendering.
		 */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/**
		 * Start using this shader and VAO for subsequent draws.
		 */
		glUseProgram(shader_program);			// Set shader for following draws
		glBindVertexArray(switch_VAO);			// Set vertex array object for following draws


		/**
		 * Create projection, view and model matrices to pass to shader.
		 */

		struct viewport_t {
			int x;
			int y;
			int width;
			int height;
		} viewport;																																// Struct instead of int [4] for field names
		glGetIntegerv(GL_VIEWPORT, (int*)&viewport);																							// Cast viewport_t to int [4] for the sake of glGet

		float aspect_ratio = (float)viewport.width / (float)viewport.height;																	// Calculate aspect ratio using viewport width and height

		glm::mat4 projection;
		if (glob::orthographic) {																												// Create projection matrix depending on value of glob::orthographic
			float ratio_size_per_depth = atan(glm::radians(glob::fov) / 2.f) * 2.f;																	// Multiply this variable by distance Z from the camera to get aspect ratio of ortho projection
			float distance = glm::length(glob::cameraFront - glob::cameraPos);																		// Calculate distance Z between camera and focal point
			float size_y = ratio_size_per_depth * distance;																							// Calculate height of projection
			float size_x = ratio_size_per_depth * distance * aspect_ratio;																			// Calculate width of projection

			projection = glm::ortho(-size_x, size_x, -size_y, size_y, 0.1f, 2.f * distance);														// Create orthographic projection matrix;
		}
		else {
			projection = glm::perspective(glm::radians(glob::fov), aspect_ratio, 0.1f, 100.f);														// Create perspective projection matrix
		}

		glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, &projection[0][0]);									// Pass projection matrix to shader

		glm::mat4 view = glm::lookAt(glob::cameraPos, glob::cameraPos + glob::cameraFront, glob::cameraUp);										// Create view matrix
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, &view[0][0]);												// Pass view matrix to shader

		/* Transforms to model matrix go here */
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, &switch_model[0][0]);									// Pass model matrix to shader

		/**
		 * Set polygon mode depending on value of wireframe
		 */
		if (glob::wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		/**
		 * Draw the VBO in order of the indices held by the EBO.
		 */
		glDrawElements(GL_TRIANGLES,							// Draw as triangles
			sizeof(switch_indices) / sizeof(unsigned int),		// Draw all indices in EBO
			GL_UNSIGNED_INT,									// Type of indices held by EBO
			0);													// Start reading EBO at the beginning.

		/**
		 * Start using this shader and VAO for subsequent draws.
		 */
		//glUseProgram(shader_program);		// Set shader for following draws
		glBindVertexArray(plane_VAO);		// Set vertex array object for following draws

		/**
		 * Pass projection, view and model matrices to pass to shader.
		 */
		//glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, &projection[0][0]);							// Pass the same projection matrix to shader

		//glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, &view[0][0]);										// Pass the same view matrix to shader

		/* Transforms to model matrix go here */
		glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, &plane_model[0][0]);									// Pass model matrix to shader


		/**
		 * Draw the VBO in order of the indices held by the EBO.
		 */
		glDrawElements(GL_TRIANGLES,							// Draw as triangles
			sizeof(plane_indices) / sizeof(unsigned int),		// Draw all indices in EBO
			GL_UNSIGNED_INT,									// Type of indices held by EBO
			0);


		glfwSwapBuffers(window);		// Swaps front and back framebuffers (output to screen)

		glfwPollEvents();				// Check to see if any events were triggered and call
											// the corresponding callback functions
	}

	/**
	 * End execution
	 */
	glfwTerminate();	// Safely terminate GLFW
	return 0;			// End execution with a good value
}

/**
 * Initialize GLFW and create render window with the newly created GL context.
 * Return nullptr on failure.
 */
GLFWwindow* create_glfw_window() {
	glfwInit();	// Initialize the GLFW library

	/**
	 * Tell GLFW to open a window with OpenGL 3.3 in the Core Profile mode
	 */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, LOCAL_GL_VERSION[0]);	// Tell GLFW the major value of the OpenGL context we wish the new window to have
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, LOCAL_GL_VERSION[1]);	// Tell GLFW the minor value of the OpenGL context we wish the new window to hav
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);		// Tell GLFW to open the new window in Core Profile mode

	/**
	 * Tell GLFW to create a window with the above configuration. If window
	 * creation fails, log to stderr and terminate.
	 */
	GLFWwindow* window = glfwCreateWindow(GLFW_WINDOW_WIDTH, GLFW_WINDOW_HEIGHT, "Hellow Window", NULL, NULL);	// Create GLFW window
	if (window == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;		// Log failure to stderr
		glfwTerminate();												// Safely terminate GLFW
		return nullptr;														// End execution with a bad value
	}

	/**
	 * Bind new window to the OpenGL 3.3 context (essential to initialize GLAD!)
	 */
	glfwMakeContextCurrent(window);

	return window;
}

/**
 * Initialize GLAD, which manages function pointers for OpenGL and allows
 * us to ignore ignore implementation specific details and just use the
 * OpenGL API to utilize the OpenGL conext. If initilization fails, log to
 * stderr and return -1.
 */
int init_glad() {
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {	// Bind function glfwGetProcAddress as the GLAD LoadGL Loader
		std::cerr << "Failed to initialize GLAD" << std::endl;	// Log failure to stderr
		return -1;												// Return with a bad value
	}

	return 0;	// Return with a good value
}

/**
 * Create a shader program using the given vertex and shader GLSL sources.
 */
unsigned int gen_shader_program(const char* vertex_shader_source, const char* fragment_shader_source) {
	int success;
	char info_log[512];

	/**
	 * Create vertex shader
	 */
	unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);	// Create a vertex shader
	glShaderSource(vertex_shader, 1, &vertex_shader_source, NULL);	// Set the shader source
	glCompileShader(vertex_shader);									// Compile the shader

	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);		// Check whether the shader compiled successfully
	if (!success) {
		glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);									// Get the info log for a failed compilation.
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << info_log << std::endl;	// Log to stderr.
	}


	/**
	 * Create fragment shader
	 */
	unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);	// Create a fragment shader
	glShaderSource(fragment_shader, 1, &fragment_shader_source, NULL);	// Set the shader source
	glCompileShader(fragment_shader);									// Compile the shader

	glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);		// Check whether the shader compiled successfully
	if (!success) {
		glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);								// Get the info log for a failed compilation.
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << info_log << std::endl;	// Log to stderr.
	}

	/**
	 * Combine shaders into one shader program and return the said program's ID.
	 */
	unsigned int shader_program = glCreateProgram();				// Create the shader program.
	glAttachShader(shader_program, vertex_shader);					// Attach the vertex shader.
	glAttachShader(shader_program, fragment_shader);				// Attach the fragment shader.
	glLinkProgram(shader_program);									// Link the shaders into one program.

	glGetProgramiv(shader_program, GL_LINK_STATUS, &success);		// Check for linking errors.
	if (!success) {
		glGetProgramInfoLog(shader_program, 512, NULL, info_log);							// Get the info log for a failed compilation.
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;	// Log to stderr.
	}
	glDeleteShader(vertex_shader);									// Delete the vertex shader now that it lives on in the shader program.
	glDeleteShader(fragment_shader);								// Delete the fragment shader now that it lives on in the shader program.

	return shader_program;
}



/**
 * Credit goes to OpenGLSample
 */
 // process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
 /// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	static bool p_pressed = false;
	static bool o_pressed = false;

	using namespace glob;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);									// When "ESC" is pressed, set the flag that tells main()'s render loop to exit
	
	if (!p_pressed && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		glob::orthographic ^= true;						// Toggle value of orthographic
		p_pressed = true;								// Set p_pressed to true
	}																			// When "P" is pressed toggle between perspective and orthographic projection
	if (p_pressed && glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
		p_pressed = false;								// Set p_pressed to false

	if (!o_pressed && glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		glob::wireframe ^= true;						// Toggle value of wireframe
		o_pressed = true;								// Set o_pressed to true
	}																			// When "O" is pressed toggle wireframe mode On or Off
	if (o_pressed && glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE)
		o_pressed = false;								// Set o_pressed to false


	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
		zoom = true;															// When "Shift" is held, scrolling behavior becomes zooming in and out
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE &&
		glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE)
		zoom = false;															// When "Shift" is released, scrolling behavior returns to speeding up and down


	float speed = cameraSpeed * deltaTime;										// Adjust speed based on how many subframes have been skipped (deltaTime)
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += speed * cameraFront;										// When "W" is pressed, move camera forwards, towards cameraFront
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= speed * cameraFront;										// When "S" is pressed, move camera backwards, away from cameraFront
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;	// When "A" is pressed, move camera left, perpindicular to cameraFront
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * speed;	// When "D" is pressed, move camera right, perpindicular to cameraFront
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraUp * speed;											// When "Q" is pressed, move camera up, towards to cameraUp
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraUp * speed;											// When "E" is pressed, move camera up, away from cameraUp
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	using namespace glob;
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}								// Edge case for first mouse event, where there are no previous X and Y coordinates

	/**
	 * Determine change in X (xoffset) and change in Y (yoffset) of the mouse's coordinates
	 * and use those changes to calculate the new pitch and yaw of the camera.
	 */

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;	// reversed since y-coordinates go from bottom to top
	lastX = xpos;
	lastY = ypos;

	float sensitivity = 0.1f;		// change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	/**
	 * Generate the new cameraFront from the new pitch and yaw.
	 *
	 * In other words, change where the camera is facing.
	 */
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	using namespace glob;
	if (zoom) {
		fov -= (float)yoffset;				// Zoom in on scroll up, out on scroll down
		if (fov < 1.0f)
			fov = 1.0f;						// Set FOV minimum
		if (fov > 45.0f)
			fov = 45.0f;					// Set FOV maximum
	}
	else {
		 cameraSpeed -= (float)yoffset;		// Speed up on scroll up, down on scroll down
		 if (cameraSpeed < 1.0f)
			 cameraSpeed = 1.0f;			// Set speed minimum
		 if (cameraSpeed > 5.0f)
			 cameraSpeed = 5.0f;			// Set speed maximum
	}
}