#include "rectangle_2d.h"
#include "../../src/shader.h"

void load_rectangle_2d(Rectangle2D *rectangle_2d, GLuint program, GLuint texture_id, float w, float h) {
	rectangle_2d->program = program;
	rectangle_2d->texture_id = texture_id;
	rectangle_2d->position.x = 0;
	rectangle_2d->position.y = 0;
	rectangle_2d->dimensions.x = w;
	rectangle_2d->dimensions.y = h;

	float w_by_2 = w / 2.0f;
	float h_by_2 = h / 2.0f;
	float vertices[] = {
		-w_by_2, +h_by_2, 0, 1,
		+w_by_2, +h_by_2, 1, 1,
		-w_by_2, -h_by_2, 0, 0,
		+w_by_2, +h_by_2, 1, 1,
		-w_by_2, -h_by_2, 0, 0,
		+w_by_2, -h_by_2, 1, 0,
	};

	glGenVertexArrays(1, &rectangle_2d->vao);
	glGenBuffers(1, &rectangle_2d->vbo);
	glBindVertexArray(rectangle_2d->vao);
	glBindBuffer(GL_ARRAY_BUFFER, rectangle_2d->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_TRUE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void translate_rectangle_2d(Rectangle2D *rct, float x, float y) {
	rct->position.x = x;
	rct->position.y = y;
}

void draw_rectangle_2d(Rectangle2D *rectangle_2d) {
	glUseProgram(rectangle_2d->program);
	set_vector2(rectangle_2d->program, "pos", &rectangle_2d->position);

	glBindTexture(GL_TEXTURE_2D, rectangle_2d->texture_id);
	glBindVertexArray(rectangle_2d->vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}
