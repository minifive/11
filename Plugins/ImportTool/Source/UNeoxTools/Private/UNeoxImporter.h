#pragma once
#include "UNeoxMeshImport.h"
#include "Engine/StaticMesh.h"
#include "UNeoxMesh.h"
#include "UNeoxGim.h"
class UNeoxImporter {
public:
	UNeoxImporter();

	//Proxy to import UNeoxMesh obeject to UE 
	UStaticMesh* UMeshImport(UObject* InParent, const FName& Name, EObjectFlags Flags, const FString& Filename, UStaticMesh* InStaticMesh, void *ExistMeshDataPtr);

	//Proxy to import a neox gim file
	UStaticMesh* UGimImport(UObject* InParent, const FName& Name, EObjectFlags Flags, const FString& Filename, UStaticMesh* InStaticMesh, void *ExistMeshDataPtr);
private:
	//UneoxMeshImport  UMeshImporter;
};