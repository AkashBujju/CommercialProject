#include "tmp_bone.h"
#include "../../src/shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void add_vertex_to_mesh(Mesh *mesh, uint32_t line_index, uint32_t vertex_index, float x1, float y1, float z1, float b0, float b1, float w0, float w1) {
	if(line_index > MAX_LINES - 1 || vertex_index > 1) {
		printf("WARNING add_vertex_to_mesh()\n");
	}
	else {
		init_vector(&mesh->lines[line_index].positions[vertex_index], x1, y1, z1);
		init_vector2(&mesh->lines[line_index].bone_indices[vertex_index], b0, b1);
		init_vector2(&mesh->lines[line_index].weights[vertex_index], w0, w1);
	}
}

void compile_mesh(Mesh *mesh, GLuint program, uint32_t num_vertices) {
	uint32_t num_floats = num_vertices * 7;	
	float *verts = (float*)malloc(sizeof(float) * num_floats);
	/* @Note: When can we free the memory? */

	mesh->num_lines = num_vertices / 2;
	mesh->program = program;

	uint32_t index = 0;
	for(uint32_t i = 0; i < mesh->num_lines; ++i) {
		verts[index++] = mesh->lines[i].positions[0].x;
		verts[index++] = mesh->lines[i].positions[0].y;
		verts[index++] = mesh->lines[i].positions[0].z;
		verts[index++] = mesh->lines[i].bone_indices[0].x;
		verts[index++] = mesh->lines[i].bone_indices[0].y;
		verts[index++] = mesh->lines[i].weights[0].x;
		verts[index++] = mesh->lines[i].weights[0].y;

		verts[index++] = mesh->lines[i].positions[1].x;
		verts[index++] = mesh->lines[i].positions[1].y;
		verts[index++] = mesh->lines[i].positions[1].z;
		verts[index++] = mesh->lines[i].bone_indices[1].x;
		verts[index++] = mesh->lines[i].bone_indices[1].y;
		verts[index++] = mesh->lines[i].weights[1].x;
		verts[index++] = mesh->lines[i].weights[1].y;
	}

	/*
	for(uint32_t i = 0; i < mesh->num_lines; ++i) {
		printf("%.3f %.3f %.3f %.2f %.2f %.2f %.2f\n", mesh->lines[i].positions[0].x, mesh->lines[i].positions[0].y, mesh->lines[i].positions[0].z, mesh->lines[i].bone_indices[0].x, mesh->lines[i].bone_indices[0].y, mesh->lines[i].weights[0].x, mesh->lines[i].weights[0].y);
		printf("%.3f %.3f %.3f %.2f %.2f %.2f %.2f\n", mesh->lines[i].positions[1].x, mesh->lines[i].positions[1].y, mesh->lines[i].positions[1].z, mesh->lines[i].bone_indices[1].x, mesh->lines[i].bone_indices[1].y, mesh->lines[i].weights[1].x, mesh->lines[i].weights[1].y);
	}
	*/

	glGenVertexArrays(1, &mesh->vao);
	glGenBuffers(1, &mesh->vbo);
	glBindVertexArray(mesh->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_floats, verts, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 7 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	/* Init */
	init_vector(&mesh->mesh_position, 0, 0, 0);
	init_vector(&mesh->mesh_orientation, 0, 0, 0);
	mesh->mesh_angle = 0;
	for(uint32_t i = 0; i < mesh->num_lines; ++i) {
		init_vector(&mesh->bones[i].orientation, 0, 0, 0);
		mesh->bones[i].angle = 0;
	}
	/* Init */
}

void translate_mesh(Mesh *mesh, float x, float y, float z) {
	mesh->mesh_position.x = x;
	mesh->mesh_position.y = y;
	mesh->mesh_position.z = z;
}

void rotate_bone_in_mesh(Mesh *mesh, uint32_t bone_index, float x, float y, float z, float degree) {
	if(bone_index >= mesh->num_lines) {
		printf("WARNING: translate_bone_in_mesh()\n");
	}
	else {
		mesh->bones[bone_index].orientation.x = x;
		mesh->bones[bone_index].orientation.y = y;
		mesh->bones[bone_index].orientation.z = z;
		mesh->bones[bone_index].angle = degree;
	}
}

void rotate_mesh(Mesh *mesh, float x, float y, float z, float degree) {
	mesh->mesh_orientation.x = x;
	mesh->mesh_orientation.y = y;
	mesh->mesh_orientation.z = z;
	mesh->mesh_angle = degree;
}

static void make_glsl_string(char* var_name, uint32_t index, char* destination) {
	char index_str[5];
	strcpy(destination, var_name);
	strcat(destination, "[");
	sprintf(index_str, "%i", index);
	strcat(destination, index_str);
	strcat(destination, "]");
}

void draw_mesh(Mesh *mesh) {
	glUseProgram(mesh->program);

	/* Make Identity */
	make_identity(&mesh->mesh_model);
	for(uint32_t i = 0; i < mesh->num_lines; ++i)
		make_identity(&mesh->bones[i].model);
	/* Make Identity */

	/* Translation */
	translate_matrix(&mesh->mesh_model, mesh->mesh_position.x, mesh->mesh_position.y, mesh->mesh_position.z);
	/* Translation */

	/* Rotation */
	Vector3 tmp;
	init_vector(&tmp, mesh->mesh_position.x, mesh->mesh_position.y, mesh->mesh_position.z);
	translate_matrix(&mesh->mesh_model, 0, 0, 0);
	rotate(&mesh->mesh_model, &mesh->mesh_orientation, mesh->mesh_angle);
	translate_matrix(&mesh->mesh_model, tmp.x, tmp.y, tmp.z);
	for(uint32_t i = 0; i < mesh->num_lines; ++i) {
		rotate(&mesh->bones[i].model, &mesh->bones[i].orientation, mesh->bones[i].angle);
	}
	/* Rotation */

	/* Setting the model matrices */
	set_matrix4(mesh->program, "model", &mesh->mesh_model);
	for(uint32_t i = 0; i < mesh->num_lines; ++i) {
		char destination[50];
		make_glsl_string("bones", i, destination);
		set_matrix4(mesh->program, destination, &mesh->bones[i].model);
	}
	/* Setting the model matrices */

	glBindVertexArray(mesh->vao);
	glDrawArrays(GL_LINES, 0, mesh->num_lines * 2);
}
