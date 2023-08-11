#include "shader.h"

char *read_shader(const char* path_to_shader) {
	FILE *shader_file;
	shader_file = fopen(path_to_shader, "r");
	if (shader_file == NULL) {
		fprintf(stderr, "No such file: %s\n", path_to_shader);
		return NULL;
	}

	int len = 0;
	while (fgetc(shader_file) != EOF) {
		len++;
	}

	char *buf = malloc(len + 1);
	if (buf == NULL) {
		fclose(shader_file);
		return NULL;
	}
	
	int c;
	int pos = 0;
	fseek(shader_file, 0L, SEEK_SET);
	while ((c = fgetc(shader_file)) != EOF) {
		buf[pos++] = (char) c;
	}
	buf[pos] = '\0';
	
	return buf;
}

GLuint compile_shader(const char *shader_source, GLenum shader_type) {
	GLuint shader_id = glCreateShader(shader_type);
	glShaderSource(shader_id, 1, &shader_source, NULL);
	glCompileShader(shader_id);
	int success;
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	if (!success) {
		char info_log[1024];
		glGetShaderInfoLog(shader_id, 1024, NULL, info_log);
		fprintf(stderr, "Failed to compile shader: %s\n%s", info_log, shader_source);
		return 0;
	}
	return shader_id;
}

GLuint load_program(const char* vert_shader_path, const char* geom_shader_path, const char* frag_shader_path) {
	int success = 0;
	
	const char* vert_shader_source = read_shader(vert_shader_path);
	GLuint vert_shader_id = compile_shader(vert_shader_source, GL_VERTEX_SHADER);
	if (!vert_shader_id) {
		goto cleanup_vert;
	}

	GLuint geom_shader_id = 0;
	const char* geom_shader_source = NULL;
	if (geom_shader_path != NULL) {
		geom_shader_source = read_shader(geom_shader_path);
		GLuint geom_shader_id = compile_shader(geom_shader_source, GL_GEOMETRY_SHADER);
		if (!geom_shader_id) {
			goto cleanup_geom;
		}
	}

	const char* frag_shader_source = read_shader(frag_shader_path);
	GLuint frag_shader_id = compile_shader(frag_shader_source, GL_FRAGMENT_SHADER);
	if (!frag_shader_id) {
		goto cleanup_frag;
	}

	GLuint shader_program_id = glCreateProgram();
	glAttachShader(shader_program_id, vert_shader_id);
	if (geom_shader_id) {
		glAttachShader(shader_program_id, geom_shader_id);
	}
	glAttachShader(shader_program_id, frag_shader_id);
	glLinkProgram(shader_program_id);

	glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
	if (!success) {
		char info_log[1024];
		glGetProgramInfoLog(shader_program_id, 1024, NULL, info_log);
		fprintf(stderr, "Failed to link shader program: %s\n", info_log);
		glDeleteProgram(shader_program_id);
	}

cleanup_frag:	
	glDeleteShader(frag_shader_id);
	free((char*) frag_shader_source);

cleanup_geom:
	if (geom_shader_id) {
		glDeleteShader(geom_shader_id);
		free((char*) geom_shader_source);
	}

cleanup_vert:
	glDeleteShader(vert_shader_id);
	free((char*) vert_shader_source);
	
	if (success) {
		return shader_program_id;
	}
	return 0;
}
