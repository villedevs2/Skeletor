#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include <vector>
#include <gl/glew.h>
#include "Shaders.h"
#include <QImage>
#include <QImageReader>

using namespace std;

class Mesh
{
public:
	struct Triangle
	{
		int p[3];
		int uv[3];
		int n[3];
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec2 uvcoord;
		glm::vec3 normal;
	};

	struct Keyframe
	{
		int time;
		float value;
	};

	struct PosKeyframe
	{
		int time;
		glm::vec3 pos;
	};

	struct RotKeyframe
	{
		int time;
		glm::quat rot;
	};

	struct ScaleKeyframe
	{
		int time;
		glm::vec3 scale;
	};

	struct FBXKeyframe
	{
		int time;
		glm::vec3 value;
	};

	struct SubMesh
	{
		vector<glm::vec3> position;
		vector<glm::vec2> uvcoord;
		vector<glm::vec3> normal;
		vector<Triangle> tris;
		std::string texname;
		std::string name;
		glm::mat4 matrix;

		vector<PosKeyframe> pos_keys;
		vector<RotKeyframe> rot_keys;
		vector<ScaleKeyframe> scl_keys;
	};

	struct BakedSubMesh
	{
		int vbo_index;
		int num_verts;
	};

	Mesh();
	~Mesh();

	void reset();
	void addSubMesh(Mesh::SubMesh submesh);
	void bake();
	void render(DefaultShaderContext* context, int time);
	void loadTexture(std::string filename, GLuint texid);
	void setTime(int start, int end);
	int getStartTime();
	int getEndTime();
	int getNumSubMeshes();
	Mesh::SubMesh& getSubMesh(int i);

private:
	void findKeyframe(Mesh::SubMesh& mesh, int time, int* position, int* rotation, int* scale);

	vector<SubMesh> m_submesh;
	GLuint m_vbo;
	vector<BakedSubMesh> m_baked_submesh;
	vector<GLuint> m_textures;

	int m_start_time;
	int m_end_time;
};