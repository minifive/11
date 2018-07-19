#include "UNeoxImportPCH.h"
#include "UNeoxMaterial.h"
using namespace tinyxml2;
void UNeoxMaterial::init(std::string fileName)
{
	this->_filePath = fileName;
}

bool UNeoxMaterial::readMaterial()
{
	XMLDocument doc;
	if (doc.LoadFile(this->_filePath.c_str())) {
		printf("can not open file:%s \n", this->_filePath.c_str());
		return false;
	}
	XMLElement* neoxElement = doc.RootElement();

	//parse materail group
	XMLElement* MaterialGroup = neoxElement->FirstChildElement("MaterialGroup");
	if (MaterialGroup == NULL) {
		return false;
	}
	const XMLAttribute* Version = MaterialGroup->FindAttribute("Version");
	if (Version == NULL)
		return false;
	this->_materialGroup.version = versionHelper(std::string(Version->Value()));
	this->_materialGroup.name = MaterialGroup->FindAttribute("Name")->Value();
	this->_materialGroup.materialCount = atoi(MaterialGroup->FindAttribute("MaterialCount")->Value());
	XMLElement*materialNode = MaterialGroup->FirstChildElement();
	while (materialNode)
	{
		Material m;
		if (!parseMaterial(materialNode,m)) {
			return false;
		}
		this->_materialGroup.materials.push_back(m);
		materialNode = materialNode->NextSiblingElement();
	}
	return true;
}

bool UNeoxMaterial::parseMaterial(tinyxml2::XMLElement * subMaterial, Material &material)
{
	//type
	const XMLAttribute*Type = subMaterial->FindAttribute("Type");
	if(Type!=NULL)
		material.type = atoi(Type->Value());
	if (material.type &128) {//not neox material
		printf("this is not neox material");
		return false;
	}
	//parse material begin
	XMLElement* curMaterial = subMaterial->FirstChildElement();
	if (curMaterial == NULL)
		return false;
	//Version
	const XMLAttribute* Version = curMaterial->FindAttribute("Version");
	if (Version == NULL) 
		return false;
	material.materialIterm.version = versionHelper(std::string(Version->Value()));
	if (material.materialIterm.version < 0x0102)
		return false;

	//Name
	material.materialIterm.name = curMaterial->FindAttribute("Name")->Value();
	//AnimParam
	XMLElement* AnimParam = curMaterial->FirstChildElement("AnimParam");
	const XMLAttribute* Speed = AnimParam->FindAttribute("Speed");
	if(Speed!=NULL)
		material.materialIterm.animParam.speed = atoi(Speed->Value());
	//ShaderMacro


	//Technique
	XMLElement* Technique = curMaterial->FirstChildElement("Technique");
	const char * techniqueType = Technique->FindAttribute("Type")->Value();
	material.materialIterm.technique.type = techniqueType;
	if ((strcmp(techniqueType, "Effect") == 0) || (strcmp(techniqueType, "NeoX") == 0)) {//< ModelNormalTech > and  < ModelAlphaTech >
		const char * techniqueName = Technique->FindAttribute("TechName")->Value();
		if (strcmp(techniqueName, "<ModelNormalTech>")==0) {
			material.materialIterm.technique.techName = std::string(techniqueName);
			material.materialIterm.technique.techName +="::TNoShader";
		}
		else if (strcmp(techniqueName, "<ModelAlphaTech>")==0) {
			material.materialIterm.technique.techName = std::string(techniqueName);
			material.materialIterm.technique.techName += "<ModelNormalTech>::TNoShader";
		}
		
	}
	//Texture
	XMLElement* ParamTable = curMaterial->FirstChildElement("ParamTable");
	XMLElement* Tex0 = ParamTable->FirstChildElement("Tex0");
	if (Tex0 != NULL)
		material.materialIterm.paramTable.tex0Value = Tex0->FindAttribute("Value")->Value();
	XMLElement* Tex1 = ParamTable->FirstChildElement("Tex1");
	if (Tex1 != NULL)
		material.materialIterm.paramTable.tex1Value = Tex1->FindAttribute("Value")->Value();
	/*should parse Texture here???????*/

	//LightingMaterial
	XMLElement* LightingMaterial = curMaterial->FirstChildElement("LightingMaterial");
	if (LightingMaterial != NULL) {
		const char * Diffuse = LightingMaterial->FindAttribute("Diffuse")->Value();
		material.materialIterm.lightMaterial.diffuse = lightHelper(std::string(Diffuse));
		const char * Ambient = LightingMaterial->FindAttribute("Ambient")->Value();
		material.materialIterm.lightMaterial.ambient = lightHelper(std::string(Ambient));
		const char * Specular = LightingMaterial->FindAttribute("Specular")->Value();
		material.materialIterm.lightMaterial.specular = lightHelper(std::string(Specular));
		const char * Emissive = LightingMaterial->FindAttribute("Emissive")->Value();
		material.materialIterm.lightMaterial.emissive = lightHelper(std::string(Emissive));
		material.materialIterm.lightMaterial.power = atof(LightingMaterial->FindAttribute("Power")->Value());
	}
	//RenderStates
	XMLElement* RenderStates = curMaterial->FirstChildElement("RenderStates");
	const char *LightingEnable = RenderStates->FindAttribute("LightingEnable")->Value();
	if (strcmp(LightingEnable, "False") == 0)
		material.materialIterm.renderStates.lightingEnable = false;
	else
		material.materialIterm.renderStates.lightingEnable = true;
	const char *SpecularEnable = RenderStates->FindAttribute("SpecularEnable")->Value();
	if (strcmp(SpecularEnable, "False") == 0)
		material.materialIterm.renderStates.specularEnable = false;
	else
		material.materialIterm.renderStates.specularEnable = true;
	const char *VColorEnable = RenderStates->FindAttribute("VColorEnable")->Value();
	if (strcmp(VColorEnable, "False") == 0)
		material.materialIterm.renderStates.vcolorEnable= false;
	else
		material.materialIterm.renderStates.vcolorEnable = true;

	const XMLAttribute* ate = RenderStates->FindAttribute("AlphaTestEnable");
	if (ate != NULL) {//This attribute may not exist
		if (strcmp(ate->Value(), "False") == 0)
			material.materialIterm.renderStates.alpTestEnable = false;
		else
			material.materialIterm.renderStates.alpTestEnable = true;
	}
	const XMLAttribute* ar = RenderStates->FindAttribute("AlphaRef");
	if (ar != NULL) {
		material.materialIterm.renderStates.isAlphaRefTest = true;
		material.materialIterm.renderStates.alphaRef = atoi(ar->Value());
	}
	const XMLAttribute* av = RenderStates->FindAttribute("AlphaVal");
	if (av != NULL) {
		material.materialIterm.renderStates.isAlphaValSet = true;
		material.materialIterm.renderStates.alphaVal = atoi(av->Value());
		material.materialIterm.renderStates.dirty = true;
	}
	const char *CullBack = RenderStates->FindAttribute("CullBack")->Value();
	if (strcmp(CullBack, "False") == 0)
		material.materialIterm.renderStates.cullBack = false;
	else
		material.materialIterm.renderStates.cullBack = true;
	XMLElement* TransparentMode = RenderStates->FirstChildElement("TransparentMode");
	if (TransparentMode != NULL)
		material.materialIterm.renderStates.transparentMode = atoi(TransparentMode->FindAttribute("TransparentMode")->Value());
	
	
	return true;
}

int UNeoxMaterial::versionHelper(std::string str)
{
	std::vector<int>buffer;
	int front = 0;
	int back = str.find_first_of(',', front);
	while (back != std::string::npos)
	{
		buffer.push_back(atoi(str.substr(front, back - front).c_str()));
		front = back + 1;
		back = str.find_first_of(',', front);
	}
	buffer.push_back(atoi(str.substr(front, back - front).c_str()));
	reverse(buffer.begin(),buffer.end());
	unsigned char*a = new unsigned char[4];
	for (int i = 0; i < buffer.size(); i++)
		a[i] = buffer[i];
	unsigned int result;
	memcpy(&result, a, 4);
	return result;
}

LightIterm UNeoxMaterial::lightHelper(std::string str)
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
	if (buffer.size() < 4)
		return LightIterm();
	return LightIterm(buffer[0],buffer[1],buffer[2],buffer[3]);
}
