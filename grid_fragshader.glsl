varying vec4			v_pos;
varying vec2			v_texcoord;
varying vec3			v_normal;
uniform sampler2D		s_diff_texture;
uniform sampler2D		s_bump_texture;

void main()
{
	gl_FragColor = vec4(0.33f, 0.33f, 0.66f, 1.0f);
}