/**
 * Griffin Nelson
 * 5/7/2021
 *
 * "main.h" - Function prototypes for the functions used by "main.cpp". Defined here to
 *		better allow for unit testing. Function implementations defined in "main.cpp"
 */
#pragma once
#ifndef __MAIN_H___
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/**
 * Configuration constants
 */
const int LOCAL_GL_VERSION[2] = { 3, 3 };	// OpenGL version to use
const int GLFW_WINDOW_WIDTH = 800;			// Initial width of GLFW render window
const int GLFW_WINDOW_HEIGHT = 600;			// Initial height of GLFW render window

GLFWwindow* create_glfw_window();			// Initialize GLFW and create the main render window
int init_glad();							// Initialize GLAD. Returns -1 on failure

unsigned int gen_shader_program(const char* vertex_shader_source, const char* fragment_shader_source);
#endif//__MAIN_H___