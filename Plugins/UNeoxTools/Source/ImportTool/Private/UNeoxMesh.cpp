#include "UNeoxImportPCH.h"
#include "UNeoxMesh.h"
#include <iostream>
#include <string>
#include <fstream>
using namespace std;



UNeoxVertex::UNeoxVertex(float x, float y, float z) {
	position = Vertex(x, y, z);
	color = false;
}

UNeoxMesh::UNeoxMesh() {

}
// to do some initialization work
void UNeoxMesh::init(string file_path, int sub_count) {
	this->_file_path = file_path;
	this->_submesh_count = sub_count;

	//init neox_header
	this->_mark = 0;
	this->_version = 0;
	this->_mesh_type = 0;
	this->_bone_count = 0;

	this->_has_morph_animation = false;
	this->_has_skeletal_animation = false;
	this->_has_track_animation = false;

}

// read mesh from _file_path
bool UNeoxMesh::readMesh() {
	ifstream meshFile;
	meshFile.open(_file_path, ios::in|ios::binary);
	if (!meshFile.is_open()) {
		printf("error:Unexisted file path!%s\n", _file_path.c_str());
		return false;
	}

	// read header 10byte
	meshFile.read((char*)&_mark,sizeof(unsigned int));
	meshFile.read((char*)&_version, sizeof(unsigned int));
	meshFile.read((char*)&_mesh_type, sizeof(short));
	if (this->_version < 0x50002l)
		return false;

	// read morph animation
	meshFile.read((char*)&_has_morph_animation, sizeof(bool));
	if (this->_has_morph_animation) {
		printf("unable to read morph animations!");
		return false;
	}

	// read track animation
	meshFile.read((char*)&_has_track_animation, sizeof(bool));
	if (this->_has_track_animation) {
		printf("unable to read mesh with tracks!");
		return false;
	}

	// read skeletal animation
	if (this->_mesh_type == 1)
	{
		meshFile.read((char*)&_bone_count, sizeof(short));
		char bones_data, bone_names[256];
		meshFile.read((char*)&bones_data, _bone_count);
		meshFile.read(bone_names, _bone_count * 32);
		bool has_bone_bounding;
		meshFile.read((char*)&has_bone_bounding, sizeof(bool));
		if (has_bone_bounding)
			meshFile.seekg(_bone_count * 7 * 4 + meshFile.tellg());
		char buf[100];
		for (int i = 0; i < 1; i++)
			meshFile.read(buf, 16 * 4);
		bool stupid_bounding;
		meshFile.read((char*)&stupid_bounding, sizeof(bool));
		if (stupid_bounding) {
			printf("Unable to read bounding for every animation key! \n");
			return false;
		}

	}
	
	// read geometry
	unsigned int ofs;
	meshFile.read((char*)&ofs, sizeof(unsigned int));
	meshFile.seekg(ofs);
	short block_count;
	meshFile.read((char*)&block_count, sizeof(unsigned short));
	vector<unsigned int>block_offset;
	for (int i = 0; i < block_count; i++) {
		unsigned int v;
		meshFile.read((char*)&v, sizeof(unsigned int));
		block_offset.push_back(v);
	}

	short pair_count;
	meshFile.read((char*)&pair_count, sizeof(unsigned short));
	for (int i = 0; i < pair_count;i++) {
		char data_table[10];
		meshFile.read(data_table,6);
	}

	//vertices begin
	if (block_offset.empty())
		return false;
	meshFile.seekg(block_offset[0]);
	MeshInfoIterm temp;

	for (int i = 0; i < _submesh_count; i++) {
		meshFile.read((char *)&temp.v_count,sizeof(unsigned int));
		meshFile.read((char *)&temp.triangle_count, sizeof(unsigned int));
		meshFile.read((char *)&temp.uv_channel_count, sizeof(signed char));
		meshFile.read((char *)&temp.has_vertex_color, sizeof(bool));
		_sub_mesh_info.push_back(temp);
	}
	short lod_index;
	meshFile.read((char*)&lod_index,sizeof(short));
	unsigned int v_count;
	meshFile.read((char*)&v_count,sizeof(unsigned int));
	unsigned int triangle_count;
	meshFile.read((char*)&triangle_count, sizeof(unsigned int));

	//position 位置坐标
	for (unsigned int i = 0; i < v_count; i++) {
		float x, y, z;
		meshFile.read((char*)&x, sizeof(float));
		meshFile.read((char*)&y, sizeof(float));
		meshFile.read((char*)&z, sizeof(float));
		_vertices.push_back(UNeoxVertex(x, y, z));
	}
	//normal 法线
	for (unsigned int i = 0; i < v_count; i++) {
		float x, y, z;
		meshFile.read((char*)&x, sizeof(float));
		meshFile.read((char*)&y, sizeof(float));
		meshFile.read((char*)&z, sizeof(float));
		_vertices[i].normal = Vertex(x,y,z);
	}
	//tangent 切线
	short has_tangent;
	meshFile.read((char*)&has_tangent,sizeof(short));
	if (has_tangent != 0) {
		for (unsigned int i=0;i<v_count;i++)
		{
			float x, y, z;
			meshFile.read((char*)&x, sizeof(float));
			meshFile.read((char*)&y, sizeof(float));
			meshFile.read((char*)&z, sizeof(float));
			_vertices[i].tangent = Vertex(x, y, z);
		}
	}
	//triangles
	for (unsigned int i = 0; i < triangle_count; i++) {
		short a, b, c;
		meshFile.read((char*)&a, sizeof(short));
		meshFile.read((char*)&b, sizeof(short));
		meshFile.read((char*)&c, sizeof(short));
		_triangles.push_back(Triangle(a,b,c));
	}

	// uv， not every sub mesh has uv data
	int ver_id_offset = 0;
	for (int submesh_id = 0; submesh_id < _submesh_count; submesh_id++) {
		int submesh_vert_count = _sub_mesh_info[submesh_id].v_count;
		for (int uv_id = 0; uv_id < _sub_mesh_info[submesh_id].uv_channel_count;uv_id++) {
			for (int submesh_vert_id = 0; submesh_vert_id < submesh_vert_count;submesh_vert_id++) {
				int vert_id = submesh_vert_id + ver_id_offset;
				float u, v;
				meshFile.read((char*)&u, sizeof(float));
				meshFile.read((char*)&v, sizeof(float));
				_vertices[vert_id].uvs.push_back(make_pair(u,v));
			}
		}
		ver_id_offset += submesh_vert_count;
	}

	meshFile.close();
	return true;
}
