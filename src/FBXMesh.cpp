#include "FBXMesh.h"

FBXMesh::FBXMesh()
{
	m_sdk_manager = FbxManager::Create();
}

FBXMesh::~FBXMesh()
{
}

bool FBXMesh::load(std::string filename, Mesh* mesh)
{
	m_mesh = mesh;
	mesh->reset();

	FbxIOSettings* ios = FbxIOSettings::Create(m_sdk_manager, IOSROOT);
	m_sdk_manager->SetIOSettings(ios);

	FbxImporter* importer = FbxImporter::Create(m_sdk_manager, "");
	if (!importer->Initialize(filename.c_str(), -1, m_sdk_manager->GetIOSettings()))
	{
		return false;
	}

	FbxScene* scene = FbxScene::Create(m_sdk_manager, "scene");
	importer->Import(scene);
	importer->Destroy();

	FbxAxisSystem::OpenGL.ConvertScene(scene);

	int num_animstacks = scene->GetSrcObjectCount<FbxAnimStack>();
	m_animstack = scene->GetSrcObject<FbxAnimStack>(0);

	FbxGlobalSettings& settings = scene->GetGlobalSettings();
	FbxTime::EMode timemode = settings.GetTimeMode();
	
	FbxTimeSpan timespan;
	settings.GetTimelineDefaultTimeSpan(timespan);

	m_anim_start_time = timespan.GetStart().GetMilliSeconds();
	m_anim_end_time = timespan.GetStop().GetMilliSeconds();

	processNode(scene->GetRootNode());

	m_mesh->setTime(m_anim_start_time, m_anim_end_time);

	m_mesh->bake();

	return true;
}

void FBXMesh::processNode(FbxNode* node)
{
	if (node)
	{
		const char* name = node->GetName();
		int num_attributes = node->GetNodeAttributeCount();
		int num_children = node->GetChildCount();

		// process attributes
		for (int i=0; i < num_attributes; i++)
		{
			FbxNodeAttribute* attr = node->GetNodeAttributeByIndex(i);
			const char* attrname = attr->GetName();
			
			FbxNodeAttribute::EType type = attr->GetAttributeType();
			switch (type)
			{
				case FbxNodeAttribute::eUnknown:
					break;

				case FbxNodeAttribute::eNull:
					break;

				case FbxNodeAttribute::eMesh:
				{
					FbxMesh* mesh = (FbxMesh*)attr;
					processMesh(mesh);

					break;
				}

				case FbxNodeAttribute::eLight:
					break;

				default:
					break;
			}
		}

		// recurse to children
		for (int i=0; i < num_children; i++)
		{
			processNode(node->GetChild(i));
		}
	}
}

void FBXMesh::processMesh(FbxMesh* mesh)
{						
	int polycount = mesh->GetPolygonCount();
	int uvcount = mesh->GetTextureUVCount();

	int* polys = mesh->GetPolygonVertices();

	FbxNode* node = mesh->GetNode();

	Mesh::SubMesh submesh;
	submesh.position.clear();
	submesh.uvcoord.clear();

	FbxSurfacePhong *material = (FbxSurfacePhong*)node->GetMaterial(0);
	
	int matcount = node->GetMaterialCount();

	FbxFileTexture *tex = (FbxFileTexture*)material->Diffuse.GetSrcObject(0); ;

	submesh.texname = std::string(tex->GetFileName());
	submesh.name = std::string(node->GetName());


	// extract triangles
	for (int i=0; i < polycount; i++)
	{
		Mesh::Triangle tri;

		for (int pv=0; pv < 3; pv++)
		{
			tri.p[pv] = mesh->GetPolygonVertex(i, pv);
			tri.uv[pv] = mesh->GetTextureUVIndex(i, pv);
			//tri.uv[pv] = (i * 3) + pv;
			//tri.uv[pv] = mesh->GetPolygonVertex(i, pv);

			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(i, pv, normal);
			tri.normal[pv] = glm::vec3(normal[0], normal[1], normal[2]);
		}

		

		submesh.tris.push_back(tri);
	}

	FbxVector4* cps = mesh->GetControlPoints(); 
	int cpcount = mesh->GetControlPointsCount();
	for (int i=0; i < cpcount; i++)
	{
		glm::vec3 coord = glm::vec3(cps[i].mData[0], cps[i].mData[1], cps[i].mData[2]);
		submesh.position.push_back(coord);
	}


	int elcount = mesh->GetElementUVCount();

	
	bool indexmode = mesh->GetElementUV(0)->GetReferenceMode();
	const FbxGeometryElementUV* element = mesh->GetElementUV(0);


	FbxLayerElementArrayTemplate<FbxVector2> uvs = mesh->GetElementUV(0)->GetDirectArray();
	int uvcc = uvs.GetCount();
	// extract uvcoords
	for (int i=0; i < uvs.GetCount(); i++)
	{
		FbxVector2 v = uvs.GetAt(i);
		glm::vec2 vv = glm::vec2(v.mData[0], 1.0f - v.mData[1]);	// y inverted, why?

		submesh.uvcoord.push_back(vv);
	}

	FbxAMatrix& transform = node->EvaluateGlobalTransform();

	FbxVector4 row1 = transform.GetRow(0);
	FbxVector4 row2 = transform.GetRow(1);
	FbxVector4 row3 = transform.GetRow(2);
	FbxVector4 row4 = transform.GetRow(3);

	glm::mat4 matrix = glm::mat4(glm::vec4(row1.mData[0], row1.mData[1], row1.mData[2], row1.mData[3]),
								 glm::vec4(row2.mData[0], row2.mData[1], row2.mData[2], row2.mData[3]),
								 glm::vec4(row3.mData[0], row3.mData[1], row3.mData[2], row3.mData[3]),
								 glm::vec4(row4.mData[0], row4.mData[1], row4.mData[2], row4.mData[3]));

	submesh.matrix = matrix;


	vector<Mesh::FBXKeyframe> pos_frames;
	vector<Mesh::FBXKeyframe> rot_frames;
	vector<Mesh::FBXKeyframe> scale_frames;

	processKeyframes(pos_frames, node->LclTranslation, node);
	processKeyframes(rot_frames, node->LclRotation, node);
	processKeyframes(scale_frames, node->LclScaling, node);

	// insert pos keyframes
	for (int i=0; i < (int)pos_frames.size(); i++)
	{
		Mesh::PosKeyframe keyframe;
		keyframe.time = pos_frames[i].time;
		
		float px = pos_frames[i].value.x;
		float py = pos_frames[i].value.y;
		float pz = pos_frames[i].value.z;

		keyframe.pos = glm::vec3(px, py, pz);

		submesh.pos_keys.push_back(keyframe);
	}

	// insert rot keyframes
	for (int i=0; i < (int)rot_frames.size(); i++)
	{
		Mesh::RotKeyframe keyframe;
		keyframe.time = rot_frames[i].time;

		float ex = rot_frames[i].value.x * M_PI / 180.0f;
		float ey = rot_frames[i].value.y * M_PI / 180.0f;
		float ez = rot_frames[i].value.z * M_PI / 180.0f;

		keyframe.rot = glm::quat(glm::vec3(ex, ey, ez));
		submesh.rot_keys.push_back(keyframe);
	}

	// insert scale keyframes
	for (int i=0; i < (int)scale_frames.size(); i++)
	{
		Mesh::ScaleKeyframe keyframe;
		keyframe.time = scale_frames[i].time;
		keyframe.scale = scale_frames[i].value;
		submesh.scl_keys.push_back(keyframe);
	}

	m_mesh->addSubMesh(submesh);
}

bool keyframe_sorter(int i,int j) { return (i<j); }

void FBXMesh::processKeyframes(vector<Mesh::FBXKeyframe>& keyframes, FbxPropertyT<FbxDouble3>& prop, FbxNode* node)
{
	const char *comp_channels[3] = { FBXSDK_CURVENODE_COMPONENT_X, FBXSDK_CURVENODE_COMPONENT_Y, FBXSDK_CURVENODE_COMPONENT_Z };

	int num_layers = m_animstack->GetMemberCount<FbxAnimLayer>();
	FbxAnimLayer *animlayer = m_animstack->GetMember<FbxAnimLayer>(0);

	vector<int> kf_times;

	// chart out keyframe times
	for (int channel=0; channel < 3; channel++)
	{
		FbxAnimCurve *curve = prop.GetCurve(animlayer, comp_channels[channel]);

		if (curve)
		{
			int numkeys = curve->KeyGetCount();
			for (int i=0; i < numkeys; i++)
			{
				FbxAnimCurveKey key = curve->KeyGet(i);
	
				float value = static_cast<float>(key.GetValue());

				FbxTime time = key.GetTime();
				int time_millis = time.GetMilliSeconds();

				if (time_millis <= m_anim_end_time)
				{
					Mesh::FBXKeyframe keyframe;
					keyframe.time = time_millis;
					keyframe.value = glm::vec3(value, 0, 0);
					keyframes.push_back(keyframe);

					// check if this timecode already exists
					int num_kf_times = (int)kf_times.size();
					bool exists = false;
					for (int k=0; k < num_kf_times; k++)
					{
						if (kf_times[k] == time_millis)
						{
							exists = true;
						}
					}

					// insert if it doesn't exist
					if (!exists)
					{
						kf_times.push_back(time_millis);
					}
				}
			}
		}
	}

	// sort keyframes by time
	std::sort(kf_times.begin(), kf_times.end(), keyframe_sorter);

	// sample and insert keyframes
	FbxAnimCurve *x_curve = prop.GetCurve(animlayer, comp_channels[0]);
	FbxAnimCurve *y_curve = prop.GetCurve(animlayer, comp_channels[1]);
	FbxAnimCurve *z_curve = prop.GetCurve(animlayer, comp_channels[2]);

	int num_kf_times = (int)kf_times.size();

	keyframes.clear();

	for (int i=0; i < num_kf_times; i++)
	{
		FbxTime time;
		time.SetMilliSeconds(kf_times[i]);

		float xv = x_curve->Evaluate(time);
		float yv = y_curve->Evaluate(time);
		float zv = z_curve->Evaluate(time);

		Mesh::FBXKeyframe keyframe;
		keyframe.time = kf_times[i];
		keyframe.value = glm::vec3(xv, yv, zv);

		keyframes.push_back(keyframe);
	}

}