#version 410 core

layout (location = 0) in vec3 pos;

uniform mat4 projection;
uniform mat4 view;

void main() {
	gl_Position = projection * view * vec4(pos.xyz, 1.0);
}
