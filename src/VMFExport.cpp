#include "VMFExport.h"

VMFExport::VMFExport()
{
}

VMFExport::~VMFExport()
{
}

bool VMFExport::write(std::string filename, Mesh* mesh)
{
	BinaryFile output;

	try
	{
		int num_submeshes = mesh->getNumSubMeshes();

		output.open(filename, BinaryFile::MODE_WRITEONLY);

		// VMFX format ID
		output.write_dword(VMF_FORMAT_ID);

		// header version
		output.write_dword(VMF_HEADER_VERSION);

		// total number of faces
		int total_faces = 0;
		for (int i=0; i < num_submeshes; i++)
		{
			Mesh::SubMesh& sb = mesh->getSubMesh(i);
			total_faces += (int)sb.tris.size();
		}
		output.write_dword(total_faces);

		// number of submeshes
		output.write_dword(num_submeshes);


		// submeshes
		for (int sbi=0; sbi < num_submeshes; sbi++)
		{
			Mesh::SubMesh sb = mesh->getSubMesh(sbi);

			int num_verts = (int)sb.position.size();
			int num_tverts = (int)sb.uvcoord.size();
			int num_normals = (int)sb.normal.size();
			int num_faces = (int)sb.tris.size();

			int submesh_structure_length = 0;
			submesh_structure_length += 64;								// name
			submesh_structure_length += 64;								// matrix
			submesh_structure_length += 4;								// texture id
			submesh_structure_length += 4;								// num of textures
			submesh_structure_length += (int)(1) * (4+64);				// texture structures
			submesh_structure_length += 4;								// vertex id
			submesh_structure_length += 4;								// num of verts
			submesh_structure_length += (int)(num_verts) * 12;			// vert structures
			submesh_structure_length += 4;								// tex vertex id
			submesh_structure_length += 4;								// num of tex verts
			submesh_structure_length += (int)(num_tverts) * 8;			// tex vert structures
			submesh_structure_length += 4;								// normal id
			submesh_structure_length += 4;								// num of normals
			submesh_structure_length += (int)(num_normals) * 12;		// normal structures
			submesh_structure_length += 4;								// face id
			submesh_structure_length += 4;								// num of faces
			submesh_structure_length += (int)(num_faces) * 36;			// face structures

			// submesh header
			// --------------------------------------------------------------------------

			// submesh id
			output.write_dword(VMF_MESH_ID);

			// submesh version
			output.write_dword(VMF_MESH_VERSION);

			// submesh structure length
			output.write_dword(submesh_structure_length);

			// submesh name
			char sbname[64];
			memset(sbname, 0, 64);
			strcpy(sbname, sb.name.c_str());
			output.write(sbname, 64);

			// submesh matrix
			for (int r=0; r < 4; r++)
			{
				for (int c=0; c < 4; c++)
				{
					float mv = sb.matrix[r][c];
					output.write_float(mv);
				}
			}

			// textures
			// --------------------------------------------------------------------------

			// texture id
			output.write_dword(VMF_TEX_ID);

			// number of textures
			output.write_dword(1);

			// texture structures
			// texture type
			output.write_dword(0x0001);		// always diffuse for now

			// texture name
			char texname[64];
			memset(texname, 0, 64);
			char* texname_stripped = strip_path(sb.texname.c_str());
			strcpy(texname, texname_stripped);
			output.write(texname, 64);

			// vertex coordinates
			// --------------------------------------------------------------------------

			// vertex id
			output.write_dword(VMF_VTX_ID);

			// number of vertices
			output.write_dword(num_verts);

			// vertex structures
			for (int i=0; i < num_verts; i++)
			{
				output.write_float(sb.position[i].x);
				output.write_float(sb.position[i].y);
				output.write_float(sb.position[i].z);
			}

			// texture vertex coordinates
			// --------------------------------------------------------------------------

			// tex vertex id
			output.write_dword(VMF_TVTX_ID);

			// number of tex vertices
			output.write_dword(num_tverts);

			// tex vertex structures
			for (int i=0; i < num_tverts; i++)
			{
				output.write_float(sb.uvcoord[i].x);
				output.write_float(sb.uvcoord[i].y);
			}

			// faces
			// --------------------------------------------------------------------------

			// face id
			output.write_dword(VMF_FACE_ID);

			// number of faces
			output.write_dword(num_faces);

			// face structures
			for (int i=0; i < num_faces; i++)
			{
				output.write_dword(sb.tris[i].p[0]);
				output.write_dword(sb.tris[i].p[1]);
				output.write_dword(sb.tris[i].p[2]);

				output.write_dword(sb.tris[i].uv[0]);
				output.write_dword(sb.tris[i].uv[1]);
				output.write_dword(sb.tris[i].uv[2]);

				output.write_float(sb.tris[i].normal[0].x);
				output.write_float(sb.tris[i].normal[0].y);
				output.write_float(sb.tris[i].normal[0].z);
				output.write_float(sb.tris[i].normal[1].x);
				output.write_float(sb.tris[i].normal[1].y);
				output.write_float(sb.tris[i].normal[1].z);
				output.write_float(sb.tris[i].normal[2].x);
				output.write_float(sb.tris[i].normal[2].y);
				output.write_float(sb.tris[i].normal[2].z);
			}
		}

		output.close();
	}
	catch (int e)
	{
		output.close();
		return false;
	}
}

char* VMFExport::strip_path(const char *path)
{
	static char temppath[200];
	int length = strlen(path);
	int start;

	for (start=length-1; start >= 0; start--)
	{
		if (path[start] == '\\')
		{
			start++;
			break;
		}
	}

	strncpy_s(temppath, &path[start], length-start);

	return temppath;
}