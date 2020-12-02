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
#include "tmp_bone.h"

void framebuffer_size_callback(GLFWwindow*, int, int);
void process_input(GLFWwindow*);
GLFWwindow* init_gl_and_window(const char*, uint16_t, uint16_t, uint8_t);
void mouse_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);

uint16_t window_width = 1024;
uint16_t window_height = 768;

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
	GLFWwindow *window = init_gl_and_window("CommercialProject", window_width, window_height, 0);

	GLuint program = compile_shader(combine_string(shaders_path, "v_shader.shader"), combine_string(shaders_path, "f_shader.shader"));
	GLuint instanced_program = compile_shader(combine_string(shaders_path, "instanced_v_shader.shader"), combine_string(shaders_path, "instanced_f_shader.shader"));
	GLuint bone_program = compile_shader(combine_string(shaders_path, "bone_v_shader.shader"), combine_string(shaders_path, "bone_f_shader.shader"));
	GLuint light_program = compile_shader(combine_string(shaders_path, "v_light.shader"), combine_string(shaders_path, "f_light.shader"));

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
	load_instanced_model(&im, instanced_program, combine_string(tmp_models_path, "person_try.model"), 2);
	translate_instanced_model(&im, 0, -2, 0, 0);
	translate_instanced_model(&im, 1, +2, 0, 0);
	set_material_instanced_model(&im, 0, "pearl");
	set_material_instanced_model(&im, 1, "jade");

	/* Lights */
	Light lights[2];
	load_light(&lights[0], light_program, combine_string(tmp_models_path, "light_cube.model"));
	load_light(&lights[1], light_program, combine_string(tmp_models_path, "light_cube.model"));
	translate_light(&lights[0], 5, 2, 3);
	translate_light(&lights[1], -5, 2, 3);
	scale_light(&lights[0], 0.1f, 0.1f, 0.1f);
	scale_light(&lights[1], 0.1f, 0.1f, 0.1f);
	/* Lights */

	/* Instanced Model */

	/* Bones */
	Mesh mesh;
	// add_vertex_to_mesh(&mesh, 0, 0, 0, -5.2f, 0, 0, 0, 1, 0);
	// add_vertex_to_mesh(&mesh, 0, 1, 0, -3.2f, 0, 0, 1, 0.5f, 0.5f);
	// add_vertex_to_mesh(&mesh, 1, 0, 0, -3.1f, 0, 0, 1, 0.5f, 0.5f);
	// add_vertex_to_mesh(&mesh, 1, 1, 0, -1.1f, 0, 1, 2, 0.5f, 0.5f);
	// add_vertex_to_mesh(&mesh, 2, 0, 0, -1.0f, 0, 1, 2, 0.5f, 0.5f);
	// add_vertex_to_mesh(&mesh, 2, 1, 0, +1.0f, 0, 2, 3, 0.5f, 0.5f);
	// add_vertex_to_mesh(&mesh, 3, 0, 0, +1.1f, 0, 2, 3, 0.5f, 0.5f);
	// add_vertex_to_mesh(&mesh, 3, 1, 0, +3.1f, 0, 3, 4, 0.5f, 0.5f);
	// add_vertex_to_mesh(&mesh, 4, 0, 0, +3.2f, 0, 3, 4, 0.5f, 0.5f);
	// add_vertex_to_mesh(&mesh, 4, 1, 0, +5.2f, 0, 4, 0, 1, 0);
	// compile_mesh(&mesh, bone_program, 6);
	/* Bones */

	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
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
		const float radius = 2.0f;
		float light_x = sin(glfwGetTime()) * radius;
		float light_z = cos(glfwGetTime()) * radius;
		translate_light(&lights[0], light_x, 3, -light_z);
		translate_light(&lights[1], -light_x, 3, light_z);
		/* Moving the light */

		/* Instanced Model */
		glUseProgram(instanced_program);
		Vector3 light_color, diffuse_color, specular_color, ambient_color;
		init_vector(&light_color, 1, 1, 1);
		init_vector(&specular_color, 1, 1, 1);
		diffuse_color = scalar_mul(&light_color, 0.6f);
		ambient_color = scalar_mul(&diffuse_color, 0.3f);
		set_vector3(instanced_program, "dir_lights[0].ambient", &ambient_color);
		set_vector3(instanced_program, "dir_lights[0].diffuse", &diffuse_color);
		set_vector3(instanced_program, "dir_lights[0].specular", &specular_color);
		set_vector3(instanced_program, "dir_lights[0].position", &lights[0].position);
		set_vector3(instanced_program, "dir_lights[1].ambient", &ambient_color);
		set_vector3(instanced_program, "dir_lights[1].diffuse", &diffuse_color);
		set_vector3(instanced_program, "dir_lights[1].specular", &specular_color);
		set_vector3(instanced_program, "dir_lights[1].position", &lights[1].position);
		set_vector3(instanced_program, "viewPos", &position);
		draw_instanced_model(&im);
		/* Instanced Model */

		/* Light */
		draw_light(&lights[0]);
		draw_light(&lights[1]);
		/* Light */

		/* Bones */
		// const float degree = 90;
		// float d = sin(glfwGetTime()) * degree;
		// rotate_bone_in_mesh(&mesh, 0, 1, 0, 0, d);
		// rotate_bone_in_mesh(&mesh, 1, 0, 0, 1, d * 0.5f);
		// rotate_bone_in_mesh(&mesh, 2, 0, 0, 1, d * 0.25f);
		// set_matrix4(bone_program, "view", &view);
		// set_matrix4(bone_program, "projection", &projection);
		// draw_mesh(&mesh);
		/* Bones */

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
