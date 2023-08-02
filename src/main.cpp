#include <iostream>

#include "include/glad/glad.h"
#include <GLFW/glfw3.h>

#include "renderer.h"
#include "shader.h"


float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f,  0.5f, 0.0f
};  

int main() try {
	mvi::Renderer renderer {800, 800};

	mvi::ShaderProgram program;
	program.load_shader("shader/shader.vert", mvi::VERTEX);
	program.load_shader("shader/shader.frag", mvi::FRAGMENT);
	program.compile_and_link();

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	glBindVertexArray(0); 

	while (!glfwWindowShouldClose(renderer.get_window())) {
		renderer.draw();

		program.use();
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(renderer.get_window());
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

}
catch (mvi::glsl_compile_error& e) {
	std::cout << e.ShaderLog << std::endl; 
}
catch (mvi::glsl_link_error& e) {
	std::cout << e.ShaderLog << std::endl;
}
