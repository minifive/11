#include "UNeoxImportPCH.h"
#include "CoreMinimal.h"
#include "UNeoxMeshImport.h"
#include "Engine/StaticMesh.h"
#include "ObjectTools.h"
#include "PackageTools.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionVectorParameter.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Factories/MaterialFactoryNew.h"
#include "Factories/TextureFactory.h"
#include "AssetRegistryModule.h"
#include "RawMesh.h"
#include "ITargetPlatform.h"
#include "ITargetPlatformManagerModule.h" 

UneoxMeshImport::UneoxMeshImport() {
	UMesh = NULL;
	uneoxMaterial = NULL;
	curSubIndex = 0;
}

UStaticMesh* UneoxMeshImport::ImportStaticMesh(UObject* InParent, const FName& Name, EObjectFlags Flags, UStaticMesh* InStaticMesh, int LODIndex, void *ExistMeshDataPtr) {

	if (UMesh == NULL)
		return NULL;
	FString MeshName = ObjectTools::SanitizeObjectName(Name.ToString());
	UPackage* Package = NULL;
	if ( InParent != nullptr && InParent->IsA(UPackage::StaticClass()))	
	{
		Package = StaticCast<UPackage*>(InParent);
	}
	// create a empty staticmesh
	UStaticMesh*StaticMesh = NULL;
	//there may be some assets has same name
	FName StaticMeshName = MakeUniqueObjectName(Package, UStaticMesh::StaticClass(), FName(*MeshName));
	if (InStaticMesh != NULL && LODIndex > 0)
	{
		StaticMesh = InStaticMesh;//在当前InstaticMesh上进行操作
	}
	else
	{
		StaticMesh = NewObject<UStaticMesh>(Package, StaticMeshName, Flags | RF_Public);
	}

	//set LOD index
	if (StaticMesh->SourceModels.Num() < LODIndex + 1)
	{
		// Add one LOD 
		new(StaticMesh->SourceModels) FStaticMeshSourceModel();
	}
	FStaticMeshSourceModel& SrcModel = StaticMesh->SourceModels[LODIndex];
	if (InStaticMesh != NULL && LODIndex > 0 && !SrcModel.RawMeshBulkData->IsEmpty())
	{
		// clear out the old mesh data
		FRawMesh EmptyRawMesh;
		SrcModel.RawMeshBulkData->SaveRawMesh(EmptyRawMesh);
	}

	// make sure it has a new lighting guid
	//StaticMesh->LightingGuid = FGuid::NewGuid();
	//// Set it to use textured lightmaps. Note that Build Lighting will do the error-checking (texcoordindex exists for all LODs, etc).
	//StaticMesh->LightMapResolution = 64;
	//StaticMesh->LightMapCoordinateIndex = 1;
 
	//Build RawMesh
	 FRawMesh NewRawMesh;
	 SrcModel.RawMeshBulkData->LoadRawMesh(NewRawMesh);
	 BuildRawMesh(UMesh,StaticMesh,LODIndex,NewRawMesh);
	 SrcModel.RawMeshBulkData->SaveRawMesh(NewRawMesh);//store the NewRawMesh build from above function

	 //Create material
	 TArray<UMaterialInterface*> Materials;
	 CreatMaterials(Package,uneoxMaterial, Materials,InParent,Flags);
	 int numberOfMaterials = Materials.Num();

	 //assign the material with staticmesh
	 for (int32 MaterialID = 0; MaterialID < numberOfMaterials; MaterialID++) {//理论上一个uneox submesh 对应一种材质
		 //bounding material to submesh
		 FStaticMaterial curM = FStaticMaterial(Materials[MaterialID]);
		 StaticMesh->StaticMaterials.Add(curM);
		 StaticMesh->SectionInfoMap.Set(0, MaterialID, FMeshSectionInfo(MaterialID));//通过LoD和MaterialID 映射到 FMeshSectionInfo
	 }

	 //section materials
	 FRawMesh curRawMesh;
	 SrcModel.RawMeshBulkData->LoadRawMesh(curRawMesh);
	 int faceOffSet = 0;
	 for (int materialIndex = 0; materialIndex < numberOfMaterials; materialIndex++) {
		 int subFaceNum = this->UMesh->_sub_mesh_info[materialIndex].triangle_count;// faces in this neox sub mesh
		 for(int faceIndex=0;faceIndex<subFaceNum;faceIndex++)
			curRawMesh.FaceMaterialIndices[faceIndex+faceOffSet]=materialIndex;
		 faceOffSet += subFaceNum;
	 }
	 SrcModel.RawMeshBulkData->SaveRawMesh(curRawMesh);


	 // Setup default LOD settings based on the selected LOD group.
	 if (LODIndex == 0) {
		 ITargetPlatform* CurrentPlatform = GetTargetPlatformManagerRef().GetRunningTargetPlatform();
		 check(CurrentPlatform);
		 const FStaticMeshLODGroup& LODGroup = CurrentPlatform->GetStaticMeshLODSettings().GetLODGroup("");
		 int32 NumLODs = LODGroup.GetDefaultNumLODs();
		 while (StaticMesh->SourceModels.Num() < NumLODs)
		 {
			 new (StaticMesh->SourceModels) FStaticMeshSourceModel();
		 }
		 for (int32 ModelLODIndex = 0; ModelLODIndex < NumLODs; ++ModelLODIndex)
		 {
			 StaticMesh->SourceModels[ModelLODIndex].ReductionSettings = LODGroup.GetDefaultSettings(ModelLODIndex);
		 }
		 StaticMesh->LightMapResolution = LODGroup.GetDefaultLightMapResolution();
	 }
	 TArray<FText> BuildErrors;
	 StaticMesh->Build(false, &BuildErrors);
	return StaticMesh;
}

UStaticMesh * UneoxMeshImport::BuildRawMesh(UNeoxMesh * UMesh, UStaticMesh* staticMesh, int32 LODIndex, FRawMesh& newRawMesh)
{

//	check(StaticMesh->SourceModels.IsValidIndex(LODIndex));
	check(staticMesh->SourceModels.Num()>LODIndex);
	FStaticMeshSourceModel& SrcModel = staticMesh->SourceModels[LODIndex];

	//init UNeoxMesh position
	//int verOffset = 0;
	//int tagOffset = 0;
	//for (int index = 0; index < curSubIndex; index++) {
	//	verOffset+= UMesh->_sub_mesh_info[index].v_count;	
	//	tagOffset += UMesh->_sub_mesh_info[index].triangle_count;
	//}

	int VertexCount = UMesh->_vertices.size();
	int TriangleCount = UMesh->_triangles.size();
	int WedgeCount = TriangleCount * 3;

	TArray<FVector> Vertices;
	TArray<FVector> Normals;
	TArray<FVector> Tangents;
	for (int i=0;i<VertexCount;i++)
	{
		FVector vertex = FVector(UMesh->_vertices[i].position.x, UMesh->_vertices[i].position.y, UMesh->_vertices[i].position.z);
		FVector normal = FVector(UMesh->_vertices[i].normal.x, UMesh->_vertices[i].normal.y, UMesh->_vertices[i].normal.z);
		FVector tangent = FVector(UMesh->_vertices[i].tangent.x, UMesh->_vertices[i].tangent.y, UMesh->_vertices[i].tangent.z);
		Vertices.Add(UNeoxToUE(vertex));
		Normals.Add(UNeoxToUE(normal));
		Tangents.Add(UNeoxToUE(tangent));
	}
	
	//init uneox triangle
	TArray<FVector>Triangles;
	for (int i = 0; i < TriangleCount+0; i++) {
		FVector triangle = FVector(UMesh->_triangles[i].a, UMesh->_triangles[i].b, UMesh->_triangles[i].c);
		Triangles.Add(UNeoxToUE(triangle));
	}

	// init RawMesh attributes.
	newRawMesh.FaceMaterialIndices.AddZeroed(TriangleCount);
	newRawMesh.FaceSmoothingMasks.AddZeroed(TriangleCount);
	newRawMesh.VertexPositions.AddZeroed(VertexCount);

	// init UVS At least one UVlayer set must exist.  
	int uvsLayers = 1;
	if(UMesh->_vertices.size()!=0)
		uvsLayers=UMesh->_vertices[0].uvs.size();
	for (int32 UVLayerIndex = 0; UVLayerIndex <uvsLayers; UVLayerIndex++)
	{
		newRawMesh.WedgeTexCoords[UVLayerIndex].AddZeroed(WedgeCount);
	}

	//vertices_position
	for (int verIndex = 0; verIndex < VertexCount; verIndex++) {
		newRawMesh.VertexPositions[verIndex]=Vertices[verIndex];
	}

	//Face
	int uvs = 0;
	for (int i = 0; i < TriangleCount; i++) {
		//triangles, Notice that neox is opposite to the normal line in UE4
		int aIndex = Triangles[i].Z;
		int bIndex = Triangles[i].Y;
		int cIndex = Triangles[i].X;
		
		//vertices
		newRawMesh.WedgeIndices.Add(aIndex);
		newRawMesh.WedgeIndices.Add(bIndex);
		newRawMesh.WedgeIndices.Add(cIndex);

		//uvs
		//int UVLayerIndex = UMesh->_vertices[aIndex].uvs.size();
		for (int uvLayerIndex = 0; uvLayerIndex < uvsLayers; uvLayerIndex++) {
			newRawMesh.WedgeTexCoords[uvLayerIndex][uvs].X = UMesh->_vertices[aIndex].uvs[uvLayerIndex].first;
			newRawMesh.WedgeTexCoords[uvLayerIndex][uvs].Y = UMesh->_vertices[aIndex].uvs[uvLayerIndex].second;
			newRawMesh.WedgeTexCoords[uvLayerIndex][uvs +1].X = UMesh->_vertices[bIndex].uvs[uvLayerIndex].first;
			newRawMesh.WedgeTexCoords[uvLayerIndex][uvs +1].Y = UMesh->_vertices[bIndex].uvs[uvLayerIndex].second;
			newRawMesh.WedgeTexCoords[uvLayerIndex][uvs +2].X = UMesh->_vertices[cIndex].uvs[uvLayerIndex].first;
			newRawMesh.WedgeTexCoords[uvLayerIndex][uvs +2].Y = UMesh->_vertices[cIndex].uvs[uvLayerIndex].second;
		}
		uvs = uvs + 3;
		//U tangents
		newRawMesh.WedgeTangentX.Add(Tangents[aIndex]);
		newRawMesh.WedgeTangentX.Add(Tangents[bIndex]);
		newRawMesh.WedgeTangentX.Add(Tangents[cIndex]);

		//V tangents, Note: neox not have this attibute
		newRawMesh.WedgeTangentY.Add(Tangents[aIndex]);
		newRawMesh.WedgeTangentY.Add(Tangents[bIndex]);
		newRawMesh.WedgeTangentY.Add(Tangents[cIndex]);

		//Normals
		newRawMesh.WedgeTangentZ.Add(Normals[aIndex]);
		newRawMesh.WedgeTangentZ.Add(Normals[bIndex]);
		newRawMesh.WedgeTangentZ.Add(Normals[cIndex]);

		//Colors  default White
		newRawMesh.WedgeColors.Add(FColor::White);
		newRawMesh.WedgeColors.Add(FColor::White);
		newRawMesh.WedgeColors.Add(FColor::White);	
	}
	return staticMesh;
}

void UneoxMeshImport::CreatMaterials(UPackage* Package, UNeoxMaterial*uneoxMaterial, TArray<UMaterialInterface*> &outMaterials, UObject* InParent, EObjectFlags Flags)
{
	if (uneoxMaterial == NULL)
		return;
	auto MaterialFactory = NewObject<UMaterialFactoryNew>();

	/*do some check is there already exit sam*/

	for (int i = 0; i < uneoxMaterial->_materialGroup.materialCount; ++i) {
		FString name(uneoxMaterial->_materialGroup.materials[i].materialIterm.name.c_str());
		FName materialName = MakeUniqueObjectName(Package, UStaticMesh::StaticClass(), FName(*name));
		
		//create a new material
		UMaterial* NewMaterial = (UMaterial*)MaterialFactory->FactoryCreateNew(UMaterial::StaticClass(), Package, materialName, RF_Standalone | RF_Public, NULL, GWarn);
		
		//creat texture
		FString tex0FilePath = TexPathHelper(uneoxMaterial->_filePath , uneoxMaterial->_materialGroup.materials[i].materialIterm.paramTable.tex0Value);
		UTexture* tex0 = CreatTexture(InParent, Flags, tex0FilePath);

		// An texture was specified, add it and assign it to the BaseColor
		if (tex0 != nullptr )
		{
			//basic color
			UMaterialExpressionTextureSample* TextureSampler = NewObject<UMaterialExpressionTextureSample>(NewMaterial);
			{
				TextureSampler->MaterialExpressionEditorX = -250;
				TextureSampler->Texture = tex0;
				TextureSampler->AutoSetSampleType();
			}
			NewMaterial->Expressions.Add(TextureSampler);
			FExpressionOutput& Output = TextureSampler->GetOutputs()[0];
			FExpressionInput& Input = (TextureSampler->SamplerType == SAMPLERTYPE_Normal)
				? (FExpressionInput&)NewMaterial->Normal
				: (FExpressionInput&)NewMaterial->BaseColor;

			Input.Expression = TextureSampler;
			Input.Mask = Output.Mask;
			Input.MaskR = Output.MaskR;
			Input.MaskG = Output.MaskG;
			Input.MaskB = Output.MaskB;
			Input.MaskA = Output.MaskA;
			NewMaterial->PostEditChange();
			//Diffuse


		}
		outMaterials.Add(NewMaterial);
	}

}

UTexture * UneoxMeshImport::CreatTexture(UObject * InParent, EObjectFlags Flags,const FString & Filename)
{
	UTexture* newTexture = nullptr;
	auto textureFactory= NewObject<UTextureFactory>();
	FString FileExtension = FPaths::GetExtension(Filename);
	FString name = FPaths::GetBaseFilename(Filename);
	FName texName= MakeUniqueObjectName(InParent, UStaticMesh::StaticClass(), FName(*name));
	//Data read from .tga 
	TArray<uint8>Data;
	if (!FFileHelper::LoadFileToArray(Data, *Filename))
	{
		UE_LOG(UNeoxLog, Error, TEXT("Failed to load file '%s' to array"), *Filename);
		return nullptr;
	}
	const uint8*Ptr = &Data[0];
	//UTexture created by UtextureFactory
	UObject* textureObject=textureFactory->FactoryCreateBinary(UTexture::StaticClass(),InParent,texName,Flags,nullptr,*FileExtension, Ptr, Ptr + Data.Num() - 1,nullptr);
	newTexture = Cast<UTexture>(textureObject);
	return newTexture;
}

FVector UneoxMeshImport::UNeoxToUE(FVector &point) {
	float tempx = point.X;
	point.X = point.Z;
	point.Z = point.Y;
	point.Y = tempx;
	return point;
}

FString UneoxMeshImport::TexPathHelper(std::string materiPath, std::string filePath)
{

	std::string texPath;
	//wipe off material name
	int back = materiPath.find("model");
	std::cout << back;
	texPath = materiPath.substr(0,back);
	std::string result = texPath + filePath;
	return FString(result.c_str());
}
