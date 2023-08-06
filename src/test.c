#include "common.h"
#include "camera.h"

void pr(vec3 v) {
	printf("(%f, %f, %f)\n", v[0], v[1], v[2]);
}

int main(void) {
	Camera cam;
	init_camera(&cam);
	pr(cam.position);
	pr(cam.front);
	pr(cam.up);
	pr(cam.right);
	pr(cam.world_up);
	
	mat4 view;
	get_view_matrix(&cam, view);
	printf("View:\n");
	for (int i = 0; i < 4; i++) {
		printf("(");
		for (int j = 0; j < 4; j++) {
			printf("%f ", view[i][j]);
		}
		printf(")\n");
	} 
	return 0;
}
