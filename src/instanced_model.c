#include "instanced_model.h"
#include "utils.h"
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char* assets_path;

void load_instanced_model(InstancedModel *instanced_model, GLuint program, const char* model_filename, uint32_t num_models) {
	if(num_models >= MAX_MODELS) {
		printf("WARNING: num_models(%d) >= MAX_MODELS(%d)\n", num_models, MAX_MODELS);
		printf("WARNING: Setting num_models to %d\n", MAX_MODELS);
		num_models = MAX_MODELS;
	}

	instanced_model->program = program;
	instanced_model->num_models = num_models;

	FILE *file = fopen(model_filename, "r");
	if(!file) {
		printf("ERROR: File %s not found\n", model_filename);
		printf("ERROR: Not created model!!!\n");
	}
	else {
		/* Retriving number of floats */
		uint32_t num_floats;
		fscanf(file, "%u", &num_floats);
		fclose(file);
		/* Retriving number of floats */

		/* @Note: Change this if normals and other info are added */
		/* 3 for the co-ordinates and 3 for the normals := 6 */
		instanced_model->num_vertices = num_floats / 6;

		float *vertices = (float*)malloc(sizeof(float) * num_floats);
		read_floats_from_file(model_filename, vertices);

		glGenVertexArrays(1, &instanced_model->vao);
		glGenBuffers(1, &instanced_model->vbo);
		glBindVertexArray(instanced_model->vao);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_floats, vertices, GL_STATIC_DRAW); /* @Note: Should we change this to dynamic draw later? */
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		/* @Note: Is this okay at this point?
		free(vertices); */

		for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
			/* @Note: Change the below to custom values later */
			init_vector(&instanced_model->scales[i], 1, 1, 1);
			init_vector(&instanced_model->positions[i], 0, 0, 0);
			instanced_model->width[i] = 1;
			instanced_model->height[i] = 1;
			instanced_model->depth[i] = 1;
			instanced_model->angle_in_degree[i] = 0;
			instanced_model->shininess[i] = 32;
			init_vector(&instanced_model->ambient[i], 0.5f, 0.5f, 0.5f);
			init_vector(&instanced_model->diffuse[i], 0.5f, 0.5f, 0.5f);
			init_vector(&instanced_model->specular[i], 0.5f, 0.5f, 0.5f);
			init_vector(&instanced_model->rotation_axes[i], 0, 0, 0);
		}
	}
}

void translate_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In translate_instanced_model(): model_index(%d) >= num_models(%d)\n", model_index, instanced_model->num_models);
	}
	else {
		instanced_model->positions[model_index].x = x;
		instanced_model->positions[model_index].y = y;
		instanced_model->positions[model_index].z = z;
	}
}

void scale_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In scale_instanced_model(): model_index(%d) >= num_models(%d)\n", model_index, instanced_model->num_models);
	}
	else {
		instanced_model->scales[model_index].x = x;
		instanced_model->scales[model_index].y = y;
		instanced_model->scales[model_index].z = z;

		instanced_model->width[model_index] *= x;
		instanced_model->height[model_index] *= y;
		instanced_model->depth[model_index] *= z;
	}
}

void set_material_instanced_model(InstancedModel *instanced_model, uint32_t model_index, char* material_name) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In set_color_instanced_model(): model_index(%d) >= num_models(%d)\n", model_index, instanced_model->num_models);
	}
	else {
		Material material = get_material_info_from_file(combine_string(assets_path, "materials/materials.info"), material_name);
		if(material.exists) {
			instanced_model->ambient[model_index] = material.ambient;
			instanced_model->diffuse[model_index] = material.diffuse;
			instanced_model->specular[model_index] = material.specular;
			instanced_model->shininess[model_index] = material.shininess;
		}
	}
}

void rotate_instanced_model(InstancedModel *instanced_model, uint32_t model_index, float x, float y, float z, float degree) {
	if(model_index >= instanced_model->num_models) {
		printf("WARNING: In rotate_instanced_model(): model_index(%d) >= num_models(%d)\n", model_index, instanced_model->num_models);
	}
	else {
		instanced_model->angle_in_degree[model_index] = degree;
		instanced_model->rotation_axes[model_index].x = x;
		instanced_model->rotation_axes[model_index].y = y;
		instanced_model->rotation_axes[model_index].z = z;
	}
}

static void make_glsl_string(char* var_name, uint32_t index, char* destination) {
	char index_str[5];
	strcpy(destination, var_name);
	strcat(destination, "[");
	sprintf(index_str, "%i", index);
	strcat(destination, index_str);
	strcat(destination, "]");
}

void draw_instanced_model(InstancedModel *instanced_model) {
	glUseProgram(instanced_model->program);

	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		make_identity(&instanced_model->models[i]);
	}

	/* Translation & Scaling */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		translate_matrix(&instanced_model->models[i], instanced_model->positions[i].x,
				instanced_model->positions[i].y, instanced_model->positions[i].z);
	}

	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		scale(&instanced_model->models[i], instanced_model->scales[i].x,
				instanced_model->scales[i].y, instanced_model->scales[i].z);
	}
	/* Translation & Scaling */

	/* Rotation */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		Vector3 tmp;
		init_vector(&tmp, instanced_model->positions[i].x, instanced_model->positions[i].y, instanced_model->positions[i].z);
		translate_matrix(&instanced_model->models[i], 0, 0, 0);
		rotate(&instanced_model->models[i], &instanced_model->rotation_axes[i], instanced_model->angle_in_degree[i]);
		translate_matrix(&instanced_model->models[i], tmp.x, tmp.y, tmp.z);
	}
	/* Rotation */

	/* Setting the Model matrix */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		char model_name[25];
		make_glsl_string("models", i, model_name);
		set_matrix4(instanced_model->program, model_name, &instanced_model->models[i]);
	}
	/* Setting the Model matrix */

	/* Setting the ambient */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		char name[30];
		make_glsl_string("material", i, name);
		strcat(name, ".ambient");
		set_vector3(instanced_model->program, name, &instanced_model->ambient[i]);
	}
	/* Setting the ambient */

	/* Setting the diffuse */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		char name[30];
		make_glsl_string("materials", i, name);
		strcat(name, ".diffuse");
		set_vector3(instanced_model->program, name, &instanced_model->diffuse[i]);
	}
	/* Setting the diffuse */

	/* Setting the specular */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		char name[30];
		make_glsl_string("materials", i, name);
		strcat(name, ".specular");
		set_vector3(instanced_model->program, name, &instanced_model->specular[i]);
	}
	/* Setting the specular */

	/* Setting the shininess */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		char name[30];
		make_glsl_string("materials", i, name);
		strcat(name, ".shininess");
		set_float(instanced_model->program, name, instanced_model->shininess[i]);
	}
	/* Setting the shininess */

	glBindVertexArray(instanced_model->vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, instanced_model->num_vertices, instanced_model->num_models);
}
