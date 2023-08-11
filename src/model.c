#include "model.h"

void load_mesh_gl(Mesh *mesh)
{
	glGenVertexArrays(1, &mesh->VAO);
	glGenBuffers(1, &mesh->VBO);

	glBindVertexArray(mesh->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
	glBufferData(GL_ARRAY_BUFFER, mesh->vert_count * sizeof(float), mesh->vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	
	glBindVertexArray(0);
}

void load_model_gl(Model *model) 
{
	for (int i = 0; i < model->mesh_count; i++) {
		load_mesh_gl(model->meshes + i);
	}
	load_textures(model);
}

void loaded_texture_size_increase()
{
	loaded_textures_size++;
	if (loaded_textures != NULL)
	{
		loaded_textures = realloc(loaded_textures, (loaded_textures_size) * sizeof(Texture));
	}
	else
	{
		loaded_textures = malloc(sizeof(Texture));
	}
}

unsigned int load_texture_gl(const char *path)
{
	unsigned int texture_id;

	glGenTextures(1, &texture_id);

	int width, height, num_components;
	unsigned char *img = stbi_load(path, &width, &height, &num_components, 0);
	if (img != NULL)
	{
		GLenum components_type = 0; 
		if (num_components == 1) 
		{
			components_type = GL_RED;
		}
		if (num_components == 3) 
		{
			components_type = GL_RGB;
		}
		else if (num_components == 4) 
		{
			components_type = GL_RGBA;
		}
		
		glBindTexture(GL_TEXTURE_2D, texture_id);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, components_type, width, height, 0, components_type, GL_UNSIGNED_BYTE, img);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		fprintf(stderr, "Failed to load texture: %s\n", path);
		exit(1);
	}

	stbi_image_free(img);

	return texture_id;
}

void draw_model(Model *model, unsigned int shader_id)
{
	glUseProgram(shader_id);
	for (int i = 0; i < model->mesh_count; i++)
	{
		draw_mesh(model, model->meshes + i, shader_id);
	}
}

void activate_texture(Texture *texture, unsigned int shader_id, const char *loc_name, int *slot)
{
	if (texture->name == NULL) return;
	int loc = glGetUniformLocation(shader_id, loc_name);
	glUniform1i(loc, *slot);
	glActiveTexture(GL_TEXTURE0 + *slot);
	glBindTexture(GL_TEXTURE_2D, texture->texture_id);
	*slot += 1;
}	

void draw_mesh(Model *model, Mesh *mesh, unsigned int shader_id)
{
	Material *material = model->materials + mesh->material_index;
	int slot = 0;
	activate_texture(&material->map_Ka, shader_id, "texture_ambient", &slot);
	activate_texture(&material->map_Kd, shader_id, "texture_diffuse", &slot);
	activate_texture(&material->map_Ks, shader_id, "texture_specular", &slot);
	activate_texture(&material->map_Ke, shader_id, "texture_emission", &slot);
	activate_texture(&material->map_Kt, shader_id, "texture_transmit", &slot);
	activate_texture(&material->map_Ns, shader_id, "texture_shininess", &slot);
	activate_texture(&material->map_Ni, shader_id, "texture_transmition", &slot);
	activate_texture(&material->map_d,  shader_id, "texture_dissolve", &slot);
	activate_texture(&material->map_bump, shader_id, "texture_bump", &slot);
	
	glBindVertexArray(mesh->VAO);
	glDrawArrays(GL_TRIANGLES, 0, mesh->vert_count / 8);
	glBindVertexArray(0);

	glActiveTexture(GL_TEXTURE0);
}

void load_texture_if_not_loaded(Texture *texture)
{
	if (texture->name == NULL)
		return;
	int loaded_texture_index = find_loaded_texture(texture->name);
	if (loaded_texture_index == -1)
	{
		texture->texture_id = load_texture_gl(texture->path);
		loaded_texture_size_increase();
		memcpy(loaded_textures + loaded_textures_size - 1, texture, sizeof(Texture));
	}
	else
	{
		texture->texture_id = loaded_textures[loaded_texture_index].texture_id;
	}
}

int find_loaded_texture(const char *name)
{
	for (int i = 0; i < loaded_textures_size; i++)
	{
		if (strcmp(name, loaded_textures[i].name) == 0)
			return i;
	}
	return -1;
}

void load_textures(Model *model)
{
	for (int i = 0; i < model->material_count; i++)
	{
		Material *material = model->materials + i;
		load_texture_if_not_loaded(&material->map_Ka);
		load_texture_if_not_loaded(&material->map_Kd);
		load_texture_if_not_loaded(&material->map_Ks);
		load_texture_if_not_loaded(&material->map_Ke);
		load_texture_if_not_loaded(&material->map_Kt);
		load_texture_if_not_loaded(&material->map_Ns);
		load_texture_if_not_loaded(&material->map_Ni);
		load_texture_if_not_loaded(&material->map_d);
		load_texture_if_not_loaded(&material->map_bump);
	}

	if (loaded_textures != NULL)
	{
		free(loaded_textures);
	}
}

Model *read_model(const char *path)
{
	fastObjMesh *fo_mesh = fast_obj_read(path);
	Model *model = make_model(fo_mesh);
	fast_obj_destroy(fo_mesh);
	return model;
}

Model *make_model(fastObjMesh *fo_mesh)
{
	Model *model = malloc(sizeof(Model));

	model->mesh_count = fo_mesh->object_count;
	model->meshes = malloc(model->mesh_count * sizeof(Mesh));
	for (unsigned int i = 0; i < model->mesh_count; i++)
	{
		make_mesh(model->meshes + i, fo_mesh, i);
	}

	model->material_count = fo_mesh->material_count;
	model->materials = malloc(model->material_count * sizeof(Material));
	for (unsigned int i = 0; i < model->material_count; i++)
	{
		make_material(model->materials + i, fo_mesh, i);
	}
	return model;
}

void texture_from_fo_texture(Texture *texture, fastObjTexture *fo_texture)
{
	if (fo_texture->name != NULL)
	{
		texture->name = strdup(fo_texture->name);
	}
	else
	{
		texture->name = NULL;
	}
	if (fo_texture->path != NULL)
	{
		texture->path = strdup(fo_texture->path);
	}
	else
	{
		texture->path = NULL;
	}
}

void make_material(Material *material, fastObjMesh *fo_mesh, unsigned int mat_num)
{
	fastObjMaterial *fo_mat = fo_mesh->materials + mat_num;
	memcpy(material, fo_mat, offsetof(fastObjMaterial, map_Ka));
	texture_from_fo_texture(&material->map_Kd, &fo_mat->map_Kd);
	texture_from_fo_texture(&material->map_Ka, &fo_mat->map_Ka);
	texture_from_fo_texture(&material->map_Ks, &fo_mat->map_Ks);
	texture_from_fo_texture(&material->map_Ke, &fo_mat->map_Ke);
	texture_from_fo_texture(&material->map_Kt, &fo_mat->map_Kt);
	texture_from_fo_texture(&material->map_Ns, &fo_mat->map_Ns);
	texture_from_fo_texture(&material->map_Ni, &fo_mat->map_Ni);
	texture_from_fo_texture(&material->map_d, &fo_mat->map_d);
	texture_from_fo_texture(&material->map_bump, &fo_mat->map_bump);
	material->name = strdup(fo_mat->name);
}

void make_mesh(Mesh *mesh, fastObjMesh *fo_mesh, unsigned int obj_num)
{
	fastObjGroup *cur_obj = fo_mesh->objects + obj_num;

	unsigned int indices_arr_index = cur_obj->index_offset;

	unsigned int face_start = cur_obj->face_offset;
	unsigned int face_end = face_start + cur_obj->face_count;
	unsigned int first_face_material_ind = fo_mesh->face_materials[face_start];

	mesh->vert_count = cur_obj->face_count * 3 * 8;
	mesh->name = strdup(cur_obj->name);

	unsigned int vert_size = cur_obj->face_count * 3 * 8 * sizeof(float);
	mesh->vertices = malloc(vert_size); // 8 is not magic as 3 (v: x, y, z) + 3 (vn) + 2 (t) for each face
	assert(mesh->vertices != NULL);
	float *cur_vert_pos = mesh->vertices;

	for (int i = face_start; i < face_end; i++)
	{
		if (first_face_material_ind != fo_mesh->face_materials[i])
		{
			printf("NO IMPLEMENTATION: MATERIALS ARE DIFFERENT\n");
			exit(1);
		}
		unsigned int face_size = fo_mesh->face_vertices[i];
		if (face_size != 3)
		{
			printf("NO IMPLEMENTATION: FACE IS NOT A TRIANGLE, %s\n", cur_obj->name);
			exit(1);
		}
		for (int j = 0; j < face_size; j++)
		{
			cur_vert_pos = fill_vert_with_face(cur_vert_pos, fo_mesh, indices_arr_index + j);
		}
		indices_arr_index += face_size;
	}
	// so now vert is filled as [v1 vn1 vt1 v2 vn2 vt2 ...]
	// think maybe of some material structure (now for every mesh we're making a copy of material struct -> that's stupid)
	mesh->material_index = first_face_material_ind;
}

float *fill_vert_with_face(float *cur_vert_pos, fastObjMesh *fo_mesh, unsigned int ind_index)
{
	unsigned int p = 3 * fo_mesh->indices[ind_index].p;
	unsigned int n = 3 * fo_mesh->indices[ind_index].n;
	unsigned int t = 2 * fo_mesh->indices[ind_index].t;

	memcpy(cur_vert_pos, &fo_mesh->positions[p], 3 * sizeof(float));
	memcpy(cur_vert_pos + 3, &fo_mesh->normals[n], 3 * sizeof(float));
	memcpy(cur_vert_pos + 6, &fo_mesh->texcoords[t], 2 * sizeof(float));
	cur_vert_pos += 8;

	return cur_vert_pos;
}

void destroy_texture(Texture *texture)
{
	if (texture->name != NULL)
	{
		free(texture->name);
	}
	if (texture->path != NULL)
	{
		free(texture->path);
	}
}

void destroy_material(Material *material)
{
	free(material->name);
	destroy_texture(&material->map_Ka);
	destroy_texture(&material->map_Kd);
	destroy_texture(&material->map_Ks);
	destroy_texture(&material->map_Ke);
	destroy_texture(&material->map_Kt);
	destroy_texture(&material->map_Ns);
	destroy_texture(&material->map_Ni);
	destroy_texture(&material->map_d);
	destroy_texture(&material->map_bump);
}

void destroy_mesh(Mesh *mesh)
{
	if (mesh->name != NULL)
	{
		free(mesh->name);
	}
	if (mesh->vertices != NULL)
	{
		free(mesh->vertices);
	}
}

void destroy_model(Model *model)
{
	for (int i = 0; i < model->mesh_count; i++)
	{
		destroy_mesh(model->meshes + i);
	}
	for (int i = 0; i < model->material_count; i++)
	{
		destroy_material(model->materials + i);
	}
	free(model->meshes);
	free(model->materials);
	free(model);
}

void mesh_info(Mesh *mesh)
{
	printf("Mesh name: %s\n", mesh->name);
	printf("Vert count: %d\n", mesh->vert_count);
	printf("Material index: %d\n", mesh->material_index);
}

void material_info(Material *material)
{
	printf("Material name: %s\n", material->name);
	printf("Ambient Ka: ");
	fo_helper_print_arrf(material->Ka, 3);
	printf("Diffuse Kd: ");
	fo_helper_print_arrf(material->Kd, 3);
	printf("Specular Ks: ");
	fo_helper_print_arrf(material->Ks, 3);
	printf("Emmission Ke: ");
	fo_helper_print_arrf(material->Ke, 3);
	printf("Transmittance Kt: ");
	fo_helper_print_arrf(material->Kt, 3);
	printf("Shininess Ns: %f\n", material->Ns);
	printf("Refraction index Ni: %f\n", material->Ni);
	printf("Transmition filter Tf: ");
	fo_helper_print_arrf(material->Tf, 3);
	printf("Dissolve (alpha): %f\n", material->d);
	printf("Illumination model: %d\n", material->illum);
	printf("id: %d;  %s, path: %s\n", material->map_Ka.texture_id, material->map_Ka.name, material->map_Ka.path);
	printf("id: %d;  %s, path: %s\n", material->map_Kd.texture_id, material->map_Kd.name, material->map_Kd.path);
	printf("id: %d;  %s, path: %s\n", material->map_Ks.texture_id, material->map_Ks.name, material->map_Ks.path);
	printf("id: %d;  %s, path: %s\n", material->map_Ke.texture_id, material->map_Ke.name, material->map_Ke.path);
	printf("id: %d;  %s, path: %s\n", material->map_Kt.texture_id, material->map_Kt.name, material->map_Kt.path);
	printf("id: %d;  %s, path: %s\n", material->map_Ns.texture_id, material->map_Ns.name, material->map_Ns.path);
	printf("id: %d;  %s, path: %s\n", material->map_Ni.texture_id, material->map_Ni.name, material->map_Ni.path);
	printf("id: %d;  %s, path: %s\n", material->map_d.texture_id, material->map_d.name, material->map_d.path);
	printf("id: %d;  %s, path: %s\n", material->map_bump.texture_id, material->map_bump.name, material->map_bump.path);
}

void model_info(Model *model, int verbose)
{
	printf("mesh_count: %d\n", model->mesh_count);
	printf("material_count: %d\n", model->material_count);

	if (verbose)
	{
		for (int i = 0; i < model->mesh_count; i++)
		{
			printf("%d\n", i);
			mesh_info(model->meshes + i);
		}
		printf("\n");

		for (int i = 0; i < model->material_count; i++)
		{
			printf("%d\n", i);
			material_info(model->materials + i);
		}
	}
}

void fo_helper_print_arrf(float *arr, int size)
{
	for (int i = 0; i < size; ++i)
	{
		printf("%f ", arr[i]);
	}
	printf("\n");
}

void fo_helper_print_mat_info(fastObjMaterial *material)
{
	printf("Material name: %s\n", material->name);
	printf("Ambient Ka: ");
	fo_helper_print_arrf(material->Ka, 3);
	printf("Diffuse Kd: ");
	fo_helper_print_arrf(material->Kd, 3);
	printf("Specular Ks: ");
	fo_helper_print_arrf(material->Ks, 3);
	printf("Emmission Ke: ");
	fo_helper_print_arrf(material->Ke, 3);
	printf("Transmittance Kt: ");
	fo_helper_print_arrf(material->Kt, 3);
	printf("Shininess Ns: %f\n", material->Ns);
	printf("Refraction index Ni: %f\n", material->Ni);
	printf("Transmition filter Tf: ");
	fo_helper_print_arrf(material->Tf, 3);
	printf("Dissolve (alpha): %f\n", material->d);
	printf("Illumination model: %d\n", material->illum);
	printf("%s, path: %s\n", material->map_Ka.name, material->map_Ka.path);
	printf("%s, path: %s\n", material->map_Kd.name, material->map_Kd.path);
	printf("%s, path: %s\n", material->map_Ks.name, material->map_Ks.path);
	printf("%s, path: %s\n", material->map_Ke.name, material->map_Ke.path);
	printf("%s, path: %s\n", material->map_Kt.name, material->map_Kt.path);
	printf("%s, path: %s\n", material->map_Ns.name, material->map_Ns.path);
	printf("%s, path: %s\n", material->map_Ni.name, material->map_Ni.path);
	printf("%s, path: %s\n", material->map_d.name, material->map_d.path);
	printf("%s, path: %s\n", material->map_bump.name, material->map_bump.path);
}

void fo_helper_print_group_info(fastObjGroup *group)
{
	printf("Group name: %s\n", group->name);
	printf("Num of faces: %d\n", group->face_count);
	printf("Face ffset: %d\n", group->face_offset);
	printf("Index offset: %d\n", group->index_offset);
}

void fo_helper_print_mesh_info(fastObjMesh *mesh, int verbose)
{
	printf("position_count: %d\n", mesh->position_count);
	printf("texcoord_count: %d\n", mesh->texcoord_count);
	printf("normal_count: %d\n", mesh->normal_count);
	printf("face_count: %d\n", mesh->face_count);
	printf("index_count: %d\n", mesh->index_count);

	printf("object_count: %d\n", mesh->object_count);
	if (verbose)
	{
		for (int i = 0; i < mesh->object_count; i++)
		{
			printf("%d.\n", i);
			fo_helper_print_group_info(&mesh->objects[i]);
		}
		printf("\n");
	}

	printf("group_count: %d\n", mesh->group_count);
	if (verbose)
	{
		for (int i = 0; i < mesh->group_count; i++)
		{
			printf("%d.\n", i);
			fo_helper_print_group_info(&mesh->groups[i]);
		}
		printf("\n");
	}

	printf("material_count: %d\n", mesh->material_count);
	if (verbose)
	{
		for (int i = 0; i < mesh->material_count; i++)
		{
			printf("%d.\n", i);
			fo_helper_print_mat_info(&mesh->materials[i]);
		}
		printf("\n");
	}
}
