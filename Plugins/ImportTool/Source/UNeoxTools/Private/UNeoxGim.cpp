#include "UNeoxImportPCH.h"
#include "UneoxGim.h"
#include <iostream>
using namespace tinyxml2;

UNeoxGim::UNeoxGim() {

}

bool UNeoxGim::readGim()
{
	XMLDocument doc;
	if (doc.LoadFile(this->_gimFile.c_str())) {
		printf("can not open file:%s \n",this->_gimFile.c_str());
		return false;
	}
	XMLElement* neoxElement = doc.RootElement();
	//parse gim header
	if (!parseHeader(neoxElement)) {
		printf("parse neox header failed \n");
		return false;
	}
	//parse sub mesh
	XMLElement* subMesh = neoxElement->FirstChildElement("SubMesh");
	if (!parseSubMesh(subMesh)) {
		printf("parse neox sub mesh failed \n");
		return false;
	}
	//GisFiles

	return true;
}

bool UNeoxGim::readMaterial()
{
	std::string temp = this->_gimFile;
	this->_mtgFile = temp.replace(temp.find(".gim"), 4, ".mtg");
	this->UMaterial = new UNeoxMaterial();
	this->UMaterial->init(_mtgFile);
	if(this->UMaterial->readMaterial())
		return true;
	return false;
}

bool UNeoxGim::readMesh()
{
	std::string temp = this->_gimFile;
	this->_meshFile = temp.replace(temp.find(".gim"), 4, ".mesh");
	this->UMesh = new UNeoxMesh();
	this->UMesh->init(_meshFile, this->subCount);
	if (this->UMesh->readMesh())
		return true;
	return false;
}


void UNeoxGim::init(std::string gim_path)
{
	this->_gimFile = gim_path;
	//gim data
	this->_compatibleMask=0;
	this->_trisortMethod=0;
	this->_hasAlphaSub=false;
	this->_tangentEnable=false;
	this->_animAccumEnable = false;
	this->_HwInstancingEnable = false;
	this->_preZALphaBlend = false;
	this->_dynamicMergEnable = false;
	this->_OWLAlphaTestMerge = false;

	this->subCount = 0;
}

bool UNeoxGim::parseHeader(XMLElement * neoxElement)
{
	//CompatibleMask
	const XMLAttribute* CompatibleMask = neoxElement->FindAttribute("CompatibleMask");
	if (CompatibleMask != NULL) {
		this->_compatibleMask = atoi(CompatibleMask->Value());
	}
		
	//TriSortMethod
	const XMLAttribute* TriSortMethod = neoxElement->FindAttribute("TriSortMethod");
	if (TriSortMethod != NULL) {
		this->_trisortMethod = atoi(TriSortMethod->Value());
	}
		

	//TangentEnable
	const XMLAttribute* TangentEnable = neoxElement->FindAttribute("TangentEnable");
	if (TangentEnable != NULL) {
		const char* value = TangentEnable->Value();
		if (strcmp(value, "false") == 0)
			this->_tangentEnable = false;
		else
			this->_tangentEnable = true;
	}

	//AnimAccumEnable
	const XMLAttribute* AnimAccumEnable = neoxElement->FindAttribute("AnimAccumEnable");
	if (AnimAccumEnable != NULL) {
		const char* value = AnimAccumEnable->Value();
		if (strcmp(value, "false") == 0)
			this->_animAccumEnable = false;
		else
			this->_animAccumEnable = true;
	}

	//HwInstancingEnable
	const XMLAttribute* HwInstancingEnable = neoxElement->FindAttribute("HwInstancingEnable");
	if (HwInstancingEnable != NULL) {
		const char* value = HwInstancingEnable->Value();
		if (strcmp(value, "false") == 0)
			this->_HwInstancingEnable = false;
		else
			this->_HwInstancingEnable = true;
	}

	//PreZAlphaBlend
	const XMLAttribute* PreZAlphaBlend = neoxElement->FindAttribute("PreZAlphaBlend");
	if (PreZAlphaBlend != NULL) {
		const char* value = PreZAlphaBlend->Value();
		if (strcmp(value, "false") == 0)
			this->_preZALphaBlend = false;
		else
			this->_HwInstancingEnable = true;
	}

	//DynamicMergeEnable
	const XMLAttribute* DynamicMergeEnable = neoxElement->FindAttribute("DynamicMergeEnable");
	if (DynamicMergeEnable != NULL) {
		const char* value = DynamicMergeEnable->Value();
		if (strcmp(value, "false") == 0)
			this->_dynamicMergEnable = false;
		else
			this->_dynamicMergEnable = true;
	}

	//OWLAlphaTestMerge
	const XMLAttribute* OWLAlphaTestMerge = neoxElement->FindAttribute("OWLAlphaTestMerge");
	if (OWLAlphaTestMerge != NULL) {
		const char* value = OWLAlphaTestMerge->Value();
		if (strcmp(value, "false") == 0)
			this->_OWLAlphaTestMerge = false;
		else
			this->_OWLAlphaTestMerge = true;
	}

	//MaxName
	const XMLAttribute* MaxName = neoxElement->FindAttribute("MaxName");
	if (MaxName != NULL) {
		this->_maxName = MaxName->Value();
	}

	//BoundingInfo
	const XMLAttribute* BoundingInfo = neoxElement->FindAttribute("BoundingInfo");
	if (BoundingInfo != NULL) {
		this->_boundingInfo = boundingHelper(std::string(BoundingInfo->Value()));
	}


	return true;
}

bool UNeoxGim::parseSubMesh(XMLElement * subMeshElement)
{
	XMLElement* subMeshNode = subMeshElement->FirstChildElement();
	int subMeshCount = 0;
	while (subMeshNode)
	{
		SubMeshIterm subMeshIterm;
		//Name
		const XMLAttribute* meshName = subMeshNode->FindAttribute("Name");
		if (meshName != NULL) {
			subMeshIterm.subMeshName = meshName->Value();
		}
		//MtiIdx
		const XMLAttribute* MtiIdx = subMeshNode->FindAttribute("MtlIdx");
		if (MtiIdx != NULL) {
			subMeshIterm.MtlIdx = atoi(MtiIdx->Value());
		}
		//BoundingCenter
		const XMLAttribute* BoundingCenter = subMeshNode->FindAttribute("BoundingCenter");
		if (BoundingCenter != NULL) {
			subMeshIterm.boundingCenter = vertexHelper(std::string( BoundingCenter->Value()));
		}
		//BoundingHalf
		const XMLAttribute* BoundingHalf = subMeshNode->FindAttribute("BoundingHalf");
		if (BoundingHalf != NULL) {
			subMeshIterm.boundingHalf = vertexHelper(std::string(BoundingHalf->Value()));
		}
		//BoundingInfo
		const XMLAttribute* BoundingInfo = subMeshNode->FindAttribute("BoundingInfo");
		if (BoundingInfo != NULL) {
			subMeshIterm.boundingInfo = boundingHelper(std::string(BoundingInfo->Value()));
		}
		this->_subMeshs.push_back(subMeshIterm);
		subMeshCount++;
		subMeshNode = subMeshNode->NextSiblingElement();
	}
	this->subCount = subMeshCount;
	return true;
}



BoundingIterm UNeoxGim:: boundingHelper(std::string str) {
	std::vector<std::string>buffer;
	int front = 0;
	int back = str.find_first_of(',', front);
	while (back!=std::string::npos)
	{
		buffer.push_back(str.substr(front, back - front));
		front = back + 1;
		back = str.find_first_of(',', front);
	}
	buffer.push_back(str.substr(front, str.length() - front));
	std::vector<float>Points;
	for (int i = 0; i < buffer.size(); i++) {
		std::string temp="";
		for (int j = 0; j < buffer[i].size(); j++) {
			if (buffer[i][j] != '(' && buffer[i][j] != ')')
				temp += buffer[i][j];
		}
		Points.push_back(atof(temp.c_str()));
	}
	return BoundingIterm(Points);
}

Vertex UNeoxGim::vertexHelper(std::string str)
{
	std::vector<float>buffer;
	int front = 0;
	int back = str.find_first_of(',', front);
	while (back != std::string::npos)
	{
		buffer.push_back(atof(str.substr(front, back - front).c_str()));
		front = back + 1;
		back = str.find_first_of(',', front);
	}
	buffer.push_back(atof(str.substr(front, back - front).c_str()));
	if (buffer.size() != 3)
		return Vertex();
	return Vertex(buffer[0],buffer[1],buffer[2]);
}
