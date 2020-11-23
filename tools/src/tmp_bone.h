#ifndef TMP_BONE_H
#define TMP_BONE_H

#include <glad/glad.h>
#include <stdint.h>
#include "../../src/math.h"

typedef struct Line {
	Vector3 positions[2];
	Vector2 bone_indices[2];
	Vector2 weights[2];
} Line;

typedef struct Mesh {
	Matrix4 model;
	GLuint vao, vbo, program;
	Matrix4 bones[3];
	Line lines[3];
	uint32_t num_lines;
} Mesh;

void add_vertex_to_mesh(Mesh *mesh, uint32_t line_index, uint32_t vertex_index, float x1, float y1, float z1, float b0, float b1, float w0, float w1);
void compile_mesh(Mesh *mesh, GLuint program, uint32_t num_vertices);
void rotate_bone_in_mesh(Mesh *mesh, uint32_t bone_index, float x, float y, float z, float degree);
void translate_bone_in_mesh(Mesh *mesh, uint32_t bone_index, float x, float y, float z);
void rotate_mesh(Mesh *mesh, float x, float y, float z, float degree);
void translate_mesh(Mesh *mesh, float x, float y, float z);
void draw_mesh(Mesh *mesh);

#endif
