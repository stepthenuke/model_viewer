#ifndef CAMERA_H
#define CAMERA_H

#include <stdbool.h>

#include <cglm/cglm.h>

#define YAW -90.f
#define PITCH 0.f
#define MOVEMENT_SPEED 10.f
#define SENSITIVITY 0.1f
#define ZOOM 45.f

typedef enum Camera_movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
} Camera_movement;

typedef struct Camera
{
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

#endif // CAMERA_H
