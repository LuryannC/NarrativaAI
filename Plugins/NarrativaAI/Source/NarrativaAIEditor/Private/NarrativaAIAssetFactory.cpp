#include "NarrativaAIAssetFactory.h"

#include "NarrativaAIAsset.h"

UNarrativaAIAssetFactory::UNarrativaAIAssetFactory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SupportedClass = UNarrativaAIAsset::StaticClass();
}

UObject* UNarrativaAIAssetFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName,
	EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	UNarrativaAIAsset* Asset = NewObject<UNarrativaAIAsset>(InParent, InName, Flags);
	return Asset;
}

bool UNarrativaAIAssetFactory::CanCreateNew() const
{
	return true;
}
