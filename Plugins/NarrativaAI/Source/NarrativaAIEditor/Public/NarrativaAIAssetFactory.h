#pragma once

#include "CoreMinimal.h"
#include "NarrativaAIAssetFactory.generated.h"


UCLASS()
class NARRATIVAAIEDITOR_API UNarrativaAIAssetFactory : public UFactory
{
	GENERATED_BODY()
	
public:
	UNarrativaAIAssetFactory(const FObjectInitializer& ObjectInitializer);

	/* Begin Factory interface*/
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
	virtual bool CanCreateNew() const override;
	/* End interface*/
};
