#include "include/glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>

int main() {
	
	glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);		
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		    std::cout << "Failed to initialize GLAD" << std::endl;
			return -1;
	}    

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << nrAttributes << std::endl;
}
