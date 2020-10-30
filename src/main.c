#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "shader.h"
#include "utils.h"
#include "math.h" /* For testing the model */

void framebuffer_size_callback(GLFWwindow*, int, int);
void process_input(GLFWwindow*);
GLFWwindow* init_gl_and_window(const char*, uint16_t, uint16_t);
void mouse_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);

Vector3 front, position, up;
static uint8_t first_mouse = 1;
static float yaw = -90.0f;
static float pitch = -40.0f;
static float last_x, last_y;
static float camera_speed = 0.5f;
static float zoom_speed = 1.0f;
const char* assets_path = "..\\data\\";
const char* shaders_path = "..\\shaders\\";

int main() {
	uint16_t window_width = 800;
	uint16_t window_height = 600;
	GLFWwindow *window = init_gl_and_window("CommercialProject", window_width, window_height);
	uint8_t program = compile_shader(combine_string(shaders_path, "v_shader.shader"), combine_string(shaders_path, "f_shader.shader"));

	/* Init view and projection */
	Matrix4 view, projection;
	init_vector(&front, 0, 0, -1);
	init_vector(&position, 0, 0, 5);
	init_vector(&up, 0, 1, 0);

	make_identity(&view);
	projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);
	/* Init view and projection */

	/* Sample */
	float vertices[108];
	read_floats_from_file(combine_string(assets_path, "vertices/cuboid_vertices.dat"), vertices);

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	Matrix4 model;
	/* Sample */

	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Vector3 pos_plus_front = add(&position, &front);
		view = look_at(&position, &pos_plus_front, &up);
		set_matrix4(program, "view", &view);
		set_matrix4(program, "projection", &projection);

		glUseProgram(program);
		make_identity(&model);
		set_matrix4(program, "model", &model);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);

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

GLFWwindow* init_gl_and_window(const char *title, uint16_t window_width, uint16_t window_height) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 1);

	GLFWwindow *window = glfwCreateWindow(window_width, window_height, title, NULL, NULL);
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

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
