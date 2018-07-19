
#pragma once
#include "Object.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "UNeoxMesh.h"
#include "UNeoxMaterial.h"
class UneoxMeshImport
{
public:
	UneoxMeshImport();
	//import static mesh function
	UStaticMesh* ImportStaticMesh(UObject* InParent, const FName& Name, EObjectFlags Flags, UStaticMesh* InStaticMesh, int LODIndex, void *ExistMeshDataPtr);
	//build raw Mesh, set vertices¡¢ Normal¡¢tangent...to rawmesh
	UStaticMesh* BuildRawMesh(UNeoxMesh * UMesh, UStaticMesh* staticMesh, int32 LODIndex, FRawMesh& RawMesh);
	//transform neox material & add to staticmesh 
	void CreatMaterials(UPackage* Package, UNeoxMaterial*uneoxMaterial,TArray<UMaterialInterface*> &outMaterials, UObject* InParent, EObjectFlags Flags);
	//read .tga file from neox and return UTexture;
	UTexture* CreatTexture(UObject * InParent, EObjectFlags Flags, const FString & Filename);
	// Transform coordinate Neox.x->UE.y; Neox.y->UE.z; Neox.z->UE.x
	FVector UNeoxToUE(FVector &point);
	FString TexPathHelper(std::string materiPath,std::string filePath);

public:
	UNeoxMaterial* uneoxMaterial;
	UNeoxMesh*UMesh;
	int curSubIndex;
};