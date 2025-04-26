#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class NARRATIVAAIEDITOR_API FNarrativaAIAssetEditorApp : public FWorkflowCentricApplication, FEditorUndoClient, FNotifyHook
{
public:
	/* IToolkit Interface */
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;

	/* FAssetEditorToolkit interface */
	virtual FName GetToolkitFName() const override { return TEXT("NarrativaAIAssetEditorApp"); };
	virtual FText GetBaseToolkitName() const override { return FText::FromName(TEXT("NarrativaAIAssetEditorApp")); }
	virtual FString GetWorldCentricTabPrefix() const override { return TEXT("NarrativaAIAssetEditorApp"); }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor(0, 0.658f, 0.419f ); } ;
	virtual FString GetDocumentationLink() const override { return TEXT("https://github.com/LuryannC"); }
	virtual void OnToolkitHostingStarted(const TSharedRef<IToolkit>& Toolkit) override { } 
	virtual void OnToolkitHostingFinished(const TSharedRef<IToolkit>& Toolkit) override { }
	/* End interface */

	/* FAssetEditorToolkit interface */
	virtual void OnClose() override;
	virtual void SaveAsset_Execute() override;
	/* End interface */

	/* Plugin's interface */
	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object);
	void OnWorkingOnAssetPreSave();
	
	/* Data */
	class UNarrativaAIAsset* GetCurrentWorkingOnAsset() { return CurrentWorkingOnAsset; }
	class UEdGraph* GetCurrentWorkingOnGraph() { return CurrentWorkingOnGraph; }

	/* UI */
	void SetCurrentWorkingOnGraphUI(TSharedPtr<SGraphEditor> InCurrentWorkingOnGraphUI) { CurrentWorkingOnGraphUI = InCurrentWorkingOnGraphUI; }
	void OnGraphSelectionChanged(const FGraphPanelSelectionSet& Selection);
	void SetSelectedNodeDetailView(TSharedPtr<class IDetailsView> DetailsView);

	// Properties
	void OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& Event);

	//Commands
	TSharedPtr<FUICommandList> GraphEditorCommands;
protected:
	void UpdateWorkingOnAssetFromGraph();
	void UpdateEditorGraphFromWorkingOnAsset();
	class UNarrativaAINodeBase* GetSelectedNode(const FGraphPanelSelectionSet& Selection);

	//Commands
	void SetupCommands();
	void DeleteSelectedNodes();
	bool CanDeleteNodes();

private:
	class UNarrativaAIAsset* CurrentWorkingOnAsset = nullptr;

	class UEdGraph* CurrentWorkingOnGraph = nullptr;

	// Working graph UI (the slate widget that displays the graph)
	TSharedPtr<SGraphEditor> CurrentWorkingOnGraphUI = nullptr;

	TSharedPtr<IDetailsView> SelectedNodeDetailView = nullptr;
	
	TSet<UObject*> SelectedNodes;
};
