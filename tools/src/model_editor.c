#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "../../src/shader.h"
#include "../../src/utils.h"
#include "../../src/math.h"
#include "../../src/instanced_model.h"
#include "model_loader_gui.h"
#include "model_properties_gui.h"
#include "helper_models.h"
#include "meta_output.h"

Vector3 front, position, up;
static uint8_t first_mouse = 1;
static float yaw = -90.0f;
static float pitch = -40.0f;
static float last_x, last_y;
static float camera_speed = 0.25f;
static float zoom_speed = 1.0f;
static uint8_t enable_cursor;
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

uint16_t window_width, window_height;
Matrix4 view, projection, text_projection;
GLuint rectangle_program, text_program, instanced_program, dir_light_program, spot_light_program, instanced_helper_program;
GLuint background_left_texture, box_texture, ht_box_texture, button_texture, ht_button_texture, cursor_texture, model_properties_background_texture, close_button_texture, move_tag_texture;

InstancedModel instanced_model;
InstancedHelperModel instanced_helper_model;
Font consolas, georgia_bold_12, georgia_bold_16, georgia_bold_20;
ModelLoaderGUI model_loader_gui;
ModelPropertiesGUI model_properties_gui;

int main() {
	GLFWwindow *window = init_gl_and_window("Model Editor", &window_width, &window_height, 1);
	load();
	enable_cursor = 1;

	load_instanced_model(&instanced_model, instanced_program, "move_stick", combine_string(tmp_models_path, "move_stick.model"));
	add_model(&instanced_model, 0, 0, 0, "pearl");

	load_instanced_helper_model(&instanced_helper_model, instanced_helper_program, combine_string(tmp_models_path, "smooth_cube.model"));
	add_helper_model(&instanced_helper_model, 0, 0, 0);
	set_color_instanced_helper_model(&instanced_helper_model, 0, 1, 0, 0);
	scale_instanced_helper_model(&instanced_helper_model, 0, 0.1f, 0.1f, 0.1f);

	InstancedDirLight instanced_dir_light;
	load_instanced_dir_light(&instanced_dir_light, dir_light_program, combine_string(tmp_models_path, "cube_1.model"), 1);
	translate_instanced_dir_light(&instanced_dir_light, 0, 3, 0, 3);
	scale_instanced_dir_light(&instanced_dir_light, 0, 0.1f, 0.1f, 0.1f);

	InstancedSpotLight instanced_spot_light;
	load_instanced_spot_light(&instanced_spot_light, spot_light_program, combine_string(tmp_models_path, "cube_1.model"), 0);

	/* Setting Lights Attributes once for instanced_program */
	Vector3 light_color, diffuse_color, specular_color, ambient_color;
	init_vector(&light_color, 1, 1, 1);
	init_vector(&specular_color, 1, 1, 1);
	diffuse_color = scalar_mul(&light_color, 0.6f);
	ambient_color = scalar_mul(&diffuse_color, 0.3f);
	glUseProgram(instanced_program);
	set_vector3(instanced_program, "light_ambient", &ambient_color);
	set_vector3(instanced_program, "light_diffuse", &diffuse_color);
	set_vector3(instanced_program, "light_specular", &specular_color);
	set_float(instanced_program, "light_cutOff", cos(to_radians(12.5f)));
	set_float(instanced_program, "light_outerCutOff", cos(to_radians(17.5f)));
	set_float(instanced_program, "light_constant", 1);
	set_float(instanced_program, "light_linear", 0.09f);
	set_float(instanced_program, "light_quadratic", 0.032f);
	/* Setting Lights Attributes once for instanced_program */

	/* Setting Lights Attributes once for instanced_helper_program */
	Vector3 lightColor = { 1, 1, 1 };
	set_vector3(instanced_helper_program, "lightColor", &lightColor);
	/* Setting Lights Attributes once for instanced_helper_program */

	init_model_loader_gui(&model_loader_gui, &georgia_bold_12, &georgia_bold_16, &georgia_bold_20, 0.007f, 55);
	init_model_properties_gui(&model_properties_gui, &georgia_bold_12, &georgia_bold_16, &georgia_bold_20);

	while (!glfwWindowShouldClose(window)) {
		float current_time = glfwGetTime();
		process_input(window);

		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* Set the view and projection */
		Vector3 pos_plus_front = add(&position, &front);
		view = look_at(&position, &pos_plus_front, &up);
		set_matrix4(text_program, "projection", &text_projection);
		set_matrix4(rectangle_program, "projection", &projection);
		set_matrix4(instanced_program, "view", &view);
		set_matrix4(instanced_program, "projection", &projection);
		set_matrix4(instanced_helper_program, "view", &view);
		set_matrix4(instanced_helper_program, "projection", &projection);
		set_matrix4(dir_light_program, "view", &view);
		set_matrix4(dir_light_program, "projection", &projection);
		set_matrix4(spot_light_program, "view", &view);
		set_matrix4(spot_light_program, "projection", &projection);
		/* Set the view and projection */

		/* Setting viewPos */
		glUseProgram(instanced_program);
		set_vector3(instanced_program, "viewPos", &position);
		glUseProgram(instanced_helper_program);
		set_vector3(instanced_helper_program, "viewPos", &position);
		/* Setting viewPos */

		draw_instanced_model(&instanced_model, &instanced_dir_light, &instanced_spot_light);
		draw_instanced_dir_light(&instanced_dir_light);
		draw_instanced_spot_light(&instanced_spot_light);

		draw_instanced_helper_model(&instanced_helper_model, &instanced_dir_light);

		handle_transition_gui(&model_loader_gui, 1, 0);
		draw_model_loader_gui(&model_loader_gui, current_time);

		/* @Note: Change this in the future */
		double x_pos, y_pos;
		glfwGetCursorPos(window, &x_pos, &y_pos);
		Vector2 norm_mouse_pos;
		norm_mouse_pos.x = f_normalize(x_pos, 0, window_width, -1, +1);
		norm_mouse_pos.y = f_normalize(y_pos, 0, window_height, +1, -1);
		translate_move_tag_and_update_properties_gui(&model_properties_gui, norm_mouse_pos.x, norm_mouse_pos.y);
		draw_model_properties_gui(&model_properties_gui, current_time);

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
		handle_key_input_gui(&model_loader_gui, key);
	if(key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		model_loader_gui.trn.activate = 1;
		enable_cursor = !enable_cursor;

		if(enable_cursor)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
	handle_textinput_gui(&model_loader_gui, (char)codepoint);
}

void process_input(GLFWwindow *window) {
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, 1);
	}
	if(enable_cursor)
		return;

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
	glfwWindowHint(GLFW_SAMPLES, 1); /* @Note: Is this even having any effect */

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

		Vector3 norm = normalize_to(x_pos, y_pos, window_width, window_height);
		Vector ray = compute_mouse_ray_2(norm.x, norm.y, &view, &projection);

		/* Test */
		// scale_instanced_model_along(&instanced_model, 0, &ray, 1, 0, 0);
		/* Test */
		
		uint8_t hit = obb(&instanced_model.models[0], &instanced_model.positions[0], instanced_model.bounding_boxes[0].width, instanced_model.bounding_boxes[0].height, instanced_model.bounding_boxes[0].depth, &ray);

		if(hit) {
			model_properties_gui.show = 1;
			set_instanced_model_info_to_properties_gui(&model_properties_gui, &instanced_model, 0);
		}

		handle_mouse_click_gui(&model_loader_gui, &norm_mouse_pos);
		handle_mouse_click_properties_gui(&model_properties_gui, &norm_mouse_pos);
		handle_mouse_movement_properties_gui(&model_properties_gui, &norm_mouse_pos);

		if(model_properties_gui.close_button.clicked) {
			model_properties_gui.show = 0;
		}

		/* @Note: Is this the we to handle this */
		// if(model_loader_gui.model_load_button.clicked) {
		// 	char* text = model_loader_gui.model_load_textbox.text.text;

		// 	if(strcmp(text, "person_try") == 0)
		// 		add_model(&instanced_model, 0, 0, 0, "pearl");
		// }
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

	if(enable_cursor)
		return;

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
