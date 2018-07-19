//Fill out your copyright notice in the Description page of Project Settings.
#include "UNeoxImportPCH.h"
#include "UNeoxImportFactory.h"
#include "ActorFactories/ActorFactoryStaticMesh.h"
#include "Engine/StaticMesh.h"
#include "Object.h"
#include "UNeoxImporter.h"

UNeoxImportFactory::UNeoxImportFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bCreateNew = false;
	bEditAfterNew = true;//
	SupportedClass = UStaticMesh::StaticClass();

	bEditorImport = true;
	bText = false;
	
	Formats.Add(TEXT("gim;Neox gim files"));
}

bool UNeoxImportFactory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	if (Extension == TEXT("gim"))
	{
		return true;
	}

	return false;
}
UObject* UNeoxImportFactory::ImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, bool& OutCanceled) {

	
	UNeoxImporter* UImporter = new UNeoxImporter();
	//return UImporter->UMeshImport(InOuter, InName, Flags, Filename, NULL, NULL);//test mesh
	//return ui->CreatTexture(InOuter, InName, Flags, Filename, NULL);// test tga;

	return UImporter->UGimImport(InOuter, InName, Flags, Filename, NULL, NULL);
}
void UNeoxImportFactory::CleanUp()
{

}
