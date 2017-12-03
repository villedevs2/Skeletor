#pragma once

typedef struct
{
	int position;
	int tex_coord;
	int normal;
	int diff_sampler;
	int bump_sampler;
	int matrix;
	int vp_matrix;
	int v_matrix;
	int light_pos;
	int light_diff_color;
	int light_spec_color;
	int material_diff_color;
	int material_spec_color;
	int material_shininess;
	int light;
} DefaultShaderContext;