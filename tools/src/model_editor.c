#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "../../src/shader.h"
#include "../../src/utils.h"
#include "../../src/math.h"
/*
#include "text.h"
#include "rectangle_2d.h"
#include "text_box.h"
#include "button.h"
*/

#include "gui.h"
#include "meta_output.h"

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
void mouse_button_callback(GLFWwindow*, int, int, int);
void key_callback(GLFWwindow*, int, int, int, int);
void character_callback(GLFWwindow*, unsigned int);

void check_and_hide_leftside_gui();

uint16_t window_width, window_height;
Matrix4 view, projection, text_projection;

GLuint rectangle_program, text_program;
GLuint background_left_texture, box_texture, ht_box_texture, button_texture, ht_button_texture, cursor_texture;

Font consolas, georgia_bold;
EditorGUI editor_gui;

int main() {
	GLFWwindow *window = init_gl_and_window("Model Editor", &window_width, &window_height, 1);
	load();

	init_editor_gui(&editor_gui, &georgia_bold, 0.007f, 55);

	while (!glfwWindowShouldClose(window)) {
		float time = glfwGetTime();
		process_input(window);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Set the view and projection */
		Vector3 pos_plus_front = add(&position, &front);
		view = look_at(&position, &pos_plus_front, &up);
		set_matrix4(text_program, "projection", &text_projection);
		set_matrix4(rectangle_program, "projection", &projection);
		/* Set the view and projection */

		handle_transition_gui(&editor_gui, 1, 0);
		draw_editor_gui(&editor_gui, time);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	printf("Ended successfully.\n\n");
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action == GLFW_PRESS)
		handle_key_input_gui(&editor_gui, key);
	 if(key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		 editor_gui.trn.activate = 1;
	 }
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
	handle_textinput_gui(&editor_gui, (char)codepoint);
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
	glfwWindowHint(GLFW_SAMPLES, 16); /* @Note: Is this even having any effect */

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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); /* @Note: Change this whenever necessary */
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	/* Mouse */

	glfwSetKeyCallback(window, key_callback);
	glfwSetCharCallback(window, character_callback);	

	/* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(1);

	return window;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double x_pos, y_pos;
		glfwGetCursorPos(window, &x_pos, &y_pos);

		/* Normalizing */
		Vector2 norm_mouse_pos;
		norm_mouse_pos.x = f_normalize(x_pos, 0, window_width, -1, +1);
		norm_mouse_pos.y = f_normalize(y_pos, 0, window_height, +1, -1);
		/* Normalizing */

		handle_mouse_click_gui(&editor_gui, &norm_mouse_pos, button);
		// handle_textbox_click(&text_box, &norm_mouse_pos);
		// uint8_t load_button_clicked = button_clicked(&load_model_button, &norm_mouse_pos);
	}
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
