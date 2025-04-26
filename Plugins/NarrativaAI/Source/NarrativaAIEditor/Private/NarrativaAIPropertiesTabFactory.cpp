#include "NarrativaAIPropertiesTabFactory.h"
#include "NarrativaAIAssetEditorApp.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "GraphEditor.h"
#include "NarrativaAIAsset.h"

FNarrativaAIPropertiesTabFactory::FNarrativaAIPropertiesTabFactory(TSharedPtr<class FNarrativaAIAssetEditorApp> App) : FWorkflowTabFactory(FName(TEXT("NarrativaAIPropertiesTab")), App)
{
	EditorApp = App;
	TabLabel = FText::FromString(TEXT("Properties"));
	ViewMenuDescription = FText::FromString(TEXT("Displays a properties view for current asset."));
	ViewMenuTooltip = FText::FromString(TEXT("Show the properties view"));
}

TSharedRef<SWidget> FNarrativaAIPropertiesTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{

	TSharedPtr<FNarrativaAIAssetEditorApp> App = EditorApp.Pin();
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

	// Asset details view settings
	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.bAllowSearch = false;
		DetailsViewArgs.bHideSelectionTip = true;
		DetailsViewArgs.bLockable = false;
		DetailsViewArgs.bSearchInitialKeyFocus = true;
		DetailsViewArgs.bUpdatesFromSelection = false;
		DetailsViewArgs.NotifyHook = nullptr;
		DetailsViewArgs.bShowOptions = true;
		DetailsViewArgs.bShowModifiedPropertiesOption = false;
		DetailsViewArgs.bShowScrollBar = false;
	}

	TSharedPtr<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObject(App->GetCurrentWorkingOnAsset());
	
	// Selected node details view
	FDetailsViewArgs SelectedNodeArgs = DetailsViewArgs;	

	TSharedPtr<IDetailsView> SelectedNodeDetailsView = PropertyEditorModule.CreateDetailView(SelectedNodeArgs);
	SelectedNodeDetailsView->SetObject(nullptr);
	App->SetSelectedNodeDetailView(SelectedNodeDetailsView);
	
	return SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			DetailsView.ToSharedRef()
		]
		+SVerticalBox::Slot()
		.FillHeight(1.0f)
		.HAlign(HAlign_Fill)
		[
			SelectedNodeDetailsView.ToSharedRef()
		];
}

FText FNarrativaAIPropertiesTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("A properties view for the current asset."));
}
