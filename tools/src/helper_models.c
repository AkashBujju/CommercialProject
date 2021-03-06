#include "helper_models.h"
#include "../../src/utils.h"
#include "../../src/shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "exp_lines.h" /* TMP */

extern char* assets_path;
extern char* tmp_models_path;
extern ExpLines exp_lines; /* TMP */

void load_instanced_helper_model(InstancedHelperModel *instanced_helper_model, GLuint program, const char* model_filename) {
	instanced_helper_model->program = program;
	instanced_helper_model->num_models = 0;

	FILE *file = fopen(model_filename, "r");
	if(!file) {
		printf("ERROR: File %s not found\n", model_filename);
		printf("ERROR: Not created model!!!\n");
	}
	else {
		/* @TODO: Come up with a better way to read the file */
		/* Reading num_floats and sub_model_count from file */
		uint32_t num_floats;
		uint16_t sub_model_count;
		char tmp[50];
		fscanf(file, "%s", tmp); /* sub_model_count string */
		fscanf(file, "%hu", &sub_model_count);
		fscanf(file, "%s", tmp); /* num_floats string */
		fscanf(file, "%u", &num_floats);
		fclose(file);
		/* Reading num_floats and sub_model_count from file */

		float *vertices = (float*)malloc(sizeof(float) * num_floats);
		Vector3 extreme_min, extreme_max;
		get_model_data_from_file(model_filename, &extreme_min, &extreme_max, vertices);

		float model_width = extreme_max.x - extreme_min.x;
		float model_height = extreme_max.y - extreme_min.y;
		float model_depth = extreme_max.z - extreme_min.z;

		/* @Note: Change this if normals and other info are added */
		/* 3 for the co-ordinates and 3 for the normals := 6 */
		instanced_helper_model->num_vertices = num_floats / 6;

		for(uint32_t i = 0; i < MAX_INSTANCED_HELPER_MODELS; ++i) {
		 	/* @Note: Change the below to custom values later */
			init_vector(&instanced_helper_model->_positions[i], 0, 0, 0);
		 	init_vector(&instanced_helper_model->initial_positions[i], 0, 0, 0);
		 	init_vector(&instanced_helper_model->scales[i], 1, 1, 1);
		 	init_vector4(&instanced_helper_model->colors[i], 0.5f, 0.5f, 0.5f, 1.0f);
			instanced_helper_model->bounding_boxes[i].width = model_width;
			instanced_helper_model->bounding_boxes[i].height = model_height;
			instanced_helper_model->bounding_boxes[i].depth = model_depth;
			make_identity(&instanced_helper_model->models[i]);
			/* @Note: Change the width, height and depth to be the bounding of the model */
		 	instanced_helper_model->angle_in_degree[i] = 0;
		 	instanced_helper_model->initial_angle_in_degree[i] = 0;
		 	init_vector(&instanced_helper_model->rotation_axes[i], 0, 0, 0);
		 	init_vector(&instanced_helper_model->initial_rotation_axes[i], 0, 0, 0);
		}

		/* Instance VBOs */
		glGenBuffers(1, &instanced_helper_model->instanceModelVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4) * MAX_INSTANCED_HELPER_MODELS, instanced_helper_model->models, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_helper_model->instanceColorVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceColorVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector4) * MAX_INSTANCED_HELPER_MODELS, instanced_helper_model->colors, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
		/* Instance VBOs */

		/* Position */
		glGenVertexArrays(1, &instanced_helper_model->vao);
		glGenBuffers(1, &instanced_helper_model->vbo);
		glBindVertexArray(instanced_helper_model->vao);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->vbo);
		/* @Note: Should we change this to dynamic draw later? */
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_floats, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		/* Position */

		/* Instanced Model Attrib */
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
    	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
    	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(4 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
    	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
    	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(5, 1);
		/* Instanced Model Attrib */

		/* Instanced Colors */
		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceColorVBO);
    	glVertexAttribPointer(6, 4, GL_FLOAT, GL_TRUE, 4 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(6, 1);
		/* Instanced Colors */
	}
}

void set_initial_pose_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float px, float py, float pz, float axis_x, float axis_y, float axis_z, float angle) {
	if(model_index >= instanced_helper_model->num_models) {
		printf("WARNING: In set_initial_pose_instanced_helper_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_helper_model->num_models);
	}
	else {
		init_vector(&instanced_helper_model->initial_positions[model_index], px, py, pz);
		init_vector(&instanced_helper_model->initial_rotation_axes[model_index], axis_x, axis_y, axis_z);
		instanced_helper_model->initial_angle_in_degree[model_index] = angle;
	}
}

void add_helper_model(InstancedHelperModel *instanced_helper_model, float x, float y, float z) {
	if(instanced_helper_model->num_models >= MAX_INSTANCED_HELPER_MODELS) {
		printf("WARNING: num_helper_models(%d) >= MAX_INSTANCED_HELPER_MODELS(%d)\n", instanced_helper_model->num_models, MAX_INSTANCED_HELPER_MODELS);
		printf("WARNING: Setting num_models to %d\n", MAX_INSTANCED_HELPER_MODELS);
	}
	else {
		instanced_helper_model->num_models += 1;
		translate_instanced_helper_model(instanced_helper_model, instanced_helper_model->num_models - 1, x, y, z);
	}
}

void set_color_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float r, float g, float b, float a) {
	if(model_index >= instanced_helper_model->num_models) {
		printf("WARNING: In set_color_instanced_helper_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_helper_model->num_models);
	}
	else {
		instanced_helper_model->colors[model_index].x = r;
		instanced_helper_model->colors[model_index].y = g;
		instanced_helper_model->colors[model_index].z = b;
		instanced_helper_model->colors[model_index].a = a;

		glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceColorVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vector4) * instanced_helper_model->num_models, instanced_helper_model->colors);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void translate_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float x, float y, float z) {
	if(model_index >= instanced_helper_model->num_models) {
		printf("WARNING: In translate_instanced_helper_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_helper_model->num_models);
	}
	else {
		init_vector(&instanced_helper_model->_positions[model_index], x, y, z);
	}
}

void scale_instanced_helper_model(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float x, float y, float z) {
	if(model_index >= instanced_helper_model->num_models) {
		printf("WARNING: In scale_instanced_helper_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_helper_model->num_models);
	}
	else {
		init_vector(&instanced_helper_model->scales[model_index], x, y, z);
		instanced_helper_model->bounding_boxes[model_index].width *= x;
		instanced_helper_model->bounding_boxes[model_index].height *= y;
		instanced_helper_model->bounding_boxes[model_index].depth *= z;
	}
}

void rotate_instanced_helper_model_about(InstancedHelperModel *instanced_helper_model, uint32_t model_index, float axis_x, float axis_y, float axis_z, float about_x, float about_y, float about_z, float degree) {
	if(model_index >= instanced_helper_model->num_models) {
		printf("WARNING: In rotate_instanced_helper_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_helper_model->num_models);
	}
	else {
		instanced_helper_model->angle_in_degree[model_index] = degree;
		init_vector(&instanced_helper_model->rotation_points[model_index], about_x, about_y, about_z);
		instanced_helper_model->rotation_axes[model_index].x = axis_x;
		instanced_helper_model->rotation_axes[model_index].y = axis_y;
		instanced_helper_model->rotation_axes[model_index].z = axis_z;
	}
}

static void make_glsl_string(char *des, uint16_t index, char *var_name, char *member_name) {
	strcpy(des, var_name);
	strcat(des, "[");

	char index_str[15];
	sprintf(index_str, "%u", index);

	strcat(des, index_str);
	strcat(des, "].");
	strcat(des, member_name);
}

void draw_instanced_helper_model(InstancedHelperModel *instanced_helper_model, InstancedDirLight *instanced_dir_light) {
	glUseProgram(instanced_helper_model->program);

	/* Updating in-struct Model Matrix */
	for(uint32_t i = 0; i < instanced_helper_model->num_models; ++i) {
		make_identity(&instanced_helper_model->models[i]);

		/* Setting initial pose */
		translate_matrix(&instanced_helper_model->models[i], 0, 0, 0);
		rotate(&instanced_helper_model->models[i], &instanced_helper_model->initial_rotation_axes[i], instanced_helper_model->initial_angle_in_degree[i]);
		translate_matrix(&instanced_helper_model->models[i], instanced_helper_model->initial_positions[i].x, instanced_helper_model->initial_positions[i].y, instanced_helper_model->initial_positions[i].z);
		/* Setting initial pose */

		translate_matrix(&instanced_helper_model->models[i], instanced_helper_model->_positions[i].x, instanced_helper_model->_positions[i].y, instanced_helper_model->_positions[i].z);
		scale(&instanced_helper_model->models[i], instanced_helper_model->scales[i].x, instanced_helper_model->scales[i].y, instanced_helper_model->scales[i].z);

		/* Rotation about a point */
		Vector3 *about_point = &instanced_helper_model->rotation_points[i];
		Vector3 disp = { instanced_helper_model->_positions[i].x - about_point->x, instanced_helper_model->_positions[i].y - about_point->y, instanced_helper_model->_positions[i].z - about_point->z };
		translateBy_matrix(&instanced_helper_model->models[i], -disp.x, -disp.y, -disp.z);
		rotate(&instanced_helper_model->models[i], &instanced_helper_model->rotation_axes[i], instanced_helper_model->angle_in_degree[i]);
		translateBy_matrix(&instanced_helper_model->models[i], +disp.x, +disp.y, +disp.z);
		/* Rotation about a point */
	}
	/* Updating in-struct Model Matrix */

	/* Updating ModelVBO */
	glBindBuffer(GL_ARRAY_BUFFER, instanced_helper_model->instanceModelVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Matrix4) * instanced_helper_model->num_models, instanced_helper_model->models);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/* Updating ModelVBO */

	/* Directional Lights */
	set_integer(instanced_helper_model->program, "num_dir_lights", instanced_dir_light->num_models);
	for(uint32_t di = 0; di < instanced_dir_light->num_models; ++di) {
		char var_name[50];
		make_glsl_string(var_name, di, "dir_lights", "position");
		set_vector3(instanced_helper_model->program, var_name, &instanced_dir_light->positions[di]);
	}
	/* Directional Lights */

	glBindVertexArray(instanced_helper_model->vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, instanced_helper_model->num_vertices, instanced_helper_model->num_models);
}

void init_helper_models(HelperModels *helper_models, GLuint instanced_helper_program) {
	helper_models->active_helper_model_index = -1;

	/* Origin */
	load_instanced_helper_model(&helper_models->cubes, instanced_helper_program, combine_string(tmp_models_path, "smooth_cube.model"));
	add_helper_model(&helper_models->cubes, 0, 0, 0);
	set_color_instanced_helper_model(&helper_models->cubes, 0, 1, 0, 0, 1);
	scale_instanced_helper_model(&helper_models->cubes, 0, 0.1f, 0.1f, 0.1f);
	/* Origin */

	/* Axes */
	// X
	add_helper_model(&helper_models->cubes, 0, 0, 0);
	set_color_instanced_helper_model(&helper_models->cubes, 1, 0, 0, 1, 1);
	scale_instanced_helper_model(&helper_models->cubes, 1, 20, 0.015f, 0.015f);

	// Y
	add_helper_model(&helper_models->cubes, 0, 0, 0);
	set_color_instanced_helper_model(&helper_models->cubes, 2, 1, 0, 0, 1);
	scale_instanced_helper_model(&helper_models->cubes, 2, 20, 0.015f, 0.015f);
	rotate_instanced_helper_model_about(&helper_models->cubes, 2, 0, 0, 1, 0, 0, 0, 90);

	// Z
	add_helper_model(&helper_models->cubes, 0, 0, 0);
	set_color_instanced_helper_model(&helper_models->cubes, 3, 0, 1, 0, 1);
	scale_instanced_helper_model(&helper_models->cubes, 3, 20, 0.015f, 0.015f);
	rotate_instanced_helper_model_about(&helper_models->cubes, 3, 0, 1, 0, 0, 0, 0, 90);
	/* Axes */

	/* Move sticks */
	// X
	add_helper_model(&helper_models->cubes, 0, 0, 0);
	set_color_instanced_helper_model(&helper_models->cubes, 4, 0, 0, 1, 1);
	scale_instanced_helper_model(&helper_models->cubes, 4, 0.3f, 0.05f, 0.05f);

	// Y
	add_helper_model(&helper_models->cubes, 0, 0, 0);
	set_color_instanced_helper_model(&helper_models->cubes, 5, 1, 0, 0, 1);
	scale_instanced_helper_model(&helper_models->cubes, 5, 0.3f, 0.05f, 0.05f);
	set_initial_pose_instanced_helper_model(&helper_models->cubes, 5, 0, 0, 0, 0, 0, 1, 90);

	// Z
	add_helper_model(&helper_models->cubes, 0, 0, 0);
	set_color_instanced_helper_model(&helper_models->cubes, 6, 0, 1, 0, 1);
	scale_instanced_helper_model(&helper_models->cubes, 6, 0.3f, 0.05f, 0.05f);
	set_initial_pose_instanced_helper_model(&helper_models->cubes, 6, 0, 0, 0, 0, 1, 0, 90);
	/* Move sticks */

	/* Setting default colors */
	init_vector(&helper_models->default_colors[0], 1, 0, 0);
	init_vector(&helper_models->default_colors[1], 0, 0, 1);
	init_vector(&helper_models->default_colors[2], 1, 0, 0);
	init_vector(&helper_models->default_colors[3], 0, 1, 0);
	init_vector(&helper_models->default_colors[4], 0, 0, 1);
	init_vector(&helper_models->default_colors[5], 1, 0, 0);
	init_vector(&helper_models->default_colors[6], 0, 1, 0);
	/* Setting default colors */
}

void set_move_sticks(HelperModels *helper_models, InstancedModel *instanced_model, uint32_t model_index) {
	helper_models->active_instanced_model_index = model_index;

	Vector3 model_pos = { instanced_model->positions[model_index].x, instanced_model->positions[model_index].y, instanced_model->positions[model_index].z };
	float move_stick_x_pos = model_pos.x + instanced_model->bounding_boxes[model_index].width / 2.0f + 0.8f;
	float move_stick_y_pos = model_pos.y + instanced_model->bounding_boxes[model_index].height / 2.0f + 0.8f;
	float move_stick_z_pos = model_pos.z + instanced_model->bounding_boxes[model_index].depth / 2.0f + 0.8f;

	translate_instanced_helper_model(&helper_models->cubes, 4, move_stick_x_pos, model_pos.y, model_pos.z);
	translate_instanced_helper_model(&helper_models->cubes, 5, model_pos.x, move_stick_y_pos, model_pos.z);
	translate_instanced_helper_model(&helper_models->cubes, 6, model_pos.x, model_pos.y, move_stick_z_pos);
}

void move_helper_models_along(HelperModels *helper_models, uint32_t model_index, Vector *ray, uint8_t along_x, uint8_t along_y, uint8_t along_z) {
	if(model_index >= helper_models->cubes.num_models) {
		printf("WARNING: In move_helper_models_along(): model_index(%u) >= num_models(%u)\n", model_index, helper_models->cubes.num_models);
	}
	else {
		Vector3 pos;
		Vector3 model_position = helper_models->cubes._positions[model_index];
		uint8_t valid = get_position_along_axis(&model_position, &pos, ray, along_x, along_y, along_z);

		if(valid) {
			if(along_x) {
				init_vector(&pos, pos.x, model_position.y, model_position.z);
			}
			else if(along_y) {
				init_vector(&pos, model_position.x, pos.y, model_position.z);
			}
			else if(along_z) {
				init_vector(&pos, model_position.x, model_position.y, pos.z);
			}
	
			translate_instanced_helper_model(&helper_models->cubes, model_index, pos.x, pos.y, pos.z);
		}
	}
}

static void scale_instanced_model_using_movesticks(HelperModels *helper_models, InstancedModel *instanced_model, Vector *ray) {
	if(helper_models->active_helper_model_index != -1) {
		if(helper_models->active_helper_model_index == 4)
			move_helper_models_along(helper_models, helper_models->active_helper_model_index, ray, 1, 0, 0);
		else if(helper_models->active_helper_model_index == 5)
			move_helper_models_along(helper_models, helper_models->active_helper_model_index, ray, 0, 1, 0);
		else if(helper_models->active_helper_model_index == 6)
			move_helper_models_along(helper_models, helper_models->active_helper_model_index, ray, 0, 0, 1);

		BoundingBox *hb = &helper_models->cubes.bounding_boxes[helper_models->active_helper_model_index];
		Vector3 *ip = &instanced_model->positions[helper_models->active_instanced_model_index];
		Vector3 hp = helper_models->cubes._positions[helper_models->active_helper_model_index];

		/* @Note: Here we are taking hb->width for all three dimensions because in all three movesticks only the width has been
		 * scaled and the model rotated to y and z axes */
		Vector3 new_pos;
		if(helper_models->active_helper_model_index == 4)
			init_vector(&new_pos, hp.x - hb->width / 2.0f - 0.2f, ip->y, ip->z);
		else if(helper_models->active_helper_model_index == 5)
			init_vector(&new_pos, ip->x, hp.y - hb->width / 2.0f - 0.2f, ip->z);
		else if(helper_models->active_helper_model_index == 6)
			init_vector(&new_pos, ip->x, ip->y, hp.z - hb->width / 2.0f - 0.2f);

		Vector3 new_scale = { 0, 0, 0 };
		if(helper_models->active_helper_model_index == 4) {
			new_scale.x = (2 * fabs(new_pos.x - instanced_model->positions[helper_models->active_instanced_model_index].x)) / instanced_model->default_boxes[helper_models->active_instanced_model_index].width;
		}
		else if(helper_models->active_helper_model_index == 5) {
			new_scale.y = (2 * fabs(new_pos.y - instanced_model->positions[helper_models->active_instanced_model_index].y)) / instanced_model->default_boxes[helper_models->active_instanced_model_index].height;
		}
		else if(helper_models->active_helper_model_index == 6) {
			new_scale.z = (2 * fabs(new_pos.z - instanced_model->positions[helper_models->active_instanced_model_index].z)) / instanced_model->default_boxes[helper_models->active_instanced_model_index].depth;
		}
	
		scale_instanced_model(instanced_model, helper_models->active_instanced_model_index, new_scale.x, new_scale.y, new_scale.z);
	}
}

static void translate_instanced_model_using_movesticks(HelperModels *helper_models, InstancedModel *instanced_model, Vector *ray) {
	if(helper_models->active_helper_model_index != -1) {
		if(helper_models->active_helper_model_index == 4)
			move_helper_models_along(helper_models, helper_models->active_helper_model_index, ray, 1, 0, 0);
		else if(helper_models->active_helper_model_index == 5)
			move_helper_models_along(helper_models, helper_models->active_helper_model_index, ray, 0, 1, 0);
		else if(helper_models->active_helper_model_index == 6)
			move_helper_models_along(helper_models, helper_models->active_helper_model_index, ray, 0, 0, 1);

		BoundingBox *ib = &instanced_model->bounding_boxes[helper_models->active_instanced_model_index];
		Vector3 *ip = &instanced_model->positions[helper_models->active_instanced_model_index];
		Vector3 hp = helper_models->cubes._positions[helper_models->active_helper_model_index];

		/* Moving instanced model */
		Vector3 new_pos;
		if(helper_models->active_helper_model_index == 4)
			init_vector(&new_pos, hp.x - ib->width / 2.0f - 0.8f, ip->y, ip->z);
		else if(helper_models->active_helper_model_index == 5)
			init_vector(&new_pos, ip->x, hp.y - ib->height / 2.0f - 0.8f, ip->z);
		else if(helper_models->active_helper_model_index == 6)
			init_vector(&new_pos, ip->x, ip->y, hp.z - ib->depth / 2.0f - 0.8f);
		
		translate_instanced_model(instanced_model, helper_models->active_instanced_model_index, new_pos.x, new_pos.y, new_pos.z);
		/* Moving instanced model */

		/* Moving the other two move sticks */
		float move_stick_x_pos = ip->x + instanced_model->bounding_boxes[helper_models->active_instanced_model_index].width / 2.0f + 0.8f;
		float move_stick_y_pos = ip->y + instanced_model->bounding_boxes[helper_models->active_instanced_model_index].height / 2.0f + 0.8f;
		float move_stick_z_pos = ip->z + instanced_model->bounding_boxes[helper_models->active_instanced_model_index].depth / 2.0f + 0.8f;

		if(helper_models->active_helper_model_index == 4) {
			translate_instanced_helper_model(&helper_models->cubes, 5, ip->x, move_stick_y_pos, ip->z);
			translate_instanced_helper_model(&helper_models->cubes, 6, ip->x, ip->y, move_stick_z_pos);
		}
		else if(helper_models->active_helper_model_index == 5) {
			translate_instanced_helper_model(&helper_models->cubes, 4, move_stick_x_pos, ip->y, ip->z);
			translate_instanced_helper_model(&helper_models->cubes, 6, ip->x, ip->y, move_stick_z_pos);
		}
		else if(helper_models->active_helper_model_index == 6) {
			translate_instanced_helper_model(&helper_models->cubes, 4, move_stick_x_pos, ip->y, ip->z);
			translate_instanced_helper_model(&helper_models->cubes, 5, ip->x, move_stick_y_pos, ip->z);
		}
		/* Moving the other two move sticks */
	}
}

void rotate_helper_models_along(HelperModels *helper_models, uint32_t model_index, Vector *ray, uint8_t along_x, uint8_t along_y, uint8_t along_z) {
	if(model_index >= helper_models->cubes.num_models) {
		printf("WARNING: In rotate_helper_models_along(): model_index(%u) >= num_models(%u)\n", model_index, helper_models->cubes.num_models);
	}
	else {
		Vector3 pos_1, pos_2;
		if(along_z) {
			Vector3 model_position = helper_models->cubes._positions[model_index];
			uint8_t valid_1 = get_position_along_axis(&model_position, &pos_1, ray, 1, 0, 0);
			uint8_t valid_2 = get_position_along_axis(&model_position, &pos_2, ray, 0, 1, 0);
			if(valid_1 && valid_2) {
				Vector3 vec_1 = { 1, 0, 0 };
				Vector3 vec_2 = { pos_1.x, pos_2.y, 0 };
				float angle = angle_vectors(&vec_1, &vec_2);
				angle = 360 - to_degrees(angle);

				rotate_instanced_helper_model_about(&helper_models->cubes, 4, 0, 0, 1, model_position.x, model_position.y, model_position.z, angle);
			}
		}
	}
}

static void rotate_instanced_model_using_movesticks(HelperModels *helper_models, InstancedModel *instanced_model, Vector *ray) {
	if(helper_models->active_helper_model_index != -1) {
		if(helper_models->active_helper_model_index == 4)
			rotate_helper_models_along(helper_models, helper_models->active_helper_model_index, ray, 0, 0, 1);
	}
}

void handle_mouse_movement_helper_models(HelperModels *helper_models, InstancedModel *instanced_model, ModelPropertiesGUI *model_properties_gui, Vector *ray) {
	if(model_properties_gui->check_translation.checked)
		translate_instanced_model_using_movesticks(helper_models, instanced_model, ray);
	else if(model_properties_gui->check_scale.checked)
		scale_instanced_model_using_movesticks(helper_models, instanced_model, ray);
	else if(model_properties_gui->check_rotation.checked)
		rotate_instanced_model_using_movesticks(helper_models, instanced_model, ray);
}

void handle_mouse_click_helper_models(HelperModels *helper_models, Vector *ray) {
	uint32_t hit_index = obb(helper_models->cubes.models, helper_models->cubes.num_models, helper_models->cubes.bounding_boxes, ray);

	if(hit_index != -1) {
		if(helper_models->active_helper_model_index != -1) {
			Vector3 dc = { helper_models->default_colors[helper_models->active_helper_model_index].x, helper_models->default_colors[helper_models->active_helper_model_index].y, helper_models->default_colors[helper_models->active_helper_model_index].z };
			set_color_instanced_helper_model(&helper_models->cubes, helper_models->active_helper_model_index, dc.x, dc.y, dc.z, 1);
		}
		if(hit_index >= 4 && hit_index <= 6) {
			set_color_instanced_helper_model(&helper_models->cubes, hit_index, 1, 1, 1, 1);
			helper_models->active_helper_model_index = hit_index;
		}
	}
	else if(helper_models->active_helper_model_index != -1) {
		Vector3 dc = { helper_models->default_colors[helper_models->active_helper_model_index].x, helper_models->default_colors[helper_models->active_helper_model_index].y, helper_models->default_colors[helper_models->active_helper_model_index].z };
		set_color_instanced_helper_model(&helper_models->cubes, helper_models->active_helper_model_index, dc.x, dc.y, dc.z, 1);
		helper_models->active_helper_model_index = -1;
	}
}

void draw_helper_models(HelperModels *helper_models, InstancedDirLight *instanced_dir_light) {
	draw_instanced_helper_model(&helper_models->cubes, instanced_dir_light);
}
