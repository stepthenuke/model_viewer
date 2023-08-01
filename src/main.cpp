#include <iostream>

#include "include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "renderer.h"

int main() {
	mvi::Renderer renderer {800, 800};
	
	while (!glfwWindowShouldClose(renderer.get_window())) {
		glfwSwapBuffers(renderer.get_window());
		glfwPollEvents();
	}

}
