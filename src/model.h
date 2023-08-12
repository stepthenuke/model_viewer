#ifndef MODEL_H
#define MODEL_H

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cglm/cglm.h>
#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include "include/stb_image.h"
#include "include/fast_obj.h"

#ifdef DEBUG
unsigned int MALLOC_CHECK = 0;
#define malloc(x) \
	malloc(x);    \
	MALLOC_CHECK++
#define strdup(x) \
	strdup(x);    \
	MALLOC_CHECK++
#define free(x) \
	free(x);    \
	MALLOC_CHECK--
#endif

typedef struct
{
	unsigned int texture_id;
	char *name;
	char *path;
} Texture;

typedef struct
{
	char *name;

	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Ke[3];
	float Kt[3];
	float Ns;	
	float Ni;	 
	float Tf[3];
	float d;	 
	int illum;	 

	Texture map_Ka;
	Texture map_Kd;
	Texture map_Ks;
	Texture map_Ke;
	Texture map_Kt;
	Texture map_Ns;
	Texture map_Ni;
	Texture map_d;
	Texture map_bump;
} Material;

typedef struct
{
	unsigned int VAO;
	unsigned int VBO;
	unsigned int vert_count;
	unsigned int material_index;
	char *name;
	float *vertices;
} Mesh;

typedef struct
{
	unsigned int mesh_count;
	unsigned int material_count;
	Mesh *meshes;
	Material *materials;
} Model;

static unsigned int loaded_textures_size = 0;
static Texture *loaded_textures = NULL;

Model *read_model(const char *path);
Model *make_model(fastObjMesh *fo_mesh);
void texture_from_fo_texture(Texture *texture, fastObjTexture *fo_texture);
void make_material(Material *material, fastObjMesh *fo_mesh, unsigned int mat_num);
void make_mesh(Mesh *mesh, fastObjMesh *fo_mesh, unsigned int obj_num);
float *fill_vert_with_face(float *cur_vert_pos, fastObjMesh *fo_mesh, unsigned int ind_index);
void mesh_calculate_tangents(Mesh *mesh); 
void destroy_texture(Texture *texture);
void destroy_model(Model *model);
void destroy_mesh(Mesh *mesh);
void destroy_material(Material *material);

void loaded_texture_size_increase();
void load_texture_if_not_loaded(Texture *texture);
int find_loaded_texture(const char *name);
void load_textures(Model *model);
unsigned int load_texture_gl(const char *path);
void loaded_texture_size_increase();
void draw_model(Model *model, unsigned int shader_id);
void draw_mesh(Model *model, Mesh *mesh, unsigned int shader_id);
void shader_set_material(unsigned int shader_id, Material *material);
void load_mesh_gl(Mesh *mesh);
void load_model_gl(Model *model);

void fo_helper_print_arrf(float *arr, int size);
void fo_helper_print_mat_info(fastObjMaterial *material);
void fo_helper_print_group_info(fastObjGroup *group);
void fo_helper_print_mesh_info(fastObjMesh *mesh, int verbose);
void mesh_info(Mesh *mesh);
void material_info(Material *material);
void model_info(Model *model, int verbose);

#endif // MODEL_H
