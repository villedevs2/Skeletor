#version 120

uniform mat4			m_vp_matrix;
uniform mat4			m_matrix;
uniform mat4			m_v_matrix;
varying vec4			v_pos;
varying vec3			v_normal;
varying vec2			v_texcoord;
varying vec3			v_light;
attribute vec4			a_position;
attribute vec3			a_normal;
attribute vec2			a_texcoord;
uniform vec3			u_light;

void main()
{
	// transform to world coords
	v_pos = m_matrix * a_position;
	//v_normal = normal_matrix * a_normal;
	


	//v_normal = mat3(m_matrix) * a_normal;

	v_normal = mat3(m_v_matrix) * a_normal;
	v_light = normalize(u_light);	
	v_texcoord = a_texcoord;

	// transform to view coords
	gl_Position = m_vp_matrix * v_pos;
}