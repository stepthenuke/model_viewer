#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

#define YAW -90.f
#define PITCH 0.f
#define MOVEMENT_SPEED 2.f
#define SENSITIVITY 0.1f
#define ZOOM 45.f

typedef enum Camera_movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
} Camera_movement;

typedef struct Camera {
	vec3 position;
	vec3 front;
	vec3 up;
	vec3 right;
	vec3 world_up;
	float last_x;
	float last_y;
	float yaw;
	float pitch;
	float movement_speed;
	float sensitivity;
	float zoom;
	bool first_mouse;
} Camera;

void init_camera(Camera *camera);
void get_view_matrix(Camera *camera, mat4 dest);
void process_input_camera(Camera *camera, Camera_movement direction, float delta_time);
void update_camera_state(Camera *camera);
void process_mouse_camera(Camera *camera, float x_offset, float y_offset);

void init_camera(Camera *camera) {
	glm_vec3_one(camera->position);
	glm_vec3_copy(GLM_YUP, camera->world_up);
	camera->yaw =  YAW;
	camera->pitch = PITCH;
	camera->movement_speed = MOVEMENT_SPEED;
	camera->sensitivity = SENSITIVITY;
	camera->zoom = ZOOM;
	camera->first_mouse = true;
	update_camera_state(camera);
}

void get_view_matrix(Camera* camera, mat4 dest) {
	vec3 dir;
	glm_vec3_add(camera->position, camera->front, dir);
	glm_lookat(camera->position, dir, camera->up, dest);
}

void process_input_camera(Camera *camera, Camera_movement direction, float delta_time) {
	float velocity = camera->movement_speed * delta_time;
	vec3 movement;
	if (direction == FORWARD) {
		glm_vec3_scale(camera->front, velocity, movement);
		glm_vec3_add(camera->position, movement, camera->position);
	}
	if (direction == BACKWARD) {
		glm_vec3_scale(camera->front, velocity, movement);
		glm_vec3_sub(camera->position, movement, camera->position);	
	}
	if (direction == LEFT) {
		glm_vec3_scale(camera->right, velocity, movement);
		glm_vec3_sub(camera->position, movement, camera->position);	
	}
	if (direction == RIGHT) {
		glm_vec3_scale(camera->right, velocity, movement);
		glm_vec3_add(camera->position, movement, camera->position);
	}
}

void update_camera_state(Camera *camera) {
	float yaw_rad = camera->yaw;
	float pitch_rad = camera->pitch;
	glm_make_rad(&yaw_rad);
	glm_make_rad(&pitch_rad);
	camera->front[0] = cosf(yaw_rad) * cosf(pitch_rad);
	camera->front[1] = sinf(pitch_rad);
	camera->front[2] = sinf(yaw_rad) * cosf(pitch_rad);
	glm_vec3_normalize(camera->front);
	glm_vec3_crossn(camera->front, camera->world_up, camera->right);
	glm_vec3_crossn(camera->right, camera->front, camera->up);
}

void process_mouse_camera(Camera *camera, float x_offset, float y_offset) {
	x_offset *= camera->sensitivity;
	y_offset *= camera->sensitivity;
	camera->yaw += x_offset;
	camera->pitch += y_offset;
	if (camera->pitch > 89.0f) camera->pitch = 89.0f;
	if (camera->pitch < -89.0f) camera->pitch = -89.0f;
	update_camera_state(camera);
}

#endif
