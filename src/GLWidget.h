#pragma once

#include <QtGui>
#include <qopenglfunctions.h>
#include <QGLWidget>
#include <qglshaderprogram.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "Shaders.h"
#include "Mesh.h"







class GLWidget : public QGLWidget, private QOpenGLFunctions
{
	Q_OBJECT

public:
	GLWidget(QWidget *parent, Mesh *mesh);
	~GLWidget(void);

	QSize minimumSizeHint() const;
	QSize sizeHint() const;

public slots:
	void setCameraAngle(int);
	void setCameraHeight(int);
	void animate();
	void setAnimTime(int);
	void bake();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);
	void wheelEvent(QWheelEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 uvcoord;
		glm::vec3 normal;
	};

	struct BakedSubMesh
	{
		int vbo_index;
		int num_verts;
	};

	void loadTexture(std::string filename, GLuint texid);
	void renderMesh(DefaultShaderContext* context, int time);

	QColor m_bgcolor;

	//GLuint m_default_shader;
	//GLuint m_grid_shader;

	DefaultShaderContext m_default_shader_context;
	DefaultShaderContext m_grid_shader_context;

	QGLShaderProgram* m_standard_program;
	QGLShaderProgram* m_grid_program;

	float m_viewport_width;
	float m_viewport_height;
	float m_viewport_aspect;

	float m_camera_angle;
	float m_camera_height;

	std::string m_error_string;

	Mesh *m_mesh;

	float m_zoom;
	QPoint m_lastmousepos;

	float m_angle_x;
	float m_angle_y;

	int m_anim_time;

	vector<BakedSubMesh> m_baked_submesh;
	vector<GLuint> m_textures;
	GLuint m_vbo;
};
