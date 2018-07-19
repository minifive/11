// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Private/UNeoxImportPCH.h"
#include "Private/UNeoxImportFactory.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeUNeoxImportFactory() {}
// Cross Module References
	IMPORTTOOL_API UClass* Z_Construct_UClass_UNeoxImportFactory_NoRegister();
	IMPORTTOOL_API UClass* Z_Construct_UClass_UNeoxImportFactory();
	UNREALED_API UClass* Z_Construct_UClass_UFactory();
	UPackage* Z_Construct_UPackage__Script_ImportTool();
// End Cross Module References
	void UNeoxImportFactory::StaticRegisterNativesUNeoxImportFactory()
	{
	}
	UClass* Z_Construct_UClass_UNeoxImportFactory_NoRegister()
	{
		return UNeoxImportFactory::StaticClass();
	}
	UClass* Z_Construct_UClass_UNeoxImportFactory()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_UFactory,
				(UObject* (*)())Z_Construct_UPackage__Script_ImportTool,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "IncludePath", "UNeoxImportFactory.h" },
				{ "ModuleRelativePath", "Private/UNeoxImportFactory.h" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<UNeoxImportFactory>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&UNeoxImportFactory::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00000080u,
				nullptr, 0,
				nullptr, 0,
				nullptr,
				&StaticCppClassTypeInfo,
				nullptr, 0,
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(UNeoxImportFactory, 88986549);
	static FCompiledInDefer Z_CompiledInDefer_UClass_UNeoxImportFactory(Z_Construct_UClass_UNeoxImportFactory, &UNeoxImportFactory::StaticClass, TEXT("/Script/ImportTool"), TEXT("UNeoxImportFactory"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(UNeoxImportFactory);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
