uniform mat4			m_vp_matrix;
uniform mat4			m_matrix;
varying vec4			v_pos;
varying vec3			v_normal;
varying vec2			v_texcoord;
attribute vec4			a_position;
attribute vec3			a_normal;
attribute vec2			a_texcoord;

void main()
{
	mat3 normal_matrix = m_matrix;

	// transform to world coords
	v_pos = m_matrix * a_position;
	v_normal = normal_matrix * a_normal;
	
	// transform to view coords
	gl_Position = m_vp_matrix * v_pos;
	
	
	v_texcoord = a_texcoord;
}