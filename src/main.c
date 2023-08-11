#include "common.h"

#include "shader.h"
#include "camera.h"
#include "model.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

void process_input(GLFWwindow *window, Camera *camera, float delta_time);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double x_in, double y_in);

Camera camera;

int main(int argc, char **argv)
{
	assert(argc == 2);

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize glfw\n");
		goto cleanup_glfw;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "MODEL VIEWER 2000", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to create window\n");
		goto cleanup_window;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		fprintf(stderr, "Failed to load GLAD\n");
		goto cleanup_window;
	}

	GLuint shader_program = load_program("../src/shader/shader.vert", NULL, "../src/shader/shader.frag");

	Model *model = read_model(argv[1]);

	load_model_gl(model);
	model_info(model, 1);

	init_camera(&camera);
	float current_frame = 0.f, last_frame = 0.f, delta_time = 0.0f;

	glEnable(GL_DEPTH_TEST);

	glUseProgram(shader_program);

	while (!glfwWindowShouldClose(window))
	{
		current_frame = (float)(glfwGetTime());
		delta_time = current_frame - last_frame;
		last_frame = current_frame;
		process_input(window, &camera, delta_time);

		glClearColor(0.1, 0.1, 0.1, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shader_program);
		GLint view_loc = glGetUniformLocation(shader_program, "view");
		mat4 view;
		get_view_matrix(&camera, view);
		glUniformMatrix4fv(view_loc, 1, GL_FALSE, view[0]);

		GLint mod_loc = glGetUniformLocation(shader_program, "model");
		mat4 mod;
		glm_mat4_identity(mod);
		glUniformMatrix4fv(mod_loc, 1, GL_FALSE, mod[0]);

		GLint proj_loc = glGetUniformLocation(shader_program, "projection");
		mat4 proj;
		glm_perspective(0.78f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.f, proj);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, proj[0]);

		draw_model(model, shader_program);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	destroy_model(model);

cleanup_window:
	glfwDestroyWindow(window);

cleanup_glfw:
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window, Camera *camera, float delta_time)
{
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		process_input_camera(camera, FORWARD, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		process_input_camera(camera, BACKWARD, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		process_input_camera(camera, LEFT, delta_time);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		process_input_camera(camera, RIGHT, delta_time);
	}
}

void mouse_callback(GLFWwindow *window, double x_in, double y_in)
{
	float x_pos = (float)x_in;
	float y_pos = (float)y_in;
	if (camera.first_mouse)
	{
		camera.last_x = x_pos;
		camera.last_y = y_pos;
		camera.first_mouse = false;
	}
	float x_offset = x_pos - camera.last_x;
	float y_offset = camera.last_y - y_pos;
	camera.last_x = x_pos;
	camera.last_y = y_pos;
	process_mouse_camera(&camera, x_offset, y_offset);
}
