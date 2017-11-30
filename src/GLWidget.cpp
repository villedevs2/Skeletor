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
	update();
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

	m_mesh->render(&m_default_shader_context, m_anim_time);



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
	
	repaint();
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
	m_lastmousepos = event->globalPos();
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
	}
}