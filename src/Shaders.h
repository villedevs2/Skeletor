#pragma once

#include <gl/glew.h>

typedef struct
{
	GLuint position;
	GLuint tex_coord;
	GLuint normal;
	GLuint diff_sampler;
	GLuint bump_sampler;
	GLuint matrix;
	GLuint vp_matrix;
	GLuint light_pos;
	GLuint light_diff_color;
	GLuint light_spec_color;
	GLuint material_diff_color;
	GLuint material_spec_color;
	GLuint material_shininess;
} DefaultShaderContext;