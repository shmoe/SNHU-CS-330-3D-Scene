/**
 * Griffin Nelson
 * 5/7/2021
 * 
 * "OpenGL-GLFW-GLAD Template Testing.cpp" - Contains the CppUnitTestFramework test class
 *		OpenGLGLFWGLADTemplateTesting and all of its test methods.
 */
#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "main.h"
#include "events.h"

namespace OpenGLGLFWGLADTemplateTesting
{
	TEST_CLASS(OpenGLGLFWGLADTemplateTesting)
	{
	public:

		TEST_METHOD(CreateGLFWWindow)
		{
			int window_width = 0,
				window_height = 0;

			GLFWwindow* window = create_glfw_window();
			Assert::IsNotNull(window, L"Window not created");

			glfwGetFramebufferSize(window, &window_width, &window_height);
			Assert::AreEqual(GLFW_WINDOW_WIDTH, window_width, L"Unexpected window width");
			Assert::AreEqual(GLFW_WINDOW_HEIGHT, window_height, L"Unexpected window height");

			glfwTerminate();
		}

		TEST_METHOD(InitGlad)
		{
			GLFWwindow* window = create_glfw_window();
			Assert::IsNotNull(window, L"See test: CreateGLFWWindow");

			Assert::AreEqual(0, init_glad(), L"GLAD failed to initialize");

			glfwTerminate();
		}

		TEST_METHOD(EventsFrameBufferSizeCallback)
		{
			int framebuffer_width = 0,
				framebuffer_height = 0;

			int viewport[4] = {
				0,	// viewport x coord
				0,	// viewport y coord
				0,	// viewport width
				0	// viewport height
			};


			GLFWwindow* window = create_glfw_window();
			Assert::IsNotNull(window, L"See test: CreateGLFWWindow");

			Assert::AreEqual(0, init_glad(), L"See test: InitGlad");

			glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);
			events::framebuffer_size_callback(window, framebuffer_width / 2, framebuffer_height / 2);
			glGetIntegerv(GL_VIEWPORT, viewport);

			Assert::AreEqual(framebuffer_width / 2, viewport[2], L"Framebuffer width does not match viewport width");
			Assert::AreEqual(framebuffer_height / 2, viewport[3], L"Framebuffer height does not match viewport height");

			glfwTerminate();
		}
	};
}

