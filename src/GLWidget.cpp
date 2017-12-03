#include "GLWidget.h"

GLWidget::GLWidget(QWidget *parent, Mesh *mesh)
				   : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	//m_bgcolor = QColor(192, 192, 192, 0);
	//m_bgcolor = QColor(0,0,0,0);
	m_bgcolor = QColor(0,0,128,0);

	m_camera_angle = 0;
	m_camera_height = 0;

	m_mesh = mesh;

	m_zoom = 250.0f;

	m_angle_x = 0.0f;
	m_angle_y = 25.0f;

	m_anim_time = 0;

	m_vbo = NULL;
}

GLWidget::~GLWidget(void)
{
}


QSize GLWidget::minimumSizeHint() const
{
	return QSize(200, 100);
}

QSize GLWidget::sizeHint() const
{
	return QSize(1600, 400);
}


void GLWidget::setCameraAngle(int angle)
{
	m_camera_angle = angle;
	update();
}

void GLWidget::setCameraHeight(int height)
{
	m_camera_height = height / 100.0f;
	update();
}

void GLWidget::animate()
{
	//update();
}

void GLWidget::setAnimTime(int value)
{
	m_anim_time = value;
	update();
}

void GLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	QFile file;
	QByteArray bytes;
	
	file.setFileName("default_vertshader.glsl");
	file.open(QIODevice::ReadOnly);
	bytes = file.readAll();
	std::string default_vs_code(bytes);
	file.close();

	file.setFileName("default_fragshader.glsl");
	file.open(QIODevice::ReadOnly);
	bytes = file.readAll();
	std::string default_fs_code(bytes);
	file.close();

	file.setFileName("grid_fragshader.glsl");
	file.open(QIODevice::ReadOnly);
	bytes = file.readAll();
	std::string grid_fs_code(bytes);
	file.close();

	const  char *vert_code, *frag_code, *grid_frag_code;

	GLuint default_vert_shader = glCreateShader(GL_VERTEX_SHADER);
	vert_code = default_vs_code.c_str();
	glShaderSource(default_vert_shader, 1, &vert_code, NULL);
	glCompileShader(default_vert_shader);

	GLuint default_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	frag_code = default_fs_code.c_str();
	glShaderSource(default_frag_shader, 1, &frag_code, NULL);
	glCompileShader(default_frag_shader);

	GLuint grid_frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	grid_frag_code = grid_fs_code.c_str();
	glShaderSource(grid_frag_shader, 1, &grid_frag_code, NULL);
	glCompileShader(grid_frag_shader);

	m_default_shader = glCreateProgram();
	glAttachShader(m_default_shader, default_vert_shader);
	glAttachShader(m_default_shader, default_frag_shader);
	glLinkProgram(m_default_shader);

	m_default_shader_context.position				= glGetAttribLocation(m_default_shader, "a_position");
	m_default_shader_context.normal					= glGetAttribLocation(m_default_shader, "a_normal");
	m_default_shader_context.tex_coord				= glGetAttribLocation(m_default_shader, "a_texcoord");
	m_default_shader_context.light_pos				= glGetUniformLocation(m_default_shader, "a_light_pos");
	m_default_shader_context.light_diff_color		= glGetUniformLocation(m_default_shader, "a_light_diff_color");
	m_default_shader_context.light_spec_color		= glGetUniformLocation(m_default_shader, "a_light_spec_color");
	m_default_shader_context.material_diff_color	= glGetUniformLocation(m_default_shader, "a_material_diff_color");
	m_default_shader_context.material_spec_color	= glGetUniformLocation(m_default_shader, "a_material_spec_color");
	m_default_shader_context.material_shininess		= glGetUniformLocation(m_default_shader, "a_material_shininess");
	m_default_shader_context.diff_sampler			= glGetUniformLocation(m_default_shader, "s_diff_texture");
	m_default_shader_context.bump_sampler			= glGetUniformLocation(m_default_shader, "s_bump_texture");
	m_default_shader_context.vp_matrix				= glGetUniformLocation(m_default_shader, "m_vp_matrix");
	m_default_shader_context.matrix					= glGetUniformLocation(m_default_shader, "m_matrix");



	m_grid_shader = glCreateProgram();
	glAttachShader(m_grid_shader, default_vert_shader);
	glAttachShader(m_grid_shader, grid_frag_shader);
	glLinkProgram(m_grid_shader);

	m_grid_shader_context.position					= glGetAttribLocation(m_grid_shader, "a_position");
	m_grid_shader_context.normal					= glGetAttribLocation(m_grid_shader, "a_normal");
	m_grid_shader_context.tex_coord					= glGetAttribLocation(m_grid_shader, "a_texcoord");
	m_grid_shader_context.vp_matrix					= glGetUniformLocation(m_grid_shader, "m_vp_matrix");
	m_grid_shader_context.matrix					= glGetUniformLocation(m_grid_shader, "m_matrix");


	glEnable(GL_CULL_FACE);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
}

void GLWidget::paintGL()
{
	QPainter painter;
	painter.begin(this);

	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	qglClearColor(m_bgcolor);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	const float grid[36*3] =
	{
		 -80.0f,  0.0f, -80.0f,		 -80.0f,  0.0f,  80.0f,
		 -60.0f,  0.0f, -80.0f,		 -60.0f,  0.0f,  80.0f,
		 -40.0f,  0.0f, -80.0f,		 -40.0f,  0.0f,  80.0f,
		 -20.0f,  0.0f, -80.0f,		 -20.0f,  0.0f,  80.0f,
		  00.0f,  0.0f, -80.0f,		  00.0f,  0.0f,  80.0f,
		  20.0f,  0.0f, -80.0f,		  20.0f,  0.0f,  80.0f,
		  40.0f,  0.0f, -80.0f,		  40.0f,  0.0f,  80.0f,
		  60.0f,  0.0f, -80.0f,		  60.0f,  0.0f,  80.0f,
		  80.0f,  0.0f, -80.0f,		  80.0f,  0.0f,  80.0f,
	    
	     -80.0f,  0.0f, -80.0f,		  80.0f,  0.0f, -80.0f,
		 -80.0f,  0.0f, -60.0f,		  80.0f,  0.0f, -60.0f,
		 -80.0f,  0.0f, -40.0f,		  80.0f,  0.0f, -40.0f,
		 -80.0f,  0.0f, -20.0f,		  80.0f,  0.0f, -20.0f,
		 -80.0f,  0.0f,  00.0f,		  80.0f,  0.0f,  00.0f,
		 -80.0f,  0.0f,  20.0f,		  80.0f,  0.0f,  20.0f,
		 -80.0f,  0.0f,  40.0f,		  80.0f,  0.0f,  40.0f,
		 -80.0f,  0.0f,  60.0f,		  80.0f,  0.0f,  60.0f,
		 -80.0f,  0.0f,  80.0f,		  80.0f,  0.0f,  80.0f,
	};
	*/

	const int gridx = 21;
	const int gridy = 21;

	float grid[(gridx+gridy)*6];

	int gi = 0;
	for (int i=0; i < gridx; i++)
	{
		float x = (i - (gridx/2)) * 20.0f;
		grid[gi++] = x;
		grid[gi++] = 0.0f;
		grid[gi++] = -(gridx/2) * 20.0f;
		grid[gi++] = x;
		grid[gi++] = 0.0f;
		grid[gi++] = (gridx/2) * 20.0f;
	}
	for (int i=0; i < gridy; i++)
	{
		float y = (i - (gridy/2)) * 20.0f;
		grid[gi++] = -(gridx/2) * 20.0f;
		grid[gi++] = 0.0f;
		grid[gi++] = y;
		grid[gi++] = (gridx/2) * 20.0f;
		grid[gi++] = 0.0f;
		grid[gi++] = y;
	}


	double fov;
	float size, ratio;

	glm::mat4 camera_proj_matrix;
	glm::mat4 camera_view_matrix;
	glm::mat4 camera_vp_matrix;

	// setup camera matrix
	if (m_viewport_width > m_viewport_height)
		fov = 105.0;
	else
		fov = 75.0;

	size = 0.01f * (float)tan((fov * M_PI / 180.0) / 2); 
	ratio = m_viewport_aspect;



	glm::vec3 vec = glm::vec3(0, 0, m_zoom);

	glm::vec3 vec2 = glm::rotate(vec, m_angle_y, glm::vec3(1,0,0));
	glm::vec3 pos = glm::rotate(vec2, m_angle_x, glm::vec3(0,1,0));
	glm::vec3 eye = glm::vec3(0.0f, 0.0f, 0.0f);

	camera_proj_matrix = glm::frustum<float>(-size, size, -size/ratio, size/ratio, 0.01f, 10000.0f);
	camera_view_matrix = glm::lookAt(pos, eye, glm::vec3(0,1,0));
	camera_vp_matrix = camera_proj_matrix * camera_view_matrix;



	// render meshes
	glUseProgram(m_default_shader);

	glUniformMatrix4fv(m_default_shader_context.vp_matrix, 1, false, glm::value_ptr(camera_vp_matrix));
	glUniform1i(m_default_shader_context.diff_sampler, 0);
	glUniform1i(m_default_shader_context.bump_sampler, 1);

	//m_mesh->render(&m_default_shader_context, m_anim_time);
	renderMesh(&m_default_shader_context, m_anim_time);



	// draw grid
	glUseProgram(m_grid_shader);

	glUniformMatrix4fv(m_grid_shader_context.vp_matrix, 1, false, glm::value_ptr(camera_vp_matrix));
	glUniform1i(m_grid_shader_context.diff_sampler, 0);
	glUniform1i(m_grid_shader_context.bump_sampler, 1);

	glm::mat4 mat = glm::mat4();
	glUniformMatrix4fv(m_grid_shader_context.matrix, 1, false, glm::value_ptr(mat));

	glVertexAttribPointer(m_grid_shader_context.position, 3, GL_FLOAT, false, 12, grid);
	glEnableVertexAttribArray(m_grid_shader_context.position);
	glDisableVertexAttribArray(m_grid_shader_context.tex_coord);
	glDrawArrays(GL_LINES, 0, (gridx+gridy)*2);



	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		char str[100];
		sprintf(str, "ERROR: %d", error);
		m_error_string = str;
	}

	if (m_error_string.length() > 0)
	{
		painter.setPen(QColor(0, 0, 0));
		painter.drawText(9, 17, m_error_string.c_str());
		painter.setPen(QColor(255, 255, 255));
		painter.drawText(8, 16, m_error_string.c_str());
	}

	{
		char str[100];
		sprintf(str, "X: %f, Y: %f", m_angle_x, m_angle_y);
		painter.setPen(QColor(0, 0, 0));
		painter.drawText(9, 37, str);
		painter.setPen(QColor(255, 255, 255));
		painter.drawText(8, 36, str);
	}


	painter.end();
}

void GLWidget::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	glScissor(0, 0, width, height);

	m_viewport_width = width;
	m_viewport_height = height;
	m_viewport_aspect = m_viewport_width / m_viewport_height;
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
	m_zoom -= (float)(event->delta()) * 0.05f;
	if (m_zoom < 1.0f) m_zoom = 1.0f;
	
	update();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	m_lastmousepos = event->globalPos();
	update();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		int dx = event->globalX() - m_lastmousepos.x();
		int dy = event->globalY() - m_lastmousepos.y();

		m_angle_x -= float(dx) * 0.001f;
		m_angle_y -= float(dy) * 0.001f;
	//	if (m_angle_y < -89.0f) m_angle_y = -89.0f;
	//	if (m_angle_y > 89.0f) m_angle_y = 89.0f;

		m_lastmousepos = event->globalPos();
		update();
	}
}


void GLWidget::loadTexture(std::string filename, GLuint texid)
{
	// strip path
	size_t p;
	p = filename.find_last_of("\\");
	if (p == string::npos)
	{
		p = filename.find_last_of("/");
	}

	std::string path_stripped = filename.substr(p + 1, string::npos);

	// extract extension
	p = path_stripped.find_last_of(".");

	std::string extension = path_stripped.substr(p + 1, string::npos);

	//if (extension.compare("png") == 0)
	{
		QImage *image = new QImage();
		bool result = image->load(filename.c_str());

		if (result)
		{
			int width = image->width();
			int height = image->height();

			char *pixels = new char[width * height * 4];

			int index = 0;
			for (int j = 0; j < height; j++)
			{
				QRgb *scan = (QRgb*)image->scanLine(j);

				for (int i = 0; i < width; i++)
				{
					int r = qRed(scan[i]);
					int g = qGreen(scan[i]);
					int b = qBlue(scan[i]);
					int a = qAlpha(scan[i]);

					pixels[index + 0] = r;
					pixels[index + 1] = g;
					pixels[index + 2] = b;
					pixels[index + 3] = a;
					index += 4;
				}
			}

			glBindTexture(GL_TEXTURE_2D, texid);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexImage2D(GL_TEXTURE_2D,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				pixels);

			delete[] pixels;
		}
	}
}



void GLWidget::bake()
{
	if (m_vbo != NULL)
		glDeleteBuffers(1, &m_vbo);

	glGenBuffers(1, &m_vbo);

	// clear textures
	for (int i=0; i < m_textures.size(); i++)
	{
		GLuint texname = m_textures[i];
		if (texname != NULL)
			glDeleteTextures(1, &texname);
	}
	m_textures.clear();


	m_baked_submesh.clear();

	int num_submeshes = m_mesh->getNumSubMeshes();

	int tri_count = 0;

	// count triangles
	for (int i=0; i < num_submeshes; i++)
	{
		Mesh::SubMesh& submesh = m_mesh->getSubMesh(i);
		tri_count += submesh.tris.size();
	}

	Vertex* tmpvtx = new Vertex[tri_count * 3];

	int vbo_index = 0;

	for (int i=0; i < num_submeshes; i++)
	{
		Mesh::SubMesh& submesh = m_mesh->getSubMesh(i);
		BakedSubMesh baked;

		int numtris = submesh.tris.size();

		baked.vbo_index = vbo_index;
		baked.num_verts = numtris * 3;

		for (int tri=0; tri < numtris; tri++)
		{
			int p0 = submesh.tris[tri].p[0];
			int p1 = submesh.tris[tri].p[1];
			int p2 = submesh.tris[tri].p[2];
			int uv0 = submesh.tris[tri].uv[0];
			int uv1 = submesh.tris[tri].uv[1];
			int uv2 = submesh.tris[tri].uv[2];

			tmpvtx[vbo_index+0].position = submesh.position[p0];
			tmpvtx[vbo_index+1].position = submesh.position[p1];
			tmpvtx[vbo_index+2].position = submesh.position[p2];
			tmpvtx[vbo_index+0].uvcoord = submesh.uvcoord[uv0];
			tmpvtx[vbo_index+1].uvcoord = submesh.uvcoord[uv1];
			tmpvtx[vbo_index+2].uvcoord = submesh.uvcoord[uv2];
	//		tmpvtx[vbo_index+0].normal = m_submesh[i].normal[v0];
	//		tmpvtx[vbo_index+1].normal = m_submesh[i].normal[v1];
	//		tmpvtx[vbo_index+2].normal = m_submesh[i].normal[v2];

			vbo_index += 3;
		}

		GLuint texname;
		glGenTextures(1, &texname);
		m_textures.push_back(texname);

		loadTexture(submesh.texname, texname);

		m_baked_submesh.push_back(baked);
	}

	GLsizeiptr size = vbo_index * sizeof(Vertex) + 4;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, size, tmpvtx, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	delete [] tmpvtx;
}


void GLWidget::renderMesh(DefaultShaderContext* context, int time)
{
	if (m_vbo == NULL)
		return;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glVertexAttribPointer(context->position, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(context->position);
	glVertexAttribPointer(context->tex_coord, 2, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, uvcoord));
	glEnableVertexAttribArray(context->tex_coord);
	//	glVertexAttribPointer(context->normal, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, norm));
	//	glEnableVertexAttribArray(context->normal);

	for (int i=0; i < (int)(m_baked_submesh.size()); i++)
	{
//		Mesh::SubMesh& sb = m_submesh.at(i);

		//if (m_texture)
		//	m_texture->apply(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);

		glm::mat4 mat = glm::mat4();

		//mat *= m_submesh[i].matrix;

		/*
		// animate
		glm::mat4 anim_transform = glm::mat4();

		int poskey = 0;
		int rotkey = 0;
		int scalekey = 0;

		// find keyframes
		findKeyframe(sb, time, &poskey, &rotkey, &scalekey);

		float pos_frame_lerp, rot_frame_lerp, scale_frame_lerp;

		// calculate animation interpolation
		{
			float frame_phase = (float)(time - sb.pos_keys[poskey].time);
			float frame_time = (float)(sb.pos_keys[poskey+1].time - sb.pos_keys[poskey].time);
			pos_frame_lerp = frame_phase / frame_time;
		}
		{
			float frame_phase = (float)(time - sb.rot_keys[rotkey].time);
			float frame_time = (float)(sb.rot_keys[rotkey+1].time - sb.rot_keys[rotkey].time);
			rot_frame_lerp = frame_phase / frame_time;
		}
		{
			float frame_phase = (float)(time - sb.scl_keys[scalekey].time);
			float frame_time = (float)(sb.scl_keys[scalekey+1].time - sb.scl_keys[scalekey].time);
			scale_frame_lerp = frame_phase / frame_time;
		}

		// interpolate position
		glm::vec3 position = glm::mix(sb.pos_keys[poskey].pos, sb.pos_keys[poskey+1].pos, pos_frame_lerp);

		// interpolate rotation
		glm::quat rotation = glm::slerp(sb.rot_keys[rotkey].rot, sb.rot_keys[rotkey+1].rot, rot_frame_lerp);

		// interpolate scale
		glm::vec3 scale = glm::mix(sb.scl_keys[scalekey].scale, sb.scl_keys[scalekey+1].scale, scale_frame_lerp);

		// build transformation matrix
		anim_transform *= glm::scale(glm::mat4(), scale);
		anim_transform *= glm::translate(glm::mat4(), position);
		anim_transform *= glm::mat4_cast(rotation);

		// apply
		mat *= anim_transform;
		*/

		glUniformMatrix4fv(context->matrix, 1, false, glm::value_ptr(mat));
		glDrawArrays(GL_TRIANGLES, m_baked_submesh[i].vbo_index, m_baked_submesh[i].num_verts);
	}

	glDisableVertexAttribArray(context->position);
	glDisableVertexAttribArray(context->tex_coord);
	//	glDisableVertexAttribArray(context->normal);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	// TODO REMOVE
}