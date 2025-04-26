#include "NarrativaAIGraphTabFactory.h"
#include "NarrativaAIAssetEditorApp.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "GraphEditor.h"
#include "SBlueprintEditorToolbar.h"

FNarrativaAIGraphTabFactory::FNarrativaAIGraphTabFactory(TSharedPtr<class FNarrativaAIAssetEditorApp> App) : FWorkflowTabFactory(FName(TEXT("NarrativaAIGraphTab")), App)
{
	EditorApp = App;
	TabLabel = FText::FromString(TEXT("Dialogue Graph"));
	ViewMenuDescription = FText::FromString(TEXT("Displays the graph tab"));
	ViewMenuTooltip = FText::FromString(TEXT("Show the graph view"));
}

TSharedRef<SWidget> FNarrativaAIGraphTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	TSharedPtr<FNarrativaAIAssetEditorApp> App = EditorApp.Pin();

	SGraphEditor::FGraphEditorEvents GraphEditorEvents;
	GraphEditorEvents.OnSelectionChanged.BindRaw(App.Get(), &FNarrativaAIAssetEditorApp::OnGraphSelectionChanged);
		
	TSharedPtr<SGraphEditor> GraphEditor = SNew(SGraphEditor)
		.IsEditable(true)
		.AdditionalCommands(App->GraphEditorCommands.ToSharedRef()) // Initialize custom commands - NOT WORKING
		.GraphEvents(GraphEditorEvents)
		.GraphToEdit(App->GetCurrentWorkingOnGraph());
	
	App->SetCurrentWorkingOnGraphUI(GraphEditor);

	return SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				.HAlign(HAlign_Fill)
				[
					GraphEditor.ToSharedRef()
				];
}

FText FNarrativaAIGraphTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("Graph view of the nodes tree"));
}
