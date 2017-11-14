varying vec4			v_pos;
varying vec2			v_texcoord;
varying vec3			v_normal;
uniform vec4			a_light_pos[4];
uniform vec4			a_light_diff_color[4];
uniform vec4			a_light_spec_color[4];
uniform vec4			a_material_diff_color;
uniform vec4			a_material_spec_color;
uniform float			a_material_shininess;
uniform sampler2D		s_diff_texture;
uniform sampler2D		s_bump_texture;

void main()
{
	//vec4 light_diff = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//vec4 light_spec = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//vec4 material_diff = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//vec4 material_spec = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//float shininess = 50.0f;


	vec3 normal = normalize(v_normal);

	vec4 diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	vec4 specular = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	for (int i=0; i < 4; i++)
	{
		if (a_light_pos[i].w > 0.0f)
		{
			// apply point light
			float dist = length(v_pos.xyz - a_light_pos[i].xyz);
			vec3 lightvec = -normalize(v_pos.xyz - a_light_pos[i].xyz);
			vec3 refl = normalize(-reflect(lightvec, normal));			// reflection vector
			vec3 view = normalize(-v_pos.xyz);
			float att = 1.0f / dist;
			float light_intensity = a_light_pos[i].w;
	
			diffuse += a_light_diff_color[i] * a_material_diff_color * clamp(dot(lightvec, normal), 0.0f, 1.0f) * att * light_intensity;
	
			if (a_material_shininess != 0.0f)
			{
				specular += a_light_spec_color[i] * a_material_spec_color * pow(max(0.0f, dot(refl, view)), a_material_shininess) * att * light_intensity;
			}
		}
	}
	
	//gl_FragColor = (texture2D(s_diff_texture, v_texcoord) * diffuse) + specular;

	gl_FragColor = texture2D(s_diff_texture, v_texcoord);
	//gl_FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}