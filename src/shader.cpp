#include "shader.h"

namespace mvi {

ShaderProgram::ShaderProgram() noexcept: 
	ID {0}, linked{false} 
{}

ShaderProgram::ShaderProgram(const std::vector<Shader>& shaders) noexcept :
	ShaderProgram()
{
	std::vector<Shader> copy {shaders};
	ShaderProgram(std::move(copy));
}	

ShaderProgram::ShaderProgram(std::vector<Shader>&& shaders) noexcept :
	ShaderProgram()
{
	std::swap(shaders_, shaders);
}

void ShaderProgram::use() {
	if (!linked || ID <= 0) {
		throw glsl_error{"Shader program's not linked"};
	}
	glUseProgram(ID);
};

void ShaderProgram::compile_and_link() {	
	if (linked) return;
	
	ID = glCreateProgram();
	if (!ID) {
		throw glsl_error{"Failed to create program"};
	}	

	std::vector<GLuint> shaderIDs;
	std::transform(shaders_.begin(), shaders_.end(), std::back_inserter(shaderIDs), 
		[this](const Shader& shad) { return compile_shader(shad); }
	);
		
	for (auto&& shaderID : shaderIDs) {
		glAttachShader(ID, shaderID);
	}
	glLinkProgram(ID);
	
	GLint status;
	glGetProgramiv(ID, GL_LINK_STATUS, &status);
	if (!status) {
		throw glsl_link_error("Failed to link program", ID);
	}
	linked = true;

	for (auto&& shaderID : shaderIDs) {
		glDeleteShader(shaderID);
	}
}

void ShaderProgram::load_shader(const Shader& shader) {
	shaders_.push_back(shader);
}

void ShaderProgram::load_shader(const std::string& path, ShaderType type) {
	load_shader(Shader{path, type});
};

GLuint ShaderProgram::compile_shader(const Shader& shader) {
	GLuint shaderID = glCreateShader(shader.type);
	std::string shader_code = read_shader_code(shader.path_to_code);
	const char* code = shader_code.c_str();
	glShaderSource(shaderID, 1, &code, NULL);
	glCompileShader(shaderID);
	
	GLint status;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);
	if (!status) {
		throw glsl_compile_error(("Failed to compile shader: " + shader.path_to_code).c_str(), shaderID);
	}
	return shaderID;
}

std::string ShaderProgram::read_shader_code(const std::string& path) {
	std::ifstream shader_file (path, std::ios::in);
	std::stringstream shader_code_stream;
	shader_code_stream << shader_file.rdbuf();
	shader_file.close();
	std::string shader_code (shader_code_stream.str());
	return shader_code;
}

GLint ShaderProgram::getUniformLocation(const std::string& name) {
	auto it = uniform_locations_.find(name);
	if (it != uniform_locations_.end()) {
		return it->second;
	}
	GLint location = glGetUniformLocation(ID, name.c_str());
	uniform_locations_[name] = location;
	return location;
}
void ShaderProgram::setUniform(const std::string& name, float value) {
	GLint location = getUniformLocation(name);
	glUniform1f(location, value);
}

void ShaderProgram::setUniform(const std::string& name, int value) {
	GLint location = getUniformLocation(name);
	glUniform1i(location, value);
}


void ShaderProgram::setUniform(const std::string& name, bool value) {
	GLint location = getUniformLocation(name);
	glUniform1i(location, value);	
}


void ShaderProgram::setUniform(const std::string& name, GLuint value) {
	GLint location = getUniformLocation(name);
	glUniform1ui(location, value);
}


void ShaderProgram::setUniform(const std::string& name, glm::vec2& vec) {
	GLint location = getUniformLocation(name);
	glUniform2f(location, vec.x, vec.y);
}


void ShaderProgram::setUniform(const std::string& name, glm::vec3& vec) {
	GLint location = getUniformLocation(name);
	glUniform3f(location, vec.x, vec.y, vec.z);
}


void ShaderProgram::setUniform(const std::string& name, glm::vec4& vec) {
	GLint location = getUniformLocation(name);
	glUniform4f(location, vec.x, vec.y, vec.z, vec.w);
}


void ShaderProgram::setUniform(const std::string& name, glm::mat3& mat) {
	GLint location = getUniformLocation(name);
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}


void ShaderProgram::setUniform(const std::string& name, glm::mat4& mat) {
	GLint location = getUniformLocation(name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}


}; //namespace mvi

