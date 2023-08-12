#version 410 core

struct Material 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 emission;
	vec3 transmittance;
	float shininess;
	float refract_index;
	vec3 trans_filter;
	float alpha;
	int illum;
};

in vec3 frag_pos_f;
in vec2 tex_coord_f;
in vec3 normal_f;
in vec3 tangent_f;

uniform vec3 light_pos;
uniform vec3 view_pos;
// uniform sampler2D texture_ambient;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
// uniform sampler2D texture_emission;
// uniform sampler2D texture_transmit;
// uniform sampler2D texture_shininess;
// uniform sampler2D texure_transmition;
// uniform sampler2D texure_dissolve;
uniform sampler2D texture_bump;
uniform Material material;

out vec4 FragColor;

vec3 calculate_bump_normal() {
	vec3 normal = normal_f;
	vec3 tangent = tangent_f;
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vec3 bitangent = cross(tangent, normal);
	vec3 bump_map_normal = texture(texture_bump, tex_coord_f).xyz;
	bump_map_normal = 2.0 * bump_map_normal - 1.0;
	mat3 TBN = mat3(tangent, bitangent, normal);
	vec3 bump_normal = TBN * bump_map_normal;
	bump_normal = normalize(bump_normal);
	return bump_normal;
}

void main() {
	vec3 normal = calculate_bump_normal();

	// ambient
	// vec3 ambient_part = material.ambient * vec3(1.0, 1.0, 1.0);

	// diffuse
	vec3 light_dir = normalize(light_pos - frag_pos_f);
	vec3 dif_color = texture(texture_diffuse, tex_coord_f).xyz;
	float dif = max(dot(normal, light_dir), 0.0);
	vec3 diffuse_part = dif * dif_color;

	// specular
	vec3 view_dir = normalize(view_pos - frag_pos_f);
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
	vec3 spec_color = texture(texture_specular, tex_coord_f).xyz;
	vec3 specular_part = spec * spec_color;

	FragColor = vec4(diffuse_part + specular_part, 1.0);
}

