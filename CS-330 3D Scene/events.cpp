/**
 * Griffin Nelson
 * 5/6/2021
 * 
 * "events.cpp" - Impementations for the callbacks to be bound to the GLFW event subsystem.
 *		Function prototypes defined in "events.h".
 */
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "events.h"

namespace events {
	/**
	 * Callback to handle framebuffer resize events.
	 */
	void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);	//set viewport to the full width and height of the new framebuffer
	}
}