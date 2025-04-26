#include "NarrativaAIAssetEditorApp.h"

#include "NarrativaAIAINode.h"
#include "NarrativaAIAppMode.h"
#include "NarrativaAIAsset.h"
#include "NarrativaAIGraphSchema.h"
#include "NarrativaAIHumanNode.h"
#include "NarrativaAIReplyNode.h"
#include "NarrativaAIStartNode.h"
#include "Framework/Commands/GenericCommands.h"
#include "Kismet2/BlueprintEditorUtils.h"

DEFINE_LOG_CATEGORY_STATIC(FNarrativaAIAssetEditorAppSub, Log, All);

void FNarrativaAIAssetEditorApp::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FWorkflowCentricApplication::RegisterTabSpawners(InTabManager);
}

void FNarrativaAIAssetEditorApp::InitEditor(const EToolkitMode::Type Mode,
	const TSharedPtr<class IToolkitHost>& InitToolkitHost, UObject* Object)
{
	/* Commands */
	SetupCommands();
	
	TArray<UObject*> ObjectsToEdit;
	ObjectsToEdit.Add(Object);

	CurrentWorkingOnAsset = Cast<UNarrativaAIAsset>(Object);
	CurrentWorkingOnAsset->SetPreSaveListener([this](){ OnWorkingOnAssetPreSave(); });

	CurrentWorkingOnGraph = FBlueprintEditorUtils::CreateNewGraph(CurrentWorkingOnAsset, NAME_None, UEdGraph::StaticClass(), UNarrativaAIGraphSchema::StaticClass() );

	InitAssetEditor(Mode, InitToolkitHost, TEXT("NarrativaAIAssetEditorApp"), FTabManager::FLayout::NullLayout, true, true, ObjectsToEdit);

	AddApplicationMode(TEXT("NarrativaAIAssetAppMode"), MakeShareable(new FNarrativaAIAppMode(SharedThis(this))));

	SetCurrentMode(TEXT("NarrativaAIAssetAppMode"));

	/* Load nodes from the current working on asset */
	UpdateEditorGraphFromWorkingOnAsset();

	
}

void FNarrativaAIAssetEditorApp::OnClose()
{
	UpdateWorkingOnAssetFromGraph();

	CurrentWorkingOnAsset->SetPreSaveListener(nullptr);
	
	FWorkflowCentricApplication::OnClose();
}

void FNarrativaAIAssetEditorApp::SaveAsset_Execute()
{
	UpdateWorkingOnAssetFromGraph();
	FWorkflowCentricApplication::SaveAsset_Execute();
}

void FNarrativaAIAssetEditorApp::UpdateWorkingOnAssetFromGraph()
{
	if (!GetCurrentWorkingOnGraph() || !GetCurrentWorkingOnGraph())
	{
		return;
	}

	UNarrativaAIRuntimeGraph* RuntimeGraph = NewObject<UNarrativaAIRuntimeGraph>(CurrentWorkingOnAsset);
	CurrentWorkingOnAsset->Graph = RuntimeGraph;

	TArray<std::pair<FGuid, FGuid>> Connections;
	TMap<FGuid, UNarrativaAIRuntimePin*> IDToPinMap;

	for (UEdGraphNode* UINode : CurrentWorkingOnGraph->Nodes)
	{
		UNarrativaAIRuntimeNode* RuntimeNode = NewObject<UNarrativaAIRuntimeNode>(CurrentWorkingOnGraph);
		RuntimeNode->Position = FVector2D(UINode->NodePosX, UINode->NodePosY);

		for (UEdGraphPin* Pin : UINode->Pins)
		{
			UNarrativaAIRuntimePin* RuntimePin = NewObject<UNarrativaAIRuntimePin>(RuntimeNode);
			RuntimePin->PinName = Pin->PinName;
			RuntimePin->PinId = Pin->PinId;
			RuntimePin->Parent = RuntimeNode;

			if (Pin->HasAnyConnections() && Pin->Direction == EGPD_Output)
			{
				for (int i = 0; i < Pin->LinkedTo.Num(); ++i)
				{
					std::pair<FGuid, FGuid> Connection = std::make_pair(Pin->PinId, Pin->LinkedTo[i]->PinId);
					Connections.Add(Connection);
				}
			}

			IDToPinMap.Add(Pin->PinId, RuntimePin);
			if (Pin->Direction == EGPD_Input)
			{
				RuntimeNode->InputPin = RuntimePin;
			}
			else
			{
				RuntimeNode->OutputPins.Add(RuntimePin);
			}
		}

		UNarrativaAINodeBase* UINodeBase = Cast<UNarrativaAINodeBase>(UINode);
		RuntimeNode->NodeInfo = DuplicateObject(UINodeBase->GetNodeInfo(), RuntimeNode);
		RuntimeNode->NodeType = UINodeBase->GetNarrativaAINodeType();

		RuntimeGraph->Nodes.Add(RuntimeNode);
	}
	for (std::pair<FGuid, FGuid> Connection : Connections)
	{
		UNarrativaAIRuntimePin* PinA = IDToPinMap[Connection.first];
		UNarrativaAIRuntimePin* PinB = IDToPinMap[Connection.second];
		PinA->Connections.AddUnique(PinB);
	}	
}

void FNarrativaAIAssetEditorApp::UpdateEditorGraphFromWorkingOnAsset()
{
	if (!CurrentWorkingOnAsset->Graph)
	{
		// Adds defaults nodes, like the start one.
		CurrentWorkingOnGraph->GetSchema()->CreateDefaultNodesForGraph(*CurrentWorkingOnGraph);
		return;
	}

	TArray<std::pair<FGuid, FGuid>> Connections;
	TMap<FGuid, UEdGraphPin*> IDToPinMap;

	for (UNarrativaAIRuntimeNode* RuntimeNode : CurrentWorkingOnAsset->Graph->Nodes)
	{
		UNarrativaAINodeBase* NewNode = nullptr;
		if (RuntimeNode->NodeType == ENarrativaAINodeType::StartNode)
		{
			NewNode = NewObject<UNarrativaAIStartNode>(CurrentWorkingOnGraph);
		}
		else if (RuntimeNode->NodeType == ENarrativaAINodeType::AINode)
		{
			NewNode = NewObject<UNarrativaAIAINode>(CurrentWorkingOnGraph);
		}
		else if (RuntimeNode->NodeType == ENarrativaAINodeType::HumanNode)
		{
			NewNode = NewObject<UNarrativaAIHumanNode>(CurrentWorkingOnGraph);
		}
		else if (RuntimeNode->NodeType == ENarrativaAINodeType::NPCReply)
		{
			NewNode = NewObject<UNarrativaAIReplyNode>(CurrentWorkingOnGraph);
		}
		else if (RuntimeNode->NodeType == ENarrativaAINodeType::EndNode)
		{
			UE_LOG(FNarrativaAIAssetEditorAppSub, Display, TEXT("FNarrativaAIAssetEditorApp::UpdateWorkingOnAssetFromGraph - Tried to make an End-Node"));
		}
		else if (RuntimeNode->NodeType == ENarrativaAINodeType::Unknown)
		{
			UE_LOG(FNarrativaAIAssetEditorAppSub, Display, TEXT("FNarrativaAIAssetEditorApp::UpdateWorkingOnAssetFromGraph - Tried to make an Unknow-Node"));
		}

		/* As I don't need to save the Guid right now I will generate a new one
		* but if needed can be saved in the graph class
		*/
		NewNode->CreateNewGuid();
		NewNode->NodePosX = RuntimeNode->Position.X;
		NewNode->NodePosY = RuntimeNode->Position.Y;

		// Set Node Info
		if (RuntimeNode->NodeInfo != nullptr)
		{
			// DuplicateObject means when the parent goes away the child also go
			NewNode->SetNodeInfo(DuplicateObject(RuntimeNode->NodeInfo, RuntimeNode));
		}
		else
		{
			NewNode->InitNodeInfo(NewNode);
		}

		if (RuntimeNode->InputPin != nullptr)
		{
			UNarrativaAIRuntimePin* Pin = RuntimeNode->InputPin;
			UEdGraphPin* UIPin = NewNode->CreateNarrativaPin(EGPD_Input, Pin->PinName);
			UIPin->PinId = Pin->PinId;

			//if (!Pin->Connections.IsEmpty())
			for (auto& CurrentConnection : Pin->Connections)
			{
				Connections.Add(std::make_pair(Pin->PinId, CurrentConnection->PinId));
			}
			IDToPinMap.Add(Pin->PinId, UIPin);
		}

		for (UNarrativaAIRuntimePin* OutputPin : RuntimeNode->OutputPins)
		{
			UEdGraphPin* UIPin = NewNode->CreateNarrativaPin(EGPD_Output, OutputPin->PinName);
			UIPin->PinId = OutputPin->PinId;

			//if (OutputPin->Connection != nullptr)
			for (auto& CurrentConnection : OutputPin->Connections)
			{
				Connections.Add(std::make_pair(OutputPin->PinId, CurrentConnection->PinId));
			}
			IDToPinMap.Add(OutputPin->PinId, UIPin);
		}

		CurrentWorkingOnGraph->AddNode(NewNode, true, true);
	}

	for (std::pair<FGuid, FGuid> Connection : Connections)
	{
		UEdGraphPin* FromPin = IDToPinMap[Connection.first];
		UEdGraphPin* ToPin = IDToPinMap[Connection.second];

		FromPin->LinkedTo.Add(ToPin);
		ToPin->LinkedTo.Add(FromPin);
	}
}

void FNarrativaAIAssetEditorApp::OnWorkingOnAssetPreSave()
{
	UpdateWorkingOnAssetFromGraph();
}

void FNarrativaAIAssetEditorApp::OnGraphSelectionChanged(const FGraphPanelSelectionSet& Selection)
{
	//SelectedNodes = Selection;
	UNarrativaAINodeBase* SelectedNode = GetSelectedNode(Selection);
	if (SelectedNode)
	{
		SelectedNodeDetailView->SetObject(SelectedNode->GetNodeInfo());
	}
	else
	{
		SelectedNodeDetailView->SetObject(nullptr);
	}
	SelectedNodes = Selection;
}

void FNarrativaAIAssetEditorApp::SetupCommands()
{
	if (GraphEditorCommands.IsValid())
	{
		return;
	}
	
	GraphEditorCommands = MakeShareable(new FUICommandList);
	
	GraphEditorCommands->MapAction(
		FGenericCommands::Get().Delete,
		FExecuteAction::CreateRaw(this, &FNarrativaAIAssetEditorApp::DeleteSelectedNodes),
		FCanExecuteAction::CreateRaw(this, &FNarrativaAIAssetEditorApp::CanDeleteNodes)
		);
}

void FNarrativaAIAssetEditorApp::DeleteSelectedNodes()
{
	if(!CurrentWorkingOnGraph) return;

	// No node selected
	if (SelectedNodes.Num() <= 0) return;

	const FScopedTransaction Transaction(FGenericCommands::Get().Delete->GetDescription());

	CurrentWorkingOnGraph->Modify();

	for (UObject* SelectedObject : SelectedNodes)
	{
		if (UEdGraphNode* Node = Cast<UEdGraphNode>(SelectedObject))
		{
			if(Node->CanUserDeleteNode())
			{
				Node->Modify();
				Node->DestroyNode();
			}				
		}
	}

	GetCurrentWorkingOnGraph()->NotifyGraphChanged();
}

bool FNarrativaAIAssetEditorApp::CanDeleteNodes()
{
	for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter)
	{
		UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
		if (Node && Node->CanUserDeleteNode())
		{
			return true;
		}
	}

	return false;
}

void FNarrativaAIAssetEditorApp::SetSelectedNodeDetailView(TSharedPtr<class IDetailsView> DetailsView)
{
	SelectedNodeDetailView = DetailsView;
	SelectedNodeDetailView->OnFinishedChangingProperties().AddRaw(this, &FNarrativaAIAssetEditorApp::OnNodeDetailViewPropertiesUpdated);
}

UNarrativaAINodeBase* FNarrativaAIAssetEditorApp::GetSelectedNode(const FGraphPanelSelectionSet& Selection)
{
	for (UObject* Object : Selection)
	{
		UNarrativaAINodeBase* Node = Cast<UNarrativaAINodeBase>(Object);
		if (Node)
		{
			return Node;
		}
	}

	return nullptr;
}

void FNarrativaAIAssetEditorApp::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& Event)
{
	if (CurrentWorkingOnGraphUI)
	{
		// Get the node being modified
		UNarrativaAINodeBase* GraphNode = GetSelectedNode(CurrentWorkingOnGraphUI->GetSelectedNodes());
		if (GraphNode)
		{
			GraphNode->OnPropertiesChanged();
		}
		CurrentWorkingOnGraphUI->NotifyGraphChanged();
	}
}
