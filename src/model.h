#ifndef MODEL_H
#define MODEL_H

#define FAST_OBJ_IMPLEMENTATION
#include "include/fast_obj.h"
#undef FAST_OBJ_IMPLEMENTATION

#include "assert.h"

void print_arrf(float* arr, int size);
void print_mat_info(fastObjMaterial *material);
void print_group_info(fastObjGroup *group);
void print_mesh_info(fastObjMesh *mesh, int verbose);

typedef struct {
	unsigned int vert_count;
	char *name;
	fastObjMaterial *material;
	float *vertices;
} Mesh;

typedef struct {
	unsigned int mesh_count;
	Mesh *meshes;
} Model;

Model *load_model(const char* path);
Model *make_model(fastObjMesh *fo_mesh);
void make_mesh(Mesh *mesh, fastObjMesh *fo_mesh, unsigned int object_num);
float *fill_vert_with_face(float *cur_vert_pos, fastObjMesh *fo_mesh, unsigned int ind_index);
void destroy_mesh(Mesh *mesh);

Model *load_model(const char* path) {
	fastObjMesh *fo_mesh = fast_obj_read(path);	
	Model *model = make_model(fo_mesh);
	fast_obj_destroy(fo_mesh);
	return model;
}

Model *make_model(fastObjMesh *fo_mesh) {
	Model *model = malloc(sizeof(Model));
	
	model->mesh_count = fo_mesh->object_count;
	model->meshes = malloc(model->mesh_count * sizeof(Mesh));
	for (unsigned int obj_n = 0; obj_n < fo_mesh->object_count; obj_n++) {
		make_mesh(model->meshes + obj_n, fo_mesh, obj_n);
	}

	return model;
}

void make_mesh(Mesh *mesh, fastObjMesh *fo_mesh, unsigned int object_num) {
	fastObjGroup *cur_obj = &fo_mesh->objects[object_num];
	
	unsigned int indices_arr_index = cur_obj->index_offset;
	
	unsigned int face_start = cur_obj->face_offset;
	unsigned int face_end = face_start + cur_obj->face_count;
	unsigned int first_face_material_ind = fo_mesh->face_materials[face_start];
	
	mesh->name = strdup(cur_obj->name);
	mesh->vert_count = cur_obj->face_count * 8 * 3;
	
	unsigned int vert_size = cur_obj->face_count * 3 *  8 * sizeof(float) ;
	mesh->vertices = malloc(vert_size); // 8 is not magic as 3 (v: x, y, z) + 3 (vn) + 2 (t) for each face	
	assert(mesh->vertices != NULL);
	float *cur_vert_pos = mesh->vertices;

	for (int i = face_start; i < face_end; i++) {
		if (first_face_material_ind != fo_mesh->face_materials[i]) {
			printf("NO IMPLEMENTATION: MATERIALS ARE DIFFERENT\n");
			exit(1);
		}
		unsigned int face_size = fo_mesh->face_vertices[i];
		if (face_size != 3) {
			printf("NO IMPLEMENTATION: FACE IS NOT A TRIANGLE, %s\n", cur_obj->name);
			exit(1);
		}
		for (int j = 0; j < face_size; j++) {
			cur_vert_pos = fill_vert_with_face(cur_vert_pos, fo_mesh, indices_arr_index + j);
		}
		indices_arr_index += face_size;
	}
	// so now vert is filled as [v1 vn1 vt1 v2 vn2 vt2 ...]
	// think maybe of some material structure (now for every mesh we're making a copy of material struct -> that's stupid)
	fastObjMaterial *material = malloc(sizeof fo_mesh->materials[first_face_material_ind]); 
	memcpy(material, &fo_mesh->materials[first_face_material_ind], sizeof *material);
}

float *fill_vert_with_face(float *cur_vert_pos, fastObjMesh *fo_mesh, unsigned int ind_index) {
	unsigned int p = 3 * fo_mesh->indices[ind_index].p;
	unsigned int n = 3 * fo_mesh->indices[ind_index].n;
	unsigned int t = 2 * fo_mesh->indices[ind_index].t;

	memcpy(cur_vert_pos,     &fo_mesh->positions[p], 3 * sizeof(float));
	memcpy(cur_vert_pos + 3, &fo_mesh->normals[n],   3 * sizeof(float));
	memcpy(cur_vert_pos + 6, &fo_mesh->texcoords[t], 2 * sizeof(float));
	cur_vert_pos += 8;

	return cur_vert_pos;
}

void destroy_mesh(Mesh *mesh) {
	free(mesh->name);
	free(mesh->material);
	free(mesh->vertices);
}

void destroy_model(Model *model) {
	for (int i = 0; i < model->mesh_count; i++) {
		destroy_mesh(&model->meshes[i]);
	}
}

void print_arrf(float* arr, int size) {
	for (int i = 0; i < size; ++i) {
		printf("%f ", arr[i]);
	}
	printf("\n");
}

void print_mat_info(fastObjMaterial *material) {
	printf("Material name: %s", material->name);
	printf("Ambient Ka: "); print_arrf(material->Ka, 3);
	printf("Diffuse Kd: "); print_arrf(material->Kd, 3);
	printf("Specular Ks: "); print_arrf(material->Ks, 3);
	printf("Emmission Ke: "); print_arrf(material->Ke, 3);
	printf("Transmittance Kt: "); print_arrf(material->Kt, 3);
	printf("Shininess Ns: %f\n", material->Ns);
	printf("Refraction index Ni: %f\n", material->Ni);
	printf("Transmition filter Tf: "); print_arrf(material->Tf, 3);
	printf("Dissolve (alpha): %f\n", material->d);
	printf("Illumination model: %d\n", material->illum);
}

void print_group_info(fastObjGroup *group) {
	printf("Group name: %s\n", group->name);
	printf("Num of faces: %d\n", group->face_count);
	printf("Face ffset: %d\n", group->face_offset);
	printf("Index offset: %d\n", group->index_offset);
}

void print_mesh_info(fastObjMesh *mesh, int verbose) {
	
	printf("position_count: %d\n", mesh->position_count);
	printf("texcoord_count: %d\n", mesh->texcoord_count);
	printf("normal_count: %d\n", mesh->normal_count);
	printf("face_count: %d\n", mesh->face_count);
	printf("index_count: %d\n", mesh->index_count);

	printf("material_count: %d\n", mesh->material_count);
	if (verbose) {
	for (int i = 0; i < mesh->material_count; i++) {
		printf("%d.\n", i);
		print_mat_info(&mesh->materials[i]);
	} 
	printf("\n");
	}

	printf("object_count: %d\n", mesh->object_count);
	if (verbose) {
	for (int i = 0; i < mesh->object_count; i++) {
		printf("%d.\n", i);
		print_group_info(&mesh->objects[i]);
	} 
	printf("\n");
	}

	printf("group_count: %d\n", mesh->group_count);
	if (verbose) {
	for (int i = 0; i < mesh->group_count; i++) {
		printf("%d.\n", i);
		print_group_info(&mesh->groups[i]);
	}
	printf("\n");
	}
}

#endif
