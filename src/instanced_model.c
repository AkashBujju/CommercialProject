#include "instanced_model.h"
#include "utils.h"
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

extern char* assets_path;

void load_instanced_model(InstancedModel *instanced_model, GLuint program, char* model_name, const char* model_filename) {
	instanced_model->program = program;
	instanced_model->num_models = 0;

	FILE *file = fopen(model_filename, "r");
	if(!file) {
		printf("ERROR: File %s not found\n", model_filename);
		printf("ERROR: Not created model!!!\n");
	}
	else {
		strcpy(instanced_model->model_name, model_name);
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
		instanced_model->num_vertices = num_floats / 6;

		for(uint32_t i = 0; i < MAX_INSTANCED_MODELS; ++i) {
		 	/* @Note: Change the below to custom values later */
		 	init_vector(&instanced_model->scales[i], 1, 1, 1);
		 	init_vector(&instanced_model->positions[i], 0, 0, 0);
		 	init_vector(&instanced_model->initial_positions[i], 0, 0, 0);
		 	init_vector(&instanced_model->rotation_points[i], 0, 0, 0);
			make_identity(&instanced_model->models[i]);
			/* @Note: Change the width, height and depth to be the bounding of the model */
			instanced_model->bounding_boxes[i].width = model_width;
			instanced_model->bounding_boxes[i].height = model_height;
			instanced_model->bounding_boxes[i].depth = model_depth;
			instanced_model->default_boxes[i].width = model_width;
			instanced_model->default_boxes[i].height = model_height;
			instanced_model->default_boxes[i].depth = model_depth;
		 	instanced_model->angle_in_degree[i] = 0;
		 	instanced_model->initial_angle_in_degree[i] = 0;
		 	instanced_model->shininess[i] = 32;
		 	instanced_model->alpha[i] = 1;
		 	init_vector(&instanced_model->ambient[i], 0.5f, 0.5f, 0.5f);
		 	init_vector(&instanced_model->diffuse[i], 0.5f, 0.5f, 0.5f);
		 	init_vector(&instanced_model->specular[i], 0.5f, 0.5f, 0.5f);
		 	init_vector(&instanced_model->rotation_axes[i], 0, 0, 0);
		 	init_vector(&instanced_model->initial_rotation_axes[i], 0, 0, 0);
		}

		/* Instance VBOs */
		glGenBuffers(1, &instanced_model->instanceAmbientVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceAmbientVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * MAX_INSTANCED_MODELS, instanced_model->ambient, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_model->instanceDiffuseVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceDiffuseVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * MAX_INSTANCED_MODELS, instanced_model->diffuse, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_model->instanceSpecularVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceSpecularVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * MAX_INSTANCED_MODELS, instanced_model->specular, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_model->instanceShininessVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceShininessVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_INSTANCED_MODELS, instanced_model->shininess, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_model->instanceAlphaVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceAlphaVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MAX_INSTANCED_MODELS, instanced_model->alpha, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_model->instanceModelVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceModelVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4) * MAX_INSTANCED_MODELS, instanced_model->models, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
		/* Instance VBOs */

		/* Position and Normals */
		GLuint tmp_vbo;
		glGenVertexArrays(1, &instanced_model->vao);
		glGenBuffers(1, &tmp_vbo);
		glBindVertexArray(instanced_model->vao);
		glBindBuffer(GL_ARRAY_BUFFER, tmp_vbo);
		/* @Note: Should we change this to dynamic draw later? */
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_floats, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		/* Position and Normals */

		/* Instanced Model Attrib */
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceModelVBO);
    	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceModelVBO);
    	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(4 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceModelVBO);
    	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(4, 1);

		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceModelVBO);
    	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(5, 1);
		/* Instanced Model Attrib */

		/* Instanced Ambient Attrib */
		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceAmbientVBO);
    	glVertexAttribPointer(6, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(6, 1);
		/* Instanced Ambient Attrib */

		/* Instanced Diffuse Attrib */
		glEnableVertexAttribArray(7);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceDiffuseVBO);
    	glVertexAttribPointer(7, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(7, 1);
		/* Instanced Diffuse Attrib */

		/* Instanced Specular Attrib */
		glEnableVertexAttribArray(8);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceSpecularVBO);
    	glVertexAttribPointer(8, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(8, 1);
		/* Instanced Specular Attrib */

		/* Instanced Shininess Attrib */
		glEnableVertexAttribArray(9);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceShininessVBO);
    	glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(9, 1);
		/* Instanced Shininess Attrib */

		/* Instanced Shininess Attrib */
		glEnableVertexAttribArray(10);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceAlphaVBO);
    	glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(10, 1);
		/* Instanced Shininess Attrib */

		/* @Note: Is this okay at this point?
		free(vertices); */
	}
}

void add_model(InstancedModel *instanced_model, float x, float y, float z, char *material_name) {
	if(instanced_model->num_models >= MAX_INSTANCED_MODELS) {
		printf("WARNING: num_models(%d) >= MAX_INSTANCED_MODELS(%d)\n", instanced_model->num_models, MAX_INSTANCED_MODELS);
		printf("WARNING: Setting num_models to %d\n", MAX_INSTANCED_MODELS);
	}
	else {
		instanced_model->num_models += 1;
		translate_instanced_model(instanced_model, instanced_model->num_models - 1, x, y, z);
		set_material_instanced_model(instanced_model, instanced_model->num_models - 1, material_name);
	}
}

void set_initial_pose_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float px, float py, float pz, float axis_x, float axis_y, float axis_z, float angle) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In set_initial_pose_instanced_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_model->num_models);
	}
	else {
		init_vector(&instanced_model->initial_positions[model_index], px, py, pz);
		init_vector(&instanced_model->initial_rotation_axes[model_index], axis_x, axis_y, axis_z);
		instanced_model->initial_angle_in_degree[model_index] = angle;
	}
}

void translate_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In translate_instanced_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_model->num_models);
	}
	else {
		instanced_model->positions[model_index].x = x;
		instanced_model->positions[model_index].y = y;
		instanced_model->positions[model_index].z = z;
	}
}

void scale_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In scale_instanced_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_model->num_models);
	}
	else {
		if(x != 0) {
			instanced_model->scales[model_index].x = x;
			instanced_model->bounding_boxes[model_index].width = instanced_model->default_boxes[model_index].width * x;
		}
		if(y != 0) {
			instanced_model->scales[model_index].y = y;
			instanced_model->bounding_boxes[model_index].height = instanced_model->default_boxes[model_index].height * y;
		}
		if(z != 0) {
			instanced_model->scales[model_index].z = z;
			instanced_model->bounding_boxes[model_index].depth = instanced_model->default_boxes[model_index].depth * z;
		}
	}
}

void set_alpha_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float alpha_value) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In set_alpha_instanced_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_model->num_models);
	}
	else {
		instanced_model->alpha[model_index] = alpha_value;
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceAlphaVBO);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * model_index, sizeof(float), &instanced_model->alpha[model_index]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void set_material_instanced_model(InstancedModel *instanced_model, uint32_t model_index, char* material_name) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In set_material_instanced_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_model->num_models);
	}
	else {
		Material material = get_material_info_from_file(combine_string(assets_path, "materials/materials.info"), material_name);
		if(material.exists) {
			instanced_model->ambient[model_index] = material.ambient;
			instanced_model->diffuse[model_index] = material.diffuse;
			instanced_model->specular[model_index] = material.specular;
			instanced_model->shininess[model_index] = material.shininess;

			glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceAmbientVBO);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector3) * model_index, sizeof(Vector3), &instanced_model->ambient[model_index]);
			glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceDiffuseVBO);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector3) * model_index, sizeof(Vector3), &instanced_model->diffuse[model_index]);
			glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceSpecularVBO);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vector3) * model_index, sizeof(Vector3), &instanced_model->specular[model_index]);
			glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceShininessVBO);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * model_index, sizeof(float), &instanced_model->shininess[model_index]);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
}

void rotate_instanced_model_about(InstancedModel *instanced_model, uint32_t model_index, float axis_x, float axis_y, float axis_z, float about_x, float about_y, float about_z, float degree) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In rotate_instanced_model(): model_index(%u) >= num_models(%u)\n", model_index, instanced_model->num_models);
	}
	else {
		instanced_model->angle_in_degree[model_index] = degree;
		init_vector(&instanced_model->rotation_points[model_index], about_x, about_y, about_z);
		instanced_model->rotation_axes[model_index].x = axis_x;
		instanced_model->rotation_axes[model_index].y = axis_y;
		instanced_model->rotation_axes[model_index].z = axis_z;
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

void draw_instanced_model(InstancedModel *instanced_model, InstancedDirLight *instanced_dir_light, InstancedSpotLight *instanced_spot_light) {
	glUseProgram(instanced_model->program);

	/* Updating in-struct Model Matrix */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		make_identity(&instanced_model->models[i]);

		/* Setting initial pose */
		translate_matrix(&instanced_model->models[i], 0, 0, 0);
		rotate(&instanced_model->models[i], &instanced_model->initial_rotation_axes[i], instanced_model->initial_angle_in_degree[i]);
		translate_matrix(&instanced_model->models[i], instanced_model->initial_positions[i].x, instanced_model->initial_positions[i].y, instanced_model->initial_positions[i].z);
		/* Setting initial pose */

		translate_matrix(&instanced_model->models[i], instanced_model->positions[i].x, instanced_model->positions[i].y, instanced_model->positions[i].z);
		scale(&instanced_model->models[i], instanced_model->scales[i].x, instanced_model->scales[i].y, instanced_model->scales[i].z);

		/* Rotation about a point */
		Vector3 *about_point = &instanced_model->rotation_points[i];
		Vector3 disp = { instanced_model->positions[i].x - about_point->x, instanced_model->positions[i].y - about_point->y, instanced_model->positions[i].z - about_point->z };
		translateBy_matrix(&instanced_model->models[i], -disp.x, -disp.y, -disp.z);
		rotate(&instanced_model->models[i], &instanced_model->rotation_axes[i], instanced_model->angle_in_degree[i]);
		translateBy_matrix(&instanced_model->models[i], +disp.x, +disp.y, +disp.z);
		/* Rotation about a point */
	}
	/* Updating in-struct Model Matrix */

	/* Updating ModelVBO */
	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceModelVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Matrix4) * instanced_model->num_models, instanced_model->models);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	/* Updating ModelVBO */

	/* Directional Lights */
	set_integer(instanced_model->program, "num_dir_lights", instanced_dir_light->num_models);
	for(uint32_t di = 0; di < instanced_dir_light->num_models; ++di) {
		char var_name[50];
		make_glsl_string(var_name, di, "dir_lights", "position");
		set_vector3(instanced_model->program, var_name, &instanced_dir_light->positions[di]);
	}
	/* Directional Lights */

	/* Spot Lights */
	set_integer(instanced_model->program, "num_spot_lights", instanced_spot_light->num_models);
	for(uint32_t di = 0; di < instanced_spot_light->num_models; ++di) {
		char var_name[50];
		make_glsl_string(var_name, di, "spot_lights", "position");
		set_vector3(instanced_model->program, var_name, &instanced_spot_light->positions[di]);
		make_glsl_string(var_name, di, "spot_lights", "direction");
		set_vector3(instanced_model->program, var_name, &instanced_spot_light->directions[di]);
	}
	/* Spot Lights */

	glEnable(GL_BLEND);
	glBindVertexArray(instanced_model->vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, instanced_model->num_vertices, instanced_model->num_models);
	glDisable(GL_BLEND);
}
