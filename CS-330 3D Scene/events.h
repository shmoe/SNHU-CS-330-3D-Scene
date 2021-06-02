/**
 * Griffin Nelson
 * 5/6/2021
 * 
 * "events.h" - Function prototypes for the callbacks to be bound to the GLFW event subsystem.
 *		Function implementations defined in "events.cpp".
 */

#pragma once
#ifndef __EVENTS_H__
namespace events {
	/**
	 * Callback function that reacts to a
	 *	framebuffer resize event.
	 */
	void framebuffer_size_callback(GLFWwindow* window, int width, int height);
}
#endif//__EVENTS_H__