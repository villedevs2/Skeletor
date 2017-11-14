#include "Mesh.h"

Mesh::Mesh()
{
	m_vbo = NULL;

	reset();
}

Mesh::~Mesh()
{
	
}

void Mesh::reset()
{
	m_submesh.clear();
}

void Mesh::setTime(int start, int end)
{
	m_start_time = start;
	m_end_time = end;
}

void Mesh::addSubMesh(Mesh::SubMesh submesh)
{
	m_submesh.push_back(submesh);
}

int Mesh::getStartTime()
{
	return m_start_time;
}

int Mesh::getEndTime()
{
	return m_end_time;
}

int Mesh::getNumSubMeshes()
{
	return m_submesh.size();
}


Mesh::SubMesh& Mesh::getSubMesh(int i)
{
	return m_submesh[i];
}

void Mesh::loadTexture(std::string filename, GLuint texid)
{
	// strip path
	size_t p;
	p = filename.find_last_of("\\");
	if (p == string::npos)
	{
		p = filename.find_last_of("/");
	}

	std::string path_stripped = filename.substr(p+1, string::npos);

	// extract extension
	p = path_stripped.find_last_of(".");
	
	std::string extension = path_stripped.substr(p+1, string::npos);

	if (extension.compare("png") == 0)
	{
		QImage *image = new QImage();
		bool result = image->load(filename.c_str());

		if (result)
		{
			int width = image->width();
			int height = image->height();

			char *pixels = new char[width * height * 4];

			int index = 0;
			for (int j=0; j < height; j++)
			{
				QRgb *scan = (QRgb*)image->scanLine(j);
		
				for (int i=0; i < width; i++)
				{
					int r = qRed(scan[i]);
					int g = qGreen(scan[i]);
					int b = qBlue(scan[i]);
					int a = qAlpha(scan[i]);

					pixels[index+0] = r;
					pixels[index+1] = g;
					pixels[index+2] = b;
					pixels[index+3] = a;
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

			delete [] pixels;
		}
	}
}

void Mesh::bake()
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

	int tri_count = 0;

	// count triangles
	for (int i=0; i < (int)m_submesh.size(); i++)
	{
		tri_count += m_submesh[i].tris.size();
	}

	Vertex* tmpvtx = new Vertex[tri_count * 3];

	int vbo_index = 0;

	for (int i=0; i < (int)m_submesh.size(); i++)
	{
		BakedSubMesh baked;

		int numtris = m_submesh[i].tris.size();

		baked.vbo_index = vbo_index;
		baked.num_verts = numtris * 3;

		for (int tri=0; tri < numtris; tri++)
		{
			int p0 = m_submesh[i].tris[tri].p[0];
			int p1 = m_submesh[i].tris[tri].p[1];
			int p2 = m_submesh[i].tris[tri].p[2];
			int uv0 = m_submesh[i].tris[tri].uv[0];
			int uv1 = m_submesh[i].tris[tri].uv[1];
			int uv2 = m_submesh[i].tris[tri].uv[2];
				
			tmpvtx[vbo_index+0].position = m_submesh[i].position[p0];
			tmpvtx[vbo_index+1].position = m_submesh[i].position[p1];
			tmpvtx[vbo_index+2].position = m_submesh[i].position[p2];
			tmpvtx[vbo_index+0].uvcoord = m_submesh[i].uvcoord[uv0];
			tmpvtx[vbo_index+1].uvcoord = m_submesh[i].uvcoord[uv1];
			tmpvtx[vbo_index+2].uvcoord = m_submesh[i].uvcoord[uv2];
	//		tmpvtx[vbo_index+0].normal = m_submesh[i].normal[v0];
	//		tmpvtx[vbo_index+1].normal = m_submesh[i].normal[v1];
	//		tmpvtx[vbo_index+2].normal = m_submesh[i].normal[v2];

			vbo_index += 3;
		}

		GLuint texname;
		glGenTextures(1, &texname);
		m_textures.push_back(texname);

		loadTexture(m_submesh[i].texname, texname);

		m_baked_submesh.push_back(baked);
	}

	GLsizeiptr size = vbo_index * sizeof(Vertex) + 4;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, size, tmpvtx, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	delete [] tmpvtx;
}

void Mesh::render(DefaultShaderContext* context, int time)
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
		Mesh::SubMesh& sb = m_submesh.at(i);

		//if (m_texture)
		//	m_texture->apply(0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_textures[i]);

		glm::mat4 mat = glm::mat4();

		//mat *= m_submesh[i].matrix;

		/*
		if (m_has_anim)
		{
			mat *= m_anim_transform;
		}
		else
		{
			mat *= m_matrix;
		}
		*/


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

		if (time == 1001)
		{
			int ppspsd = 1;
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



		glUniformMatrix4fv(context->matrix, 1, false, glm::value_ptr(mat));
		glDrawArrays(GL_TRIANGLES, m_baked_submesh[i].vbo_index, m_baked_submesh[i].num_verts);
	}

	glDisableVertexAttribArray(context->position);
	glDisableVertexAttribArray(context->tex_coord);		
//	glDisableVertexAttribArray(context->normal);
	glBindBuffer(GL_ARRAY_BUFFER, 0);	// TODO REMOVE
}


void Mesh::findKeyframe(Mesh::SubMesh& mesh, int time, int* position, int* rotation, int* scale)
{
	*position = 0;
	*rotation = 0;
	*scale = 0;

	int num_pos_keys = mesh.pos_keys.size();
	int num_rot_keys = mesh.rot_keys.size();
	int num_scale_keys = mesh.scl_keys.size();

	for (int j=1; j < num_pos_keys; j++)
	{
		if (mesh.pos_keys[j].time >= time)
		{
			*position = j-1;
			break;
		}
	}
	for (int j=1; j < num_rot_keys; j++)
	{
		if (mesh.rot_keys[j].time >= time)
		{
			*rotation = j-1;
			break;
		}
	}
	for (int j=1; j < num_scale_keys; j++)
	{
		if (mesh.scl_keys[j].time >= time)
		{
			*scale = j-1;
			break;
		}
	}
}