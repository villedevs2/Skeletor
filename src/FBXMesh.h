#pragma once

#include <QtGui>

#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"

#include <fbxsdk.h>
#include <fbxsdk/fileio/fbxiosettings.h>

#include <string>

#include "Mesh.h"

class FBXMesh
{
public:
	FBXMesh();
	~FBXMesh();

	bool load(std::string filename, Mesh* mesh);

private:
	void processNode(FbxNode* node);
	void processMesh(FbxMesh* mesh);
	void processKeyframes(vector<Mesh::FBXKeyframe>& keyframes, FbxPropertyT<FbxDouble3>& prop, FbxNode* node);

	FbxManager* m_sdk_manager;

	Mesh* m_mesh;

	FbxAnimStack* m_animstack;

	int m_anim_start_time;
	int m_anim_end_time;
};