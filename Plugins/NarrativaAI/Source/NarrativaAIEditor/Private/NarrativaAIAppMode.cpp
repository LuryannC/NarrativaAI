#include "NarrativaAIAppMode.h"

#include "NarrativaAIAssetEditorApp.h"
#include "NarrativaAIGraphTabFactory.h"
#include "NarrativaAIPropertiesTabFactory.h"

FNarrativaAIAppMode::FNarrativaAIAppMode(TSharedPtr<class FNarrativaAIAssetEditorApp> App) : FApplicationMode(TEXT("NarrativaAIAssetAppMode"))
{
	EditorApp = App;
	Tabs.RegisterFactory(MakeShareable(new FNarrativaAIGraphTabFactory(App)));
	Tabs.RegisterFactory(MakeShareable(new FNarrativaAIPropertiesTabFactory(App)));

	 TabLayout = FTabManager::NewLayout("NarrativaAIAssetAppModeLayout")->AddArea
	 (
	 	FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
	 	->Split
	 	(
	 		FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
	 		->Split
	 		(
	 			FTabManager::NewStack()
	 			->SetSizeCoefficient(0.75)
	 			->AddTab(FName(TEXT("NarrativaAIGraphTab")), ETabState::OpenedTab)
	 		)
	 		->Split
	 		(
	 			FTabManager::NewStack()
	 			->SetSizeCoefficient(0.25)
	 			->AddTab(FName(TEXT("NarrativaAIPropertiesTab")), ETabState::OpenedTab)
	 		)
	 	)
	 );
}

void FNarrativaAIAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FNarrativaAIAssetEditorApp> App = EditorApp.Pin();
	App->PushTabFactories(Tabs);
	FApplicationMode::RegisterTabFactories(InTabManager);	
}

void FNarrativaAIAppMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();
}

void FNarrativaAIAppMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}
