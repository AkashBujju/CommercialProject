#include <glad/glad.h>
#include <stdio.h>
#include <string.h>
#include "text.h"
#include "shader.h"

void init_freetype(FT_Library *ft) {
	if(FT_Init_FreeType(ft))
		printf("Could not init FreeType library\n");
}

void init_font(Font *font, uint16_t font_size, const char* filepath, FT_Library *ft) {
	FT_Face face;
	if(FT_New_Face(*ft, filepath, 0, &face)) {
		printf("Could not load font %s\n", filepath);
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, font_size);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for(unsigned int i = 0; i < 128; ++i) {
		if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
			printf("Failed to load glyph for %d\n", i);
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RED,
			face->glyph->bitmap.width, face->glyph->bitmap.rows,
			0, GL_RED,
			GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		font->characters[i].texture_id = texture;
		init_vector2(&font->characters[i].size, face->glyph->bitmap.width, face->glyph->bitmap.rows);
		init_vector2(&font->characters[i].bearing, face->glyph->bitmap_left, face->glyph->bitmap_top);
		font->characters[i].advance = face->glyph->advance.x;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(*ft);

	glGenVertexArrays(1, &font->vao);
	glGenBuffers(1, &font->vbo);
	glBindVertexArray(font->vao);
	glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Vector2 get_char_offset(Font *font, float scale, const char* text, int16_t index) {
	int text_length = -1;
	while(text[++text_length] != '\0'){}

	if(index >= text_length || index < 0) {
		printf("WARNING get_char_pos(): index(%d) && text_length(%d). Setting pos to default.\n", index, text_length);
		Vector2 default_pos = { 0, 0 };
		return default_pos;
	}

	Vector2 offset;
	float width = 0, height;
	for(int i = 0; i <= index; ++i) {
		int ascii = (int)text[i];
		Character *character = &font->characters[ascii];
		GLfloat w = character->size.x * scale;
		width += w;
		width += (character->advance >> 6) * scale;
	}
	offset.x = width;

	float total_height = 0;
	for(int i = 0; i < text_length; ++i) {
		int ascii = (int)text[i];
		Character *character = &font->characters[ascii];
		GLfloat h = character->size.y * scale;
		total_height += h;
	}
	offset.y = total_height / text_length;

	return offset;
}

float get_text_height(Font *font, float scale, const char* text) {
	int text_length = -1;
	while(text[++text_length] != '\0'){}

	if(text_length == 0)
		return 0;

	float total_height = 0;
	for(int i = 0; i < text_length; ++i) {
		int ascii = (int)text[i];
		Character *character = &font->characters[ascii];
		GLfloat h = character->size.y * scale;
		total_height += h;
	}

	return total_height / text_length;
}

float get_text_width(Font *font, float scale, const char* text) {
	int text_length = -1;
	while(text[++text_length] != '\0'){}

	if(text_length == 0)
		return 0;

	float width = 0;
	for(int i = 0; i < text_length; ++i) {
		int ascii = (int)text[i];
		Character *character = &font->characters[ascii];
		GLfloat w = character->size.x * scale;
		width += w;
		width += (character->advance >> 6) * scale;
	}

	return width;
}

void render_text(Font *font, unsigned int program, const char* text, float x, float y, float scale, float r, float g, float b) {
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);

	int text_length = -1;
	while(text[++text_length] != '\0'){}

	Vector3 color;
	init_vector(&color, r, g, b);

	glUseProgram(program);
	set_vector3(program, "textColor", &color);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(font->vao);

	float tmp_x = x;
	for(int i = 0; i < text_length; ++i) {
		int ascii = (int)text[i];
		Character *character = &font->characters[ascii];

		GLfloat xpos = tmp_x + character->bearing.x * scale;
		GLfloat ypos = y - (character->size.y - character->bearing.y) * scale;
		GLfloat w = character->size.x * scale;
		GLfloat h = character->size.y * scale;

		GLfloat vertices[6][4] = {
			{ xpos, ypos + h, 0.0, 0.0 },          
			{ xpos, ypos, 0.0, 1.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos, ypos + h, 0.0, 0.0 },
			{ xpos + w, ypos, 1.0, 1.0 },
			{ xpos + w, ypos + h, 1.0, 0.0 }           
		};

		glBindTexture(GL_TEXTURE_2D, character->texture_id);
		glBindBuffer(GL_ARRAY_BUFFER, font->vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		tmp_x += (character->advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}
