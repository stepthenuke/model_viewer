#version 410 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tex_coord;
layout (location = 3) in vec3 tangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model; 

out vec2 tex_coord_f;
out vec3 normal_f;
out vec3 tangent_f;
out vec3 frag_pos_f;

void main() {
	gl_Position = projection * view * model * vec4(pos, 1.0);
	tex_coord_f = tex_coord;
	frag_pos_f = vec3(model * vec4(pos, 1.0));
	normal_f = normalize(vec3(model * vec4(normal, 0.0)));
	tangent_f = normalize(vec3(model * vec4(tangent, 0.0)));
}
