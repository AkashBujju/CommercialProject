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

		for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		 	/* @Note: Change the below to custom values later */
		 	init_vector(&instanced_model->scales[i], 1, 1, 1);
		 	init_vector(&instanced_model->positions[i], 0, 0, 0);
			make_identity(&instanced_model->models[i]);
			/* @Note: Change the width, height and depth to be the bounding of the model */
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

		/* Instance VBOs */
		glGenBuffers(1, &instanced_model->instanceAmbientVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceAmbientVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * instanced_model->num_models, instanced_model->ambient, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_model->instanceDiffuseVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceDiffuseVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * instanced_model->num_models, instanced_model->diffuse, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_model->instanceSpecularVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceSpecularVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3) * instanced_model->num_models, instanced_model->specular, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_model->instanceShininessVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceShininessVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * instanced_model->num_models, instanced_model->shininess, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenBuffers(1, &instanced_model->instanceModelVBO);
    	glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceModelVBO);
    	glBufferData(GL_ARRAY_BUFFER, sizeof(Matrix4) * instanced_model->num_models, instanced_model->models, GL_STATIC_DRAW);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
		/* Instance VBOs */

		/* Position and Normals */
		glGenVertexArrays(1, &instanced_model->vao);
		glGenBuffers(1, &instanced_model->vbo);
		glBindVertexArray(instanced_model->vao);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->vbo);
		/* @Note: Should we change this to dynamic draw later? */
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_floats, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		/* Position and Normals */

		/* Instanced Model Data */
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
		/* Instanced Model Data */

		/* Instanced Ambient Data */
		glEnableVertexAttribArray(6);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceAmbientVBO);
    	glVertexAttribPointer(6, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(6, 1);
		/* Instanced Ambient Data */

		/* Instanced Diffuse Data */
		glEnableVertexAttribArray(7);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceDiffuseVBO);
    	glVertexAttribPointer(7, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(7, 1);
		/* Instanced Diffuse Data */

		/* Instanced Specular Data */
		glEnableVertexAttribArray(8);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceSpecularVBO);
    	glVertexAttribPointer(8, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(8, 1);
		/* Instanced Specular Data */

		/* Instanced Shininess Data */
		glEnableVertexAttribArray(9);
		glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceShininessVBO);
    	glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    	glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glVertexAttribDivisor(9, 1);
		/* Instanced Shininess Data */

		/* @Note: Is this okay at this point?
		free(vertices); */
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

void draw_instanced_model(InstancedModel *instanced_model) {
	glUseProgram(instanced_model->program);

	/* Updating in-struct Model Matrix */
	for(uint32_t i = 0; i < instanced_model->num_models; ++i) {
		make_identity(&instanced_model->models[i]);

		translate_matrix(&instanced_model->models[i], instanced_model->positions[i].x, instanced_model->positions[i].y, instanced_model->positions[i].z);
		scale(&instanced_model->models[i], instanced_model->width[i], instanced_model->height[i], instanced_model->depth[i]);
 		Vector3 tmp_position;
 		init_vector(&tmp_position, 
				      instanced_model->positions[i].x, 
						instanced_model->positions[i].y, 
						instanced_model->positions[i].z);
 		translate_matrix(&instanced_model->models[i], 0, 0, 0);
 		rotate(&instanced_model->models[i], &instanced_model->rotation_axes[i], instanced_model->angle_in_degree[i]);
 		translate_matrix(&instanced_model->models[i], tmp_position.x, tmp_position.y, tmp_position.z);
	}
	/* Updating in-struct Model Matrix */

	/* Updating ModelVBO */
   glBindBuffer(GL_ARRAY_BUFFER, instanced_model->instanceModelVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Matrix4) * instanced_model->num_models, instanced_model->models);
   glBindBuffer(GL_ARRAY_BUFFER, 0);
	/* Updating ModelVBO */

	glBindVertexArray(instanced_model->vao);
	glDrawArraysInstanced(GL_TRIANGLES, 0, instanced_model->num_vertices, instanced_model->num_models);
}
