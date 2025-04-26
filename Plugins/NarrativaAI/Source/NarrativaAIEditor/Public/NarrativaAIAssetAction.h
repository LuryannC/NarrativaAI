#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

/*
 * Handles what happens when opening the NarrativeAI Asset
 */
class NARRATIVAAIEDITOR_API FNarrativaAIAssetAction : public FAssetTypeActions_Base
{
public:
	FNarrativaAIAssetAction(EAssetTypeCategories::Type Category);

public:
	/* FAssetTypeActions_Base interface */
	virtual FText GetName() const override;
	virtual FColor GetTypeColor() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual void OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor) override;
	virtual uint32 GetCategories() override;
	/* End interface */

private:
	EAssetTypeCategories::Type AssetCategory;
};
