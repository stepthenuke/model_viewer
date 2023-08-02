#include "renderer.h"

namespace mvi {

Renderer::Renderer(unsigned int screen_width, unsigned int screen_height) :
	window_{init_window(screen_width, screen_height), window_deleter_}
{}

GLFWwindow* Renderer::init_window(unsigned int screen_width, unsigned int screen_height) {
	glfwSetErrorCallback(&Renderer::error_callback_);
	if (!glfwInit()) {
		throw glfw_error("Failed to init OpenGL");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Model Viewer 2000", NULL, NULL);
	if (window == NULL) {
		throw glfw_error("Failed to create GLFW window");
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, &Renderer::framebuffer_size_callback_);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		throw glfw_error("Failed to load GLAD");
	}
	return window;
}

void Renderer::error_callback_(int, const char* error_str) {
	throw glfw_error(error_str);
}

void Renderer::framebuffer_size_callback_(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

GLFWwindow* Renderer::get_window() {
	return window_.get();
}

void Renderer::draw() {
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

}
