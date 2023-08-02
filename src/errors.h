#ifndef ERRORS_H
#define ERRORS_H

#include <stdexcept>
#include <string>

namespace mvi {

struct glfw_error : public std::runtime_error {
	glfw_error(const char* str) : std::runtime_error{str} {}
};

struct glsl_error : public std::runtime_error {
	std::string ShaderLog;
	glsl_error(const char* str) : std::runtime_error{str} {}
};

struct glsl_compile_error : public glsl_error {
	glsl_compile_error(const char* str, GLuint ShaderID) :
		glsl_error{str}
	{
		GLint length;
		glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> ShaderLogTemp(length);
		glGetShaderInfoLog(ShaderID, length, NULL, ShaderLogTemp.data());
		ShaderLog.assign(ShaderLogTemp.begin(), ShaderLogTemp.end());
	}
};

struct glsl_link_error : public glsl_error {
	glsl_link_error(const char* str, GLuint ProgramID) :
		glsl_error{str}
	{
		GLint length;
		glGetProgramiv(ProgramID, GL_LINK_STATUS, &length);
		std::vector<char> ProgramLogTemp(length);
		glGetProgramInfoLog(ProgramID, length, NULL, ProgramLogTemp.data());
		ShaderLog.assign(ProgramLogTemp.begin(), ProgramLogTemp.end());
	}
};

} // namespace mvi

#endif

