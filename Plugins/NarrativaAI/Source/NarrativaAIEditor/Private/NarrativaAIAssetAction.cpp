#include "NarrativaAIAssetAction.h"

#include "NarrativaAIAsset.h"
#include "NarrativaAIAssetEditorApp.h"

FNarrativaAIAssetAction::FNarrativaAIAssetAction(EAssetTypeCategories::Type Category)
{
	AssetCategory = Category;
}

FText FNarrativaAIAssetAction::GetName() const
{
	return FText::FromString(TEXT("Dialogue"));
}

FColor FNarrativaAIAssetAction::GetTypeColor() const
{
	return FColor(0, 168, 107);
}

UClass* FNarrativaAIAssetAction::GetSupportedClass() const
{
	return UNarrativaAIAsset::StaticClass();
}

void FNarrativaAIAssetAction::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (UObject* Asset : InObjects)
	{
		UNarrativaAIAsset* NarrativaAIAsset = Cast<UNarrativaAIAsset>(Asset);
		if (NarrativaAIAsset)
		{
			TSharedPtr<FNarrativaAIAssetEditorApp> EditorApp(new FNarrativaAIAssetEditorApp());
			EditorApp->InitEditor(Mode, EditWithinLevelEditor, NarrativaAIAsset);
		}
	}
}

uint32 FNarrativaAIAssetAction::GetCategories()
{
	return AssetCategory;
}
