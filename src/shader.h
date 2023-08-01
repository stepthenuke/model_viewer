#include <string>
#include <sstream>
#include <iostream>

#include "include/glad/glad.h"
#include <GLFW/glfw3.h>

namespace mvi { 

class Shader final {
private:
	std::string path_to_shader_;
	std::string code_;
public:
	
};

class ShaderProgram final {
private:
	Shader vertex_shader_;
	Shader geometry_shader_;
	Shader fragment_shader_;
	
public:
	GLuint ID;
	
};

} // namespace mvi
