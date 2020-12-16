#include "exp_lines.h"
#include "../../src/utils.h"
#include <stdio.h>

extern char* tmp_models_path;
extern GLuint exp_lines_program;

void init_exp_lines(ExpLines *exp_lines) {
	exp_lines->num_lines = 0;
	exp_lines->program = exp_lines_program;
}

void add_exp_line(ExpLines *exp_lines, float x1, float y1, float z1, float x2, float y2, float z2, float r, float g, float b) {
	if(exp_lines->num_lines >= MAX_EXP_LINES) {
		printf("add_exp_lines(): exp_lines->num_lines(%d) >= MAX_EXP_LINES(%d)\n", exp_lines->num_lines, MAX_EXP_LINES);
	}
	else {
		float vertices[] = {
			x1, y1, z1, r, g, b,
			x2, y2, z2, r, g, b
		};

		GLuint tmp_vbo;
		glGenVertexArrays(1, &exp_lines->vao[exp_lines->num_lines]);
		glGenBuffers(1, &tmp_vbo);
		glBindVertexArray(exp_lines->vao[exp_lines->num_lines]);
		glBindBuffer(GL_ARRAY_BUFFER, tmp_vbo);
		/* @Note: Should we change this to dynamic draw later? */
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		exp_lines->num_lines += 1;
	}
}

void draw_exp_lines(ExpLines *exp_lines) {
	for(uint32_t i = 0; i < exp_lines->num_lines; ++i) {
		glUseProgram(exp_lines->program);
		glBindVertexArray(exp_lines->vao[i]);
		glDrawArrays(GL_LINES, 0, 6);
	}
}
