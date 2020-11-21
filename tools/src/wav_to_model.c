#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#define BUFFER_SIZE 32768
#define ARRAY_SIZE 8192

typedef struct Vector3 {
	float a, b, c;
} Vector3;

typedef struct Vector2 {
	float a, b;
} Vector2;

typedef struct Info {
	uint16_t vertex_numbers[4];
	uint16_t normal_numbers[4];
	uint16_t len; /* len may wary from 1 to 4 */
} Info;

typedef struct Strings {
	char strings[10][128];
	uint16_t len;
} Strings;

typedef struct SplitStrings {
	Strings *strings;
	uint32_t len;
} SplitStrings;

typedef struct Data {
	Vector3 vertices[ARRAY_SIZE];
	Vector3 vertex_normals[ARRAY_SIZE];
	Info infos[ARRAY_SIZE];
	uint32_t v_len;
	uint32_t n_len;
	uint32_t i_len;
} Data;

void pack_into_structs(Data *data, SplitStrings *split_strs);
void save_model(Data*, const char*);

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("Second argument ie. the filename is missing!!\n");
	}
	else {
		const size_t line_sz = 256;
		char buffer[line_sz];
		FILE *file = NULL;
		file = fopen(argv[1], "r");

		if(file == NULL) {
			printf("Could not open file %s\n", argv[1]);
		}
		else {
			fseek(file, 0, SEEK_END);
			const size_t filesize = ftell(file);
			fseek(file, 0, SEEK_SET);

			SplitStrings split_strs;
			split_strs.len = 0;
			split_strs.strings = (Strings*)malloc(sizeof(Strings) * BUFFER_SIZE);

			printf("-- Parsing file '%s' of size: %zu bytes\n", argv[1], filesize);
			while(fgets(buffer, line_sz, file)) {
				/* Deleting the newline character */
				const size_t len = strlen(buffer);
				if(buffer[len - 1] == '\n')
					buffer[len - 1] = '\0';
				/* Deleting the newline character */

				/* Split string and Copy*/
				uint16_t str_index = 0;
				char* pch = strtok(buffer, " ");
				while(pch != NULL) {
					strcpy(split_strs.strings[split_strs.len].strings[str_index++], pch);
					/* @Note: How is this not crashing for cone.obj, for array size > 10 */
					pch = strtok(NULL, " ");
				}
				/* Split string and Copy*/

				split_strs.strings[split_strs.len].len = str_index;
				split_strs.len += 1;
			}

			Data data;
			pack_into_structs(&data, &split_strs);
			free(split_strs.strings);

			/* Saving */
			char* pch = strtok(argv[1], ".");
#ifdef __APPLE__
			pch = strtok(pch, "/");
			pch = strtok(NULL, "/");
			char filename[50] = "../data_out/";
#else
			pch = strtok(pch, "\\");
			pch = strtok(NULL, "\\");
			char filename[50] = "..\\data_out\\";
#endif
			strcat(filename, pch);
			strcat(filename, ".model");
			save_model(&data, filename);
			/* Saving */
		}

		fclose(file);
	}

	printf("-- ENDED\n\n");
	return 0;
}

void pack_into_structs(Data *data, SplitStrings *split_strs) {
	data->v_len = 0;
	data->n_len = 0;
	data->i_len = 0;

	for(uint32_t i = 0; i < split_strs->len; ++i) {
		char* str = split_strs->strings[i].strings[0];
		if(strcmp(str, "v") == 0) {
			float f1 = strtof(split_strs->strings[i].strings[1], NULL);
			float f2 = strtof(split_strs->strings[i].strings[2], NULL);
			float f3 = strtof(split_strs->strings[i].strings[3], NULL);
			data->vertices[data->v_len].a = f1;
			data->vertices[data->v_len].b = f2;
			data->vertices[data->v_len].c = f3;
			data->v_len += 1;
		}
		else if(strcmp(str, "vn") == 0) {
			float f1 = strtof(split_strs->strings[i].strings[1], NULL);
			float f2 = strtof(split_strs->strings[i].strings[2], NULL);
			float f3 = strtof(split_strs->strings[i].strings[3], NULL);
			data->vertex_normals[data->n_len].a = f1;
			data->vertex_normals[data->n_len].b = f2;
			data->vertex_normals[data->n_len].c = f3;
			data->n_len += 1;
		}
		else if(strcmp(str, "f") == 0) {
			data->infos[data->i_len].len = split_strs->strings[i].len - 1;
			for(uint16_t j = 1; j < split_strs->strings[i].len; ++j) {
				/* Split string */
				char *s = split_strs->strings[i].strings[j];
				char* s1 = strtok(s, "/");
				char* s2 = strtok(NULL, "/");
				char* s3 = strtok(NULL, "/");

				if(s3 == NULL) {
					s3 = s2;
					s2 = NULL;
				}

				uint16_t vertex_index = strtof(s1, NULL);
				uint16_t normals_index = strtof(s3, NULL);
				data->infos[data->i_len].vertex_numbers[j - 1] = vertex_index;
				data->infos[data->i_len].normal_numbers[j - 1] = normals_index;
				/* Split string */
			}
			data->i_len += 1;
		}
	}
}

void save_model(Data *data, const char* filename) {
	uint32_t num_floats = 0;

	/* Calculating number of floats */
	/* @Note: 3 for the co-ordinates and 3 for the normals := 6 */
	for(uint16_t i = 0; i < data->i_len; ++i) {
		if(data->infos[i].len == 4)
			num_floats += (data->infos[i].len + 2) * 6;
		else
			num_floats += data->infos[i].len	* 6;
	}
	/* Calculating number of floats */

	FILE *file = fopen(filename, "w");
	if(!file) {
		printf("File %s not found\n", filename);
	}
	else {
		fprintf(file, "%u", num_floats);

		for(uint32_t i = 0; i < data->i_len; ++i) {
			if(data->infos[i].len == 4) {
				uint16_t vi_1 = data->infos[i].vertex_numbers[0] - 1; 
				uint16_t vi_2 = data->infos[i].vertex_numbers[1] - 1;
				uint16_t vi_3 = data->infos[i].vertex_numbers[2] - 1;
				uint16_t vi_4 = data->infos[i].vertex_numbers[3] - 1;
				uint16_t ni_1 = data->infos[i].normal_numbers[0] - 1; 
				uint16_t ni_2 = data->infos[i].normal_numbers[1] - 1;
				uint16_t ni_3 = data->infos[i].normal_numbers[2] - 1;
				uint16_t ni_4 = data->infos[i].normal_numbers[3] - 1;
				Vector3 *v1 = &data->vertices[vi_1];
				Vector3 *v2 = &data->vertices[vi_2];
				Vector3 *v3 = &data->vertices[vi_3];
				Vector3 *v4 = &data->vertices[vi_4];
				Vector3 *n1 = &data->vertex_normals[ni_1];
				Vector3 *n2 = &data->vertex_normals[ni_2];
				Vector3 *n3 = &data->vertex_normals[ni_3];
				Vector3 *n4 = &data->vertex_normals[ni_4];

				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v1->a, v1->b, v1->c, n1->a, n1->b, n1->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v2->a, v2->b, v2->c, n2->a, n2->b, n2->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v3->a, v3->b, v3->c, n3->a, n3->b, n3->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v1->a, v1->b, v1->c, n1->a, n1->b, n1->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v4->a, v4->b, v4->c, n4->a, n4->b, n4->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v3->a, v3->b, v3->c, n3->a, n3->b, n3->c);
			}
			else if(data->infos[i].len == 3) {
				uint16_t vi_1 = data->infos[i].vertex_numbers[0] - 1; 
				uint16_t vi_2 = data->infos[i].vertex_numbers[1] - 1;
				uint16_t vi_3 = data->infos[i].vertex_numbers[2] - 1;
				uint16_t ni_1 = data->infos[i].normal_numbers[0] - 1; 
				uint16_t ni_2 = data->infos[i].normal_numbers[1] - 1;
				uint16_t ni_3 = data->infos[i].normal_numbers[2] - 1;
				Vector3 *v1 = &data->vertices[vi_1];
				Vector3 *v2 = &data->vertices[vi_2];
				Vector3 *v3 = &data->vertices[vi_3];
				Vector3 *n1 = &data->vertex_normals[ni_1];
				Vector3 *n2 = &data->vertex_normals[ni_2];
				Vector3 *n3 = &data->vertex_normals[ni_3];

				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v1->a, v1->b, v1->c, n1->a, n1->b, n1->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v2->a, v2->b, v2->c, n2->a, n2->b, n2->c);
				fprintf(file, "\n%+.3f %+.3f %+.3f %+.3f %+.3f %+.3f", v3->a, v3->b, v3->c, n3->a, n3->b, n3->c);
			}
		}

		printf("-- Saved to %s\n", filename);
		fclose(file);
	}
}
