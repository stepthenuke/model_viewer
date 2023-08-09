#version 410 core

in vec2 tex_coord_f;

out vec4 FragColor;

uniform sampler2D texture_ambient;
uniform sampler2D texture_diffuse;

void main() {
	FragColor = texture(texture_diffuse, tex_coord_f);
//	FragColor = vec4(0.3f, 0.3f, 1.f, 1.f);
}

