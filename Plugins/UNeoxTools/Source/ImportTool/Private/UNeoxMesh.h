#pragma once
#include <string>
#include <vector>
//#include "UNeoxMesh.generated.h"
// the .generated.h file should always be the last #include in a header
/*
*	read Mesh  from Neox
*/
struct Vertex
{
	float x;
	float y;
	float z;
	Vertex() { x = 0.0, y = 0.0, z = 0.0; }
	Vertex(const Vertex& t) :x(t.x), y(t.y), z(t.z) {}
	Vertex(float v1, float v2, float v3) : x(v1), y(v2), z(v3) {}
};
struct Triangle
{
	short a;
	short b;
	short c;
	Triangle(short x, short y, short z) :a(x), b(y), c(z) {}
};
struct MeshInfoIterm
{
	unsigned int v_count;
	unsigned int triangle_count;
	signed char uv_channel_count;
	bool has_vertex_color;
};

class UNeoxVertex {
public:
	UNeoxVertex(float x, float y, float z);
public:
	Vertex position;
	Vertex normal;
	Vertex tangent;
	bool color;
	std::vector<std::pair<float, float>>uvs;
};

//UCLASS(Blueprintable)
class UNeoxMesh //:public UObject
{
public:
	UNeoxMesh();
	//UPROPERTY(EditAnywhere, Category = "My Object Properties")
	//FString Name;//this value is created for editor property
	void init(std::string file_path, int sub_count);
	bool readMesh();
public:
	std::string _file_path;
	std::vector<UNeoxVertex>_vertices;
	std::vector<MeshInfoIterm>_sub_mesh_info;
	std::vector<Triangle>_triangles;
	int _submesh_count;
	unsigned int _mark;
	unsigned int _version;
	short _mesh_type;
	short _bone_count;
	bool _has_morph_animation;
	bool _has_track_animation;
	bool _has_skeletal_animation;
};

