#include "UNeoxImportPCH.h"
#include "UNeoxImporter.h"

DEFINE_LOG_CATEGORY(UNeoxLog);

UNeoxImporter::UNeoxImporter() {
	
}

UStaticMesh* UNeoxImporter::UMeshImport(UObject* InParent, const FName& Name, EObjectFlags Flags, const FString& Filename, UStaticMesh* InStaticMesh, void *ExistMeshDataPtr)
{
	UNeoxMesh*UMesh = new UNeoxMesh();
	UMesh->init(TCHAR_TO_UTF8(*Filename), 1);
	if(!UMesh->readMesh())
		return nullptr;
	UneoxMeshImport *UMeshImporter=new UneoxMeshImport();
	UMeshImporter->UMesh = UMesh;
	return UMeshImporter->ImportStaticMesh(InParent,Name,Flags ,InStaticMesh,0,ExistMeshDataPtr);
}

UStaticMesh* UNeoxImporter::UGimImport(UObject* InParent, const FName& Name, EObjectFlags Flags, const FString& Filename, UStaticMesh* InStaticMesh, void *ExistMeshDataPtr)
{
	UNeoxGim *UGim = new UNeoxGim();
	UGim->init(TCHAR_TO_UTF8(*Filename));

	//to read file and then create NeoxMesh, NeoxMaterial
	if (!UGim->readGim()) {
		UE_LOG(UNeoxLog, Warning, TEXT("read gim file failed , check whether %s is legal..\n"), UGim->_gimFile.c_str());
		return nullptr;
	}
		
	if (!UGim->readMesh()) {
		UE_LOG(UNeoxLog, Warning, TEXT("read mesh file failed , check whether %s is legal..\n") ,UGim->_meshFile.c_str());
		return nullptr;
	}
		
	if (!UGim->readMaterial()) {
		UE_LOG(UNeoxLog, Warning, TEXT("read material file failed , check whether %s is legal..\n"), UGim->_mtgFile.c_str());
		return nullptr;
	}
	
	UneoxMeshImport *UMeshImporter = new UneoxMeshImport();
	UMeshImporter->UMesh = UGim->UMesh;                         //assign neox mesh
	UMeshImporter->uneoxMaterial = UGim->UMaterial;             //assign neox material
	
	UStaticMesh* UEMesh = UMeshImporter->ImportStaticMesh(InParent, Name, Flags, InStaticMesh, 0, ExistMeshDataPtr);

	FString path = UEMesh->GetPathName();
	FString name = UEMesh->GetName();

	return UEMesh;
}
