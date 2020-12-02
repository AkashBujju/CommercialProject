#include "light.h"
#include "utils.h"
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

/* Dir Light */
void load_instanced_dir_light(InstancedDirLight *light, GLuint program, const char* model_filename, uint32_t num_models) {
	light->program = program;
	light->num_models = num_models;

	if(num_models > MAX_LIGHTS) {
		printf("WARNING: load_instanced_dir_light -> num_models(%u) > MAX_LIGHTS(%d)\n", num_models, MAX_LIGHTS);
	}

	FILE *file = fopen(model_filename, "r");
	if(!file) {
		printf("File %s not found\n", model_filename);
	}
	else {
		/* Retriving number of floats */
		uint32_t num_floats;
		fscanf(file, "%u", &num_floats);
		fclose(file);
		/* Retriving number of floats */

		/* @Note: Change this if normals and other info are added */
		/* 3 for the co-ordinates, 3 for the normals */
		light->num_vertices = num_floats / 6;

		float *vertices = (float*)malloc(sizeof(float) * num_floats);
		read_floats_from_file(model_filename, vertices);

		/* @Note: Change the below to custom values later */
		for(uint32_t i = 0; i < light->num_models; ++i) {
			init_vector(&light->scales[i], 1, 1, 1);
			init_vector(&light->positions[i], 0, 0, 0);
			make_identity(&light->models[i]);
		}

		/* Instance VBOs */
		glGenBuffers(1, &light->instanced_vbo);
    	glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4) * light->num_models, light->models, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
		/* Instance VBOs */

		glGenVertexArrays(1, &light->vao);
		glGenBuffers(1, &light->vbo);
		glBindVertexArray(light->vao);
		glBindBuffer(GL_ARRAY_BUFFER, light->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * num_floats, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		/* Instanced Model Data */
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
    	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(1, 1);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
    	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(4 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
    	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
    	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(4, 1);
		/* Instanced Model Data */

		/* @Note: Is this okay at this point?
		free(vertices); */
	}
}

void translate_instanced_dir_light(InstancedDirLight *light, uint32_t model_index, float x, float y, float z) {
	if(model_index >= light->num_models) {
		printf("WARNING: In translate_instanced_dir_light(): model_index(%u) >= num_models(%u)\n", model_index, light->num_models);
	}
	else {
		light->positions[model_index].x = x;
		light->positions[model_index].y = y;
		light->positions[model_index].z = z;
	}
}

void scale_instanced_dir_light(InstancedDirLight *light, uint32_t model_index, float x, float y, float z) {
	if(model_index >= light->num_models) {
		printf("WARNING: In scale_instanced_dir_light(): model_index(%u) >= num_models(%u)\n", model_index, light->num_models);
	}
	else {
		light->scales[model_index].x *= x;
		light->scales[model_index].y *= y;
		light->scales[model_index].z *= z;
	}
}

void set_direction_instanced_spot_light(InstancedSpotLight *light, uint32_t model_index, float x, float y, float z) {
	if(model_index >= light->num_models) {
		printf("WARNING: In set_direction_instanced_dir_light(): model_index(%u) >= num_models(%u)\n", model_index, light->num_models);
	}
	else {
		light->directions[model_index].x = x;
		light->directions[model_index].y = y;
		light->directions[model_index].z = z;
	}
}

void draw_instanced_dir_light(InstancedDirLight *light) {
	glUseProgram(light->program);

	/* Translation & Scaling */
	for(uint32_t i = 0; i < light->num_models; ++i) {
		make_identity(&light->models[i]);
		translate_matrix(&light->models[i], light->positions[i].x, light->positions[i].y, light->positions[i].z);
		scale(&light->models[i], light->scales[i].x, light->scales[i].y, light->scales[i].z);
	}
	/* Translation & Scaling */

   glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Matrix4) * light->num_models, light->models);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(light->vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, light->num_vertices, light->num_models);
}
/* Dir Light */

/* Spot Light */
void load_instanced_spot_light(InstancedSpotLight *light, GLuint program, const char* model_filename, uint32_t num_models) {
	light->program = program;
	light->num_models = num_models;

	FILE *file = fopen(model_filename, "r");
	if(!file) {
		printf("File %s not found\n", model_filename);
	}
	else {
		/* Retriving number of floats */
		uint32_t num_floats;
		fscanf(file, "%u", &num_floats);
		fclose(file);
		/* Retriving number of floats */

		/* @Note: Change this if normals and other info are added */
		/* 3 for the co-ordinates */
		light->num_vertices = num_floats / 3;

		float *vertices = (float*)malloc(sizeof(float) * num_floats);
		read_floats_from_file(model_filename, vertices);

		/* @Note: Change the below to custom values later */
		for(uint32_t i = 0; i < light->num_models; ++i) {
			init_vector(&light->scales[i], 1, 1, 1);
			init_vector(&light->positions[i], 0, 0, 0);
			make_identity(&light->models[i]);
		}

		/* Instance VBOs */
		glGenBuffers(1, &light->instanced_vbo);
    	glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4) * light->num_models, light->models, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
		/* Instance VBOs */

		glGenVertexArrays(1, &light->vao);
		glGenBuffers(1, &light->vbo);
		glBindVertexArray(light->vao);
		glBindBuffer(GL_ARRAY_BUFFER, light->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * num_floats, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		/* Instanced Model Data */
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
    	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(1, 1);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
    	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(4 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(2, 1);

		glEnableVertexAttribArray(3);
		glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
    	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(8 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(3, 1);

		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
    	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void*)(12 * sizeof(float)));
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(4, 1);
		/* Instanced Model Data */

		/* @Note: Is this okay at this point?
		free(vertices); */
	}
}

void translate_instanced_spot_light(InstancedSpotLight *light, uint32_t model_index, float x, float y, float z) {
	if(model_index >= light->num_models) {
		printf("WARNING: In translate_instanced_spot_light(): model_index(%u) >= num_models(%u)\n", model_index, light->num_models);
	}
	else {
		light->positions[model_index].x = x;
		light->positions[model_index].y = y;
		light->positions[model_index].z = z;
	}
}

void scale_instanced_spot_light(InstancedSpotLight *light, uint32_t model_index, float x, float y, float z) {
	if(model_index >= light->num_models) {
		printf("WARNING: In scale_instanced_spot_light(): model_index(%u) >= num_models(%u)\n", model_index, light->num_models);
	}
	else {
		light->scales[model_index].x *= x;
		light->scales[model_index].y *= y;
		light->scales[model_index].z *= z;
	}
}

void draw_instanced_spot_light(InstancedSpotLight *light) {
	glUseProgram(light->program);

	/* Translation & Scaling */
	for(uint32_t i = 0; i < light->num_models; ++i) {
		make_identity(&light->models[i]);
		translate_matrix(&light->models[i], light->positions[i].x, light->positions[i].y, light->positions[i].z);
		scale(&light->models[i], light->scales[i].x, light->scales[i].y, light->scales[i].z);
	}
	/* Translation & Scaling */

   glBindBuffer(GL_ARRAY_BUFFER, light->instanced_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Matrix4) * light->num_models, light->models);
   glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(light->vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, light->num_vertices, light->num_models);
}
/* Spot Light */
