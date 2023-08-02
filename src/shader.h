#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>

#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "errors.h"

namespace mvi { 

enum ShaderType {
	VERTEX = GL_VERTEX_SHADER,
	GEOMETRY = GL_GEOMETRY_SHADER,
	FRAGMENT = GL_FRAGMENT_SHADER
};

struct Shader {
	std::string path_to_code;
	ShaderType type;
};

class ShaderProgram final {
public:
	ShaderProgram() noexcept;
	ShaderProgram(const std::vector<Shader>& shaders) noexcept;
	ShaderProgram(std::vector<Shader>&& shaders) noexcept;

	void use();
	void compile_and_link();
	
	void load_shader(const Shader& shader);
	void load_shader(const std::string& path, ShaderType type);
	
	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator= (const ShaderProgram) = delete;

private:
	GLuint ID;
	bool linked;
	
	std::vector<Shader> shaders_;
	std::map<std::string, GLint> uniform_locations_;
	
	GLuint compile_shader(const Shader& shader);
	std::string read_shader_code(const std::string& path);
	
	GLint getUniformLocation(const std::string& name);

	void setUniform(const std::string& name, float value);
	void setUniform(const std::string& name, int value);
	void setUniform(const std::string& name, bool value);
	void setUniform(const std::string& name, GLuint value);
	void setUniform(const std::string& name, glm::vec2& vec);
	void setUniform(const std::string& name, glm::vec3& vec);
	void setUniform(const std::string& name, glm::vec4& vec);
	void setUniform(const std::string& name, glm::mat3& mat);
	void setUniform(const std::string& name, glm::mat4& mat);

};

} // namespace mvi

#endif
