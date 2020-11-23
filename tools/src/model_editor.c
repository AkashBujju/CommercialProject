#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "../../src/shader.h"
#include "../../src/utils.h"
#include "../../src/math.h" /* For testing the model */

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

void framebuffer_size_callback(GLFWwindow*, int, int);
void process_input(GLFWwindow*);
GLFWwindow* init_gl_and_window(const char*, uint16_t*, uint16_t*, uint8_t);
void mouse_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);

/* Model Loading */
typedef struct Rectangle2D {
	GLuint vao, vbo, program;
	GLuint texture_id;
	Vector2 position;
	Vector2 dimensions;
} Rectangle2D;

void load_rectangle_2d(Rectangle2D *rectangle_2d, GLuint program, GLuint texture_id, float w, float h);
void translate_rectangle_2d(Rectangle2D *rct, float x, float y);
void draw_rectangle_2d(Rectangle2D *rectangle_2d);
/* Model Loading */

int main() {
	uint16_t window_width, window_height;
	GLFWwindow *window = init_gl_and_window("Model Editor", &window_width, &window_height, 1);

	GLuint program_2d = compile_shader(combine_string(shaders_path, "rect_v_shader.shader"), combine_string(shaders_path, "texture_f_shader.shader"));

	GLuint sample_texture = make_texture(combine_string(assets_path, "png/test_1.png")); 

	/* Init view and projection */
	Matrix4 view, projection;
	init_vector(&front, 0, 0, -1);
	init_vector(&position, 0, 0, 10);
	init_vector(&up, 0, 1, 0);
	make_identity(&view);
	projection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);
	/* Init view and projection */

	/* Ortho Rectangle */
	Rectangle2D rct;
	load_rectangle_2d(&rct, program_2d, sample_texture, 0.4f, 1.94f);
	translate_rectangle_2d(&rct, -0.98f + rct.dimensions.x / 2.0f, 0);
	/* Ortho Rectangle */

	while (!glfwWindowShouldClose(window)) {
		process_input(window);

		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Set the view and projection */
		Vector3 pos_plus_front = add(&position, &front);
		view = look_at(&position, &pos_plus_front, &up);
		/* Set the view and projection */

		draw_rectangle_2d(&rct);

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

GLFWwindow* init_gl_and_window(const char *title, uint16_t *window_width, uint16_t *window_height, uint8_t maximize_window) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_SAMPLES, 1);

	GLFWmonitor *monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode *video_mode = glfwGetVideoMode(monitor);
	*window_width = video_mode->width;
	*window_height = video_mode->height;

	GLFWwindow *window = glfwCreateWindow(*window_width, *window_height, title, monitor, NULL);
	if(maximize_window)
		glfwMaximizeWindow(window);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	/* Mouse */
	last_x = *window_width / 2.0f;
	last_y = *window_height / 2.0f;
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

/* Model Loading */
void load_rectangle_2d(Rectangle2D *rectangle_2d, GLuint program, GLuint texture_id, float w, float h) {
	rectangle_2d->program = program;
	rectangle_2d->texture_id = texture_id;
	rectangle_2d->position.x = 0;
	rectangle_2d->position.y = 0;
	rectangle_2d->dimensions.x = w;
	rectangle_2d->dimensions.y = h;

	float w_by_2 = w / 2.0f;
	float h_by_2 = h / 2.0f;
	float vertices[] = {
		-w_by_2, +h_by_2, 0, 1,
		+w_by_2, +h_by_2, 1, 1,
		-w_by_2, -h_by_2, 0, 0,
		+w_by_2, +h_by_2, 1, 1,
		-w_by_2, -h_by_2, 0, 0,
		+w_by_2, -h_by_2, 1, 0,
	};

	glGenVertexArrays(1, &rectangle_2d->vao);
	glGenBuffers(1, &rectangle_2d->vbo);
	glBindVertexArray(rectangle_2d->vao);
	glBindBuffer(GL_ARRAY_BUFFER, rectangle_2d->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void translate_rectangle_2d(Rectangle2D *rct, float x, float y) {
	rct->position.x = x;
	rct->position.y = y;
}

void draw_rectangle_2d(Rectangle2D *rectangle_2d) {
	glUseProgram(rectangle_2d->program);
	set_vector2(rectangle_2d->program, "pos", &rectangle_2d->position);

	glBindTexture(GL_TEXTURE_2D, rectangle_2d->texture_id);
	glBindVertexArray(rectangle_2d->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
/* Model Loading */
