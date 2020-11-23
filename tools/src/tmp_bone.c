#include "tmp_bone.h"
#include "../../src/shader.h"
#include <stdio.h>
#include <stdlib.h>

void add_vertex_to_mesh(Mesh *mesh, uint32_t line_index, uint32_t vertex_index, float x1, float y1, float z1, float b0, float b1, float w0, float w1) {
	if(line_index > 2 || vertex_index > 1) {
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

	for(uint32_t i = 0; i < mesh->num_lines; ++i) {
		printf("%.3f %.3f %.3f %.2f %.2f %.2f %.2f\n", mesh->lines[i].positions[0].x, mesh->lines[i].positions[0].y, mesh->lines[i].positions[0].z, mesh->lines[i].bone_indices[0].x, mesh->lines[i].bone_indices[0].y, mesh->lines[i].weights[0].x, mesh->lines[i].weights[0].y);
		printf("%.3f %.3f %.3f %.2f %.2f %.2f %.2f\n", mesh->lines[i].positions[1].x, mesh->lines[i].positions[1].y, mesh->lines[i].positions[1].z, mesh->lines[i].bone_indices[1].x, mesh->lines[i].bone_indices[1].y, mesh->lines[i].weights[1].x, mesh->lines[i].weights[1].y);
	}

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

	make_identity(&mesh->model);
	for(uint32_t i = 0; i < mesh->num_lines; ++i) {
		make_identity(&mesh->bones[i]);
	}
}

void translate_bone_in_mesh(Mesh *mesh, uint32_t bone_index, float x, float y, float z) {
	if(bone_index >= mesh->num_lines) {
		printf("WARNING: translate_bone_in_mesh()\n");
	}
	else {
		translate_matrix(&mesh->bones[bone_index], x, y, z);
	}
}

void translate_mesh(Mesh *mesh, float x, float y, float z) {
	translate_matrix(&mesh->model, x, y, z);
}

void rotate_bone_in_mesh(Mesh *mesh, uint32_t bone_index, float x, float y, float z, float degree) {
	if(bone_index >= mesh->num_lines) {
		printf("WARNING: translate_bone_in_mesh()\n");
	}
	else {
		Vector3 axes = { x, y, z };
		rotate(&mesh->bones[bone_index], &axes, degree);
	}
}

void rotate_mesh(Mesh *mesh, float x, float y, float z, float degree) {
	Vector3 axes = { x, y, z };
	rotate(&mesh->model, &axes, degree);
}

void draw_mesh(Mesh *mesh) {
	glUseProgram(mesh->program);

	Matrix4 ib_1 = matrix_inverse(&mesh->bones[0]);
	Matrix4 ib_2 = matrix_inverse(&mesh->bones[1]);
	Matrix4 ib_3 = matrix_inverse(&mesh->bones[2]);

	set_matrix4(mesh->program, "model", &mesh->model);
	set_matrix4(mesh->program, "bones[0]", &ib_1);
	set_matrix4(mesh->program, "bones[1]", &ib_2);
	set_matrix4(mesh->program, "bones[2]", &ib_3);

	glBindVertexArray(mesh->vao);
	glDrawArrays(GL_LINES, 0, mesh->num_lines * 2);
}
