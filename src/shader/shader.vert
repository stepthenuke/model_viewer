#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; 

out vec2 tex_coord_f;

void main() {
	gl_Position = projection * view * model * vec4(pos.x, pos.y, pos.z, 1.0);
	tex_coord_f = tex_coord;
}
