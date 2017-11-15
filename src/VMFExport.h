#pragma once

#include "glm/glm.hpp"

#include "BinaryFile.h"
#include "Mesh.h"

using namespace std;

class VMFExport
{
public:
	VMFExport();
	~VMFExport();

	bool write(std::string filename, Mesh* mesh);

private:
	char* strip_path(const char* path);

	static const int VMF_HEADER_VERSION = 0x0004;
	static const int VMF_MESH_VERSION = 0x0004;

	static const unsigned int VMF_FORMAT_ID = 0x564d4658;
	static const unsigned int VMF_MESH_ID = 0x53424d58;
	static const unsigned int VMF_TEX_ID = 0x54455858;
	static const unsigned int VMF_VTX_ID = 0x56545858;
	static const unsigned int VMF_TVTX_ID = 0x54565458;
	static const unsigned int VMF_NRM_ID = 0x4e524d58;
	static const unsigned int VMF_FACE_ID = 0x46435858;
	static const unsigned int VMF_KEY_ID = 0x4b455958;
	static const unsigned int VMF_ANIM_ID = 0x414e4d58;
};