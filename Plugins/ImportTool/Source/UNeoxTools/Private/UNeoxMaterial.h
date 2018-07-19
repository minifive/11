#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "tinyxml2.h"
struct AnimParam 
{
	int speed;
};
struct ShaderMacro//宏
{
};
struct Technique
{
	std::string type;
	std::string techName;
};
struct ParamTable
{
	std::string tex0Value;//最多两个纹理？
	std::string tex1Value;
	bool cloudSameDirection;
};
struct LightIterm
{
	float a;float b;float g;float r;
	LightIterm():a(0.0),b(0.0),g(0.0),r(0.0) {}
	LightIterm(float x, float y, float z, float t) :a(x),b(y),g(z),r(t){}
};
struct LightingMaterial
{
	LightIterm diffuse;//漫反射
	LightIterm ambient;//环境光
	LightIterm specular;//高光
	LightIterm emissive;
	float power;
};
struct  RenderStates
{
	bool lightingEnable;
	bool specularEnable;
	bool vcolorEnable;
	bool alpTestEnable;
	int alphaRef;
	bool isAlphaRefTest;
	int alphaVal;
	bool isAlphaValSet;
	bool cullBack;
	bool dirty;
	int transparentMode;
	RenderStates():cullBack(false),transparentMode(0),dirty(false),isAlphaValSet(false),alphaVal(0),isAlphaRefTest(false),
		lightingEnable(false),specularEnable(false),vcolorEnable(false),alpTestEnable(false),alphaRef(0){}
};
struct MaterialIterm 
{
	int version;
	std::string name;
	AnimParam animParam;
	ShaderMacro shaderMacro;
	Technique technique;
	ParamTable paramTable;
	LightingMaterial lightMaterial;
	RenderStates renderStates;
};
struct Material
{
	int type;//是否外部材质文件。如果此位为0，表示内部材质，即所有neox内部创建或复制(包括从材质文件复制)的材质
	MaterialIterm materialIterm;
};
struct MaterialGroup
{
	int version;
	std::string name;
	int materialCount;
	std::vector<Material>materials;
};
class UNeoxMaterial {
public:
	void init(std::string fileName);
	bool readMaterial();
	bool parseMaterial(tinyxml2::XMLElement* subMaterial, Material &material);
	//transform const char* into int
	int versionHelper(std::string str);
	//transform const char * into LightIterm
	LightIterm lightHelper(std::string str);
public:
	std::string _filePath;
	MaterialGroup _materialGroup;
};