#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "../../src/math.h"

#define MAX_COUNT 25

typedef struct ShaderInfos {
	uint32_t count;
	char variable_name[MAX_COUNT][50];
	char vertex_shader_filename[MAX_COUNT][50];
	char fragment_shader_filename[MAX_COUNT][50];
} ShaderInfos;

typedef struct TextureInfos {
	uint32_t count;
	char variable_name[MAX_COUNT][50];
	char filename[MAX_COUNT][50];
} TextureInfos;

typedef struct FontInfos {
	uint32_t count;
	char variable_name[MAX_COUNT][50];
	char filename[MAX_COUNT][50];
} FontInfos;

typedef struct CameraInfo {
	Vector3 front;
	Vector3 position;
} CameraInfo;

typedef struct MetaInfo {
	ShaderInfos shader_infos;
	TextureInfos texture_infos;
	FontInfos font_infos;
	CameraInfo camera_info;
} MetaInfo;

void write_meta(MetaInfo *meta_info);
void read_data(MetaInfo *meta_info);

int main() {
	MetaInfo meta_info;
	meta_info.shader_infos.count = 0;
	meta_info.texture_infos.count = 0;
	meta_info.font_infos.count = 0;

	read_data(&meta_info);
	write_meta(&meta_info);
	
	printf("Ended.\n\n");
	return 0;
}

void write_meta(MetaInfo *meta_info) {
	FILE *file = fopen("../src/meta_output.c", "w");

	/* INCLUDE FILES */
	fprintf(file, "%s", "#include \"rectangle_2d.h\"\n");
	fprintf(file, "%s", "#include \"text.h\"\n");
	fprintf(file, "%s", "#include \"../../src/shader.h\"\n");
	fprintf(file, "%s", "#include \"../../src/utils.h\"\n");
	fprintf(file, "%s", "#include \"../../src/math.h\"\n");
	fprintf(file, "%s", "\n");
	/* INCLUDE FILES */

	/* EXTERN DECLARATIONS */
	fprintf(file, "extern char* assets_path;\n");
	fprintf(file, "extern char* shaders_path;\n");
	fprintf(file, "extern uint16_t window_width;\n");
	fprintf(file, "extern uint16_t window_height;\n");
	fprintf(file, "extern Matrix4 view;\n");
	fprintf(file, "extern Matrix4 projection;\n");
	fprintf(file, "extern Matrix4 text_projection;\n");
	fprintf(file, "extern Vector3 front;\n");
	fprintf(file, "extern Vector3 position;\n");
	fprintf(file, "extern Vector3 up;\n");
	for(uint32_t i = 0; i < meta_info->shader_infos.count; ++i)
		fprintf(file, "%s %s;\n", "extern GLuint", meta_info->shader_infos.variable_name[i]);
	for(uint32_t i = 0; i < meta_info->texture_infos.count; ++i)
		fprintf(file, "%s %s;\n", "extern GLuint", meta_info->texture_infos.variable_name[i]);
	for(uint32_t i = 0; i < meta_info->font_infos.count; ++i)
		fprintf(file, "%s %s;\n", "extern Font", meta_info->font_infos.variable_name[i]);
	/* EXTERN DECLARATIONS */

	/* LOAD FUNCTION */
	fprintf(file, "\n");
	fprintf(file, "%s\n", "void load() {");

	// SHADERS
	for(uint32_t i = 0; i < meta_info->shader_infos.count; ++i) {
		fprintf(file, "\t%s = compile_shader(combine_string(shaders_path, \"%s\"), combine_string(shaders_path, \"%s\"));\n", meta_info->shader_infos.variable_name[i], meta_info->shader_infos.vertex_shader_filename[i], meta_info->shader_infos.fragment_shader_filename[i]);
	}
	fprintf(file, "\n");

	// TEXTURES
	for(uint32_t i = 0; i < meta_info->texture_infos.count; ++i) {
		fprintf(file, "\t%s = make_texture(combine_string(assets_path, \"%s\"));\n", meta_info->texture_infos.variable_name[i], meta_info->texture_infos.filename[i]);
	}
	fprintf(file, "\n");

	// FONTS
	for(uint32_t i = 0; i < meta_info->font_infos.count; ++i) {
		fprintf(file, "\tFT_Library ft_%d;\n", i + 1); 
		fprintf(file, "\tinit_freetype(&ft_%d);\n", i + 1); 
		fprintf(file, "\tinit_font(&%s, combine_string(assets_path, \"%s\"), &ft_%d);\n\n", meta_info->font_infos.variable_name[i], meta_info->font_infos.filename[i], i + 1);
	}

	// CAMERA FRONT & POSITION
	fprintf(file, "\tinit_vector(&front, %.3f, %.3f, %.3f);\n", meta_info->camera_info.front.x, meta_info->camera_info.front.y, meta_info->camera_info.front.z);
	fprintf(file, "\tinit_vector(&position, %.3f, %.3f, %.3f);\n", meta_info->camera_info.position.x, meta_info->camera_info.position.y, meta_info->camera_info.position.z);
	fprintf(file, "\tinit_vector(&up, 0, 1, 0);\n");

	// PROJECTIONS
	fprintf(file, "\tmake_identity(&view);");
	fprintf(file, "\tprojection = perspective(45.0f, (float)window_width / window_height, 0.1f, 500.0f);");
	fprintf(file, "\ttext_projection = ortho(0, window_width, 0, window_height);");
	
	fprintf(file, "%s\n", "}");
	/* LOAD FUNCTION */

	fprintf(file, "\n");
	fclose(file);
}

void read_data(MetaInfo *meta_info) {
	FILE *file = fopen("../data_out/data.variables", "r");
	if(!file) {
		printf("ERROR read_data(): file(%s) not found.\n", "../data_out/data.variables");
	}
	else {
		char variable_type[50];
		while(fscanf(file, "%s", variable_type) != EOF) {
			if(strcmp(variable_type, "SHADER") == 0) {
				uint32_t index = meta_info->shader_infos.count;
				fscanf(file, "%s", meta_info->shader_infos.variable_name[index]);
				fscanf(file, "%s", meta_info->shader_infos.vertex_shader_filename[index]);
				fscanf(file, "%s", meta_info->shader_infos.fragment_shader_filename[index]);
				
				meta_info->shader_infos.count += 1;
			}
			else if(strcmp(variable_type, "TEXTURE") == 0) {
				uint32_t index = meta_info->texture_infos.count;
				fscanf(file, "%s", meta_info->texture_infos.variable_name[index]);
				fscanf(file, "%s", meta_info->texture_infos.filename[index]);

				meta_info->texture_infos.count += 1;
			}
			else if(strcmp(variable_type, "FONT") == 0) {
				uint32_t index = meta_info->font_infos.count;
				fscanf(file, "%s", meta_info->font_infos.variable_name[index]);
				fscanf(file, "%s", meta_info->font_infos.filename[index]);

				meta_info->font_infos.count += 1;
			}
			else if(strcmp(variable_type, "CAMERA_FRONT") == 0) {
				fscanf(file, "%f %f %f", &meta_info->camera_info.front.x, &meta_info->camera_info.front.y, &meta_info->camera_info.front.z);
			}
			else if(strcmp(variable_type, "CAMERA_POSITION") == 0) {
				fscanf(file, "%f %f %f", &meta_info->camera_info.position.x, &meta_info->camera_info.position.y, &meta_info->camera_info.position.z);
			}
		}

		fclose(file);
	}
}
