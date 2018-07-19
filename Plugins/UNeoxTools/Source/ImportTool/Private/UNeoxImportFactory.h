
#pragma once
#include "Factories/Factory.h"
#include "UNeoxMesh.h"
#include "UNeoxImportFactory.generated.h"

UCLASS()
class  UNeoxImportFactory : public UFactory
{
	GENERATED_UCLASS_BODY()

public:
	// UFactory Interface
	//void EnableShowOption() { bShowOption = true; }
	//virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual UObject* ImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, bool& OutCanceled)override;
	virtual void CleanUp() override;

private:
	//UPROPERTY()
	//FUSDAssetImportContext ImportContext;
};