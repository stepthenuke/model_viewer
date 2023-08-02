#ifndef RENDERER_H
#define RENDERER_H

#include <stdexcept>
#include <functional>

#include "include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "errors.h"

namespace mvi {

class Renderer final {
public:
	Renderer(unsigned int screen_width, unsigned int screen_height);
	void draw();
	
	GLFWwindow* get_window();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator= (const Renderer&) = delete;
	Renderer& operator= (Renderer&&) = delete;

private:
	static inline auto window_deleter_ = [](GLFWwindow*) { glfwTerminate(); };
	using window_ptr = std::unique_ptr<GLFWwindow, decltype(window_deleter_)>;
	window_ptr window_;

	GLFWwindow* init_window(unsigned int screen_width, unsigned int screen_height);
	
	static void error_callback_(int, const char* error_str);	
	static void framebuffer_size_callback_(GLFWwindow* window, int width, int height);
};

}; //namespace mvi

#endif
