#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "../../src/shader.h"
#include "../../src/utils.h"
#include "../../src/math.h" /* For testing the model */
#include "../../src/instanced_model.h"
#include "../../src/light.h"

void framebuffer_size_callback(GLFWwindow*, int, int);
void process_input(GLFWwindow*);
GLFWwindow* init_gl_and_window(const char*, uint16_t, uint16_t, uint8_t);
void mouse_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);

Vector3 front, position, up;
static uint8_t first_mouse = 1;
static float yaw = -90.0f;
static float pitch = -40.0f;
static float last_x, last_y;
static float camera_speed = 0.5f;
static float zoom_speed = 1.0f;
const char* assets_path = "../../data/";
const char* shaders_path = "../../shaders/";
const char* tmp_models_path = "../data_out/";

int main() {
	uint16_t window_width = 1024;
	uint16_t window_height = 768;
	GLFWwindow *window = init_gl_and_window("CommercialProject", window_width, window_height, 0);

	GLuint program = compile_shader(combine_string(shaders_path, "v_shader.shader"),
			                          combine_string(shaders_path, "f_shader.shader"));
	GLuint instanced_program = compile_shader(combine_string(shaders_path, "instanced_v_shader.shader"),
			                                   combine_string(shaders_path, "instanced_f_shader.shader"));
	GLuint light_program = compile_shader(combine_string(shaders_path, "v_light.shader"),
			                                combine_string(shaders_path, "f_light.shader"));

	/* Init view and projection */
	Matrix4 view, projection;
	init_vector(&front, 0, 0, -1);
	init_vector(&position, 0, 0, 10);
	init_vector(&up, 0, 1, 0);
	make_identity(&view);
	projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);
	/* Init view and projection */

	/* Instanced Model */
	InstancedModel im;
	load_instanced_model(&im, instanced_program, combine_string(tmp_models_path, "torus.model"), 9);

	Vector3 current_pos;
	Vector3 current_color;
	init_vector(&current_pos, -5, +5, 0);
	init_vector(&current_color, 0.1f, 0.1f, 0.1f);
	for(uint16_t i = 1; i <= 9; ++i) {
		if((i - 1) != 0 && (i - 1) % 3 == 0) {
			current_pos.y -= 5;
			current_pos.x = -5;
		}

		translate_instanced_model(&im, i - 1, current_pos.x, current_pos.y, current_pos.z);
		rotate_instanced_model(&im, i - 1, 1, 0, 0, 90);
		current_pos.x += 5;
		current_color.x += 0.01f;
		current_color.y += 0.04f;
		current_color.z += 0.03f;
	}

	set_material_instanced_model(&im, 0, "ruby");
	set_material_instanced_model(&im, 1, "gold");
	set_material_instanced_model(&im, 2, "silver");
	set_material_instanced_model(&im, 3, "jade");
	set_material_instanced_model(&im, 4, "black_rubber");
	set_material_instanced_model(&im, 5, "red_rubber");
	set_material_instanced_model(&im, 6, "yellow_rubber");
	set_material_instanced_model(&im, 7, "copper");
	set_material_instanced_model(&im, 8, "bronze");

	Light l1;
	load_light(&l1, light_program, combine_string(tmp_models_path, "light_cube.model"));
	translate_light(&l1, 0, 0, 5);
	scale_light(&l1, 0.1f, 0.1f, 0.1f);
	/* Instanced Model */

	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Set the view and projection */
		Vector3 pos_plus_front = add(&position, &front);
		view = look_at(&position, &pos_plus_front, &up);
		set_matrix4(program, "view", &view);
		set_matrix4(program, "projection", &projection);
		set_matrix4(instanced_program, "view", &view);
		set_matrix4(instanced_program, "projection", &projection);
		set_matrix4(light_program, "view", &view);
		set_matrix4(light_program, "projection", &projection);
		/* Set the view and projection */

		/* Moving the light */
		const float radius = 5.0f;
		float light_x = sin(glfwGetTime()) * radius;
		float light_z = cos(glfwGetTime()) * radius;
		translate_light(&l1, light_x, 0, light_z);
		/* Moving the light */

		/* Instanced Model */
		glUseProgram(instanced_program);
		Vector3 light_color, diffuse_color, specular_color, ambient_color;
		init_vector(&light_color, 1, 1, 1);
		init_vector(&specular_color, 1, 1, 1);
		diffuse_color = scalar_mul(&light_color, 0.5f);
		ambient_color = scalar_mul(&diffuse_color, 0.2f);
		set_vector3(instanced_program, "light.ambient", &ambient_color);
		set_vector3(instanced_program, "light.diffuse", &diffuse_color);
		set_vector3(instanced_program, "light.specular", &specular_color);
		set_vector3(instanced_program, "light.position", &l1.position);
		set_vector3(instanced_program, "viewPos", &position);
		draw_instanced_model(&im);
		/* Instanced Model */

		/* Light */
		draw_light(&l1);
		/* Light */

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Vector3 res = cross(&front, &up);
		normalize_vector(&res);

		position.x -= camera_speed * res.x;
		position.y -= camera_speed * res.y;
		position.z -= camera_speed * res.z;
	}
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		Vector3 res = cross(&front, &up);
		normalize_vector(&res);

		position.x += camera_speed * res.x;
		position.y += camera_speed * res.y;
		position.z += camera_speed * res.z;
	}
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		Vector3 _1 = cross(&front, &up);
		Vector3 res = cross(&_1, &up);
		position.x -= camera_speed * res.x;
		position.y -= camera_speed * res.y;
		position.z -= camera_speed * res.z;
	}
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Vector3 _1 = cross(&front, &up);
		Vector3 res = cross(&_1, &up);
		position.x += camera_speed * res.x;
		position.y += camera_speed * res.y;
		position.z += camera_speed * res.z;
	}
}

GLFWwindow* init_gl_and_window(const char *title, uint16_t window_width, uint16_t window_height, uint8_t maximize_window) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_SAMPLES, 1);

	GLFWwindow *window = glfwCreateWindow(window_width, window_height, title, NULL, NULL);
	if(maximize_window)
		glfwMaximizeWindow(window);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	/* Mouse */
	last_x = window_width / 2.0f;
	last_y = window_height / 2.0f;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	/* Mouse */

	/* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
	glEnable(GL_DEPTH_TEST);
	glfwSwapInterval(1);

	return window;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (first_mouse) {
		last_x = xpos;
		last_y = ypos;
		first_mouse = 0;
	}

	float xoffset = xpos - last_x;
	float yoffset = last_y - ypos;
	last_x = xpos;
	last_y = ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	front.x = cos(to_radians(yaw)) * cos(to_radians(pitch));
	front.y = sin(to_radians(pitch));
	front.z = sin(to_radians(yaw)) * cos(to_radians(pitch));
	normalize_vector(&front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	position.x += (zoom_speed * front.x) * yoffset;
	position.y += (zoom_speed * front.y) * yoffset;
	position.z += (zoom_speed * front.z) * yoffset;
}
