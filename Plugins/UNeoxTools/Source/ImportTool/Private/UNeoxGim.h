#pragma once
#include <string>
#include <iostream>
#include "UneoxMesh.h"
#include "UNeoxMaterial.h"
#include "tinyxml2.h"

struct BoundingIterm
{
	Vertex center;
	Vertex half_size;
	float radius;
	BoundingIterm():center(Vertex()),half_size(Vertex()),radius(0.0) {}
	BoundingIterm(const BoundingIterm& Bterm) :center(Bterm.center), half_size(Bterm.half_size), radius(Bterm.radius) {}
	BoundingIterm(std::vector<float>Points) {
		if (Points.size() != 7)
			return;
		center = Vertex(Points[0], Points[1], Points[2]);
		half_size = Vertex(Points[3], Points[4], Points[5]);
		radius = Points[6];
	}
};
struct SubMeshIterm
{
	std::string subMeshName;
	int MtlIdx;
	Vertex boundingCenter;
	Vertex boundingHalf;
	BoundingIterm boundingInfo;
	SubMeshIterm() :subMeshName(""), MtlIdx(0), boundingCenter(Vertex()), boundingHalf(Vertex()), boundingInfo(BoundingIterm()) {}
	SubMeshIterm(const SubMeshIterm& T):subMeshName(T.subMeshName),MtlIdx(T.MtlIdx),boundingCenter(T.boundingCenter),boundingHalf(T.boundingHalf),boundingInfo(T.boundingInfo) {}
};
class UNeoxGim {
public:
	UNeoxGim();

	bool readGim();
	bool readMaterial();
	bool readMesh();
	//when you begin to parse gim file you should do init first
	void init(std::string file_path);

	bool parseHeader(tinyxml2::XMLElement* neoxElement);
	bool parseSubMesh(tinyxml2::XMLElement* subMeshElement);

	//this function is help to transform string to boundingInfo
	BoundingIterm boundingHelper(std::string str);
	//transform string to vertex
	Vertex vertexHelper(std::string str);
public:
	std::string _gimFile;
	std::string _meshFile;
	std::string _mtgFile;
	UNeoxMesh *UMesh;
	UNeoxMaterial* UMaterial;

public://gim data
	int _compatibleMask;
	int _trisortMethod;
	bool _hasAlphaSub;
	//int _ambLightClr
	bool _tangentEnable;
	bool _animAccumEnable;
	bool _HwInstancingEnable;
	bool _preZALphaBlend;
	bool _dynamicMergEnable;
	bool _OWLAlphaTestMerge;
	const char *_maxName;
	BoundingIterm _boundingInfo;
	std::vector<SubMeshIterm> _subMeshs;
	int subCount;
};