#include "light.h"
#include "utils.h"
#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

void load_light(Light *light, GLuint program, const char* model_filename) {
	light->program = program;

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

		glGenVertexArrays(1, &light->vao);
		glGenBuffers(1, &light->vbo);
		glBindVertexArray(light->vao);
		glBindBuffer(GL_ARRAY_BUFFER, light->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) * num_floats, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		free(vertices); /* @Note: Is this okay at this point? */

		init_vector(&light->scale, 1, 1, 1);
		init_vector(&light->position, 0, 0, 0);
		/* @Note: Change the below to custom values later */
		light->width = 1;
		light->height = 1;
		light->depth = 1;
	}
}

void translate_light(Light *light, float x, float y, float z) {
	light->position.x = x;
	light->position.y = y;
	light->position.z = z;
}

void scale_light(Light *light, float x, float y, float z) {
	light->scale.x = x;
	light->scale.y = y;
	light->scale.z = z;

	light->width *= x;
	light->height *= y;
	light->depth *= z;
}

void draw_light(Light *light) {
	glUseProgram(light->program);
	make_identity(&light->model);

	/* Translation & Scaling */
	translate_matrix(&light->model, light->position.x, light->position.y, light->position.z);
	scale(&light->model, light->scale.x, light->scale.y, light->scale.z);
	/* Translation & Scaling */

	set_matrix4(light->program, "model", &light->model);
	glBindVertexArray(light->vao);
	glDrawArrays(GL_TRIANGLES, 0, light->num_vertices);
}
