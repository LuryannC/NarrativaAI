#include "NarrativaAIComponent.h"

#include "NarrativaAIAINodeInfo.h"
#include "NarrativaAILLMHandler.h"
#include "NarrativaAIAsset.h"
#include "NarrativaAIHumanNodeInfo.h"
#include "NarrativaAIPrecondition.h"
#include "NarrativaAIRuntimeGraph.h"
#include "NarrativaAISettings.h"
#include "NarrativaAIStartNodeInfo.h"

DEFINE_LOG_CATEGORY_STATIC(NarrativaAIComponentLog, Log, All);

UNarrativaAIComponent::UNarrativaAIComponent()
{
	LLMHandler = UNarrativaAILLMHandler::Get();
}

void UNarrativaAIComponent::RunNarrativaAI(UNarrativaAIAsset* NarrativaAIAsset)
{
	LLMHandler->ClearConversationHistory();
	RunNarrativaAI_Internal(NarrativaAIAsset);
}

void UNarrativaAIComponent::RunNarrativaAI_Internal(UNarrativaAIAsset* NarrativaAIAsset)
{
	if (!NarrativaAIAsset)
	{
		return;
	}

	CurrentNarrativaAIAsset = NarrativaAIAsset;
	UNarrativaAIRuntimeGraph* Graph = NarrativaAIAsset->Graph;
	UNarrativaAIRuntimeNode* StartNode = FindStartNode(Graph->Nodes);
	
	// This should never happen, but it is there just in case.
	if (!StartNode)
	{
		UE_LOG(NarrativaAIComponentLog, Error, TEXT("Couldn't find a start node in the graph. Try creating a new asset."));
		return;
	}

	InitDialogue(StartNode);
}


void UNarrativaAIComponent::OnPlayerOptionSelected(FString Context, UObject* SelectedNode)
{
	if(UNarrativaAIRuntimeNode* NarrativaAIRuntimeNode = Cast<UNarrativaAIRuntimeNode>(SelectedNode))
	{
		if (!NarrativaAIRuntimeNode)
		{
			UE_LOG(NarrativaAIComponentLog, Error, TEXT("SelectedNode is null! Cannot process player selection."));
			return;
		}

		FString FinalString = FString::Printf(TEXT("NPC Response to: %s"), *Context);
		
		GenerateNPCReply(FinalString, NarrativaAIRuntimeNode);
	}
}

TArray<UNarrativaAINodeInfoBase*> UNarrativaAIComponent::GetAllConnectedInfos(UNarrativaAIRuntimeNode* InNode)
{	
	if (InNode->OutputPins.Num() == 0)
	{
		return {};
	}

	TArray<UNarrativaAINodeInfoBase*> OutArray;
	// I am using 0 here as the nodes should have only one connection that allows multiple connections.
	OutArray.Reserve(InNode->OutputPins[0]->Connections.Num());

	for (UNarrativaAIRuntimePin* CurrentPin : InNode->OutputPins[0]->Connections)
	{
		OutArray.Add(CurrentPin->Parent->NodeInfo);
	}

	return OutArray;
}

TArray<UNarrativaAIRuntimeNode*> UNarrativaAIComponent::GetAllConnectedNodes(UNarrativaAIRuntimeNode* InNode)
{
	if (InNode->OutputPins.Num() == 0 || InNode->OutputPins[0]->Connections.Num() == 0)
	{
		return {};
	}

	TArray<UNarrativaAIRuntimeNode*> OutNodes;
	OutNodes.Reserve(InNode->OutputPins[0]->Connections.Num());

	for (UNarrativaAIRuntimePin* CurrentPin : InNode->OutputPins[0]->Connections)
	{
		OutNodes.Add(CurrentPin->Parent);
	}

	return OutNodes;
}

bool UNarrativaAIComponent::ProcessNodePreconditions(UNarrativaAIRuntimeNode* CurrentNode)
{
	if (UNarrativaAINodeInfoChild* NodeInfo = Cast<UNarrativaAINodeInfoChild>(CurrentNode->NodeInfo))
	{
		if (!NodeInfo->Preconditions.IsEmpty())
		{
			const FActorInfoStructure InActorInfoStructure{GetOwner(), this};
			for (const auto& Precondition : NodeInfo->Preconditions)
			{
				if (UNarrativaAIPrecondition* PreconditionInstance = NewObject<UNarrativaAIPrecondition>(this, Precondition))
				{
					if (!PreconditionInstance->CheckPrecondition(InActorInfoStructure))
					{
						UE_LOG(NarrativaAIComponentLog, Display, TEXT("Node %s did not meet precondition."), *CurrentNode->GetName());
						return false;
					}
				}
			}
		}
	}
	return true;
}

UNarrativaAIRuntimeNode* UNarrativaAIComponent::FindStartNode(TArray<UNarrativaAIRuntimeNode*> InNodes)
{
	for (UNarrativaAIRuntimeNode* CurrentNode : InNodes)
	{
		if (CurrentNode->NodeType == ENarrativaAINodeType::StartNode)
		{
			const UNarrativaAIStartNodeInfo* StartNodeInfo = Cast<UNarrativaAIStartNodeInfo>(CurrentNode->NodeInfo);
			LLMHandler->SetBackstory(StartNodeInfo->Backstory.ToString());
			return CurrentNode;
		}
	}
	return nullptr;
}

// 

void UNarrativaAIComponent::InitDialogue(UNarrativaAIRuntimeNode* StartNode)
{
	if (!StartNode) return;

	if (UNarrativaAIStartNodeInfo* StartNodeInfo = Cast<UNarrativaAIStartNodeInfo>(StartNode->NodeInfo))
	{
		InitializeBackstory(StartNodeInfo->Backstory.ToString(), StartNode);
	}
}

void UNarrativaAIComponent::InitializeBackstory(const FString& Backstory, UNarrativaAIRuntimeNode* Node)
{
	FString FinalString = CurrentNarrativaAIAsset->ReplyRules + Backstory;
	
	LLMHandler->OnAPIResponseReceived.RemoveAll(this);
	LLMHandler->OnAPIResponseReceived.AddDynamic(this, &UNarrativaAIComponent::ReceiveNPCReply);
	LLMHandler->SendLLMRequest(FinalString, Node);
	
	OnGenerationRequested.Broadcast();
}

void UNarrativaAIComponent::GenerateNPCReply(const FString& Context, UNarrativaAIRuntimeNode* Node)
{
	if (!Node) return;

	if(UNarrativaAIRuntimeNode* NarrativaAIRuntimeNode = Cast<UNarrativaAIRuntimeNode>(Node))
	{
		LLMHandler->OnAPIResponseReceived.RemoveAll(this);
		LLMHandler->OnAPIResponseReceived.AddDynamic(this, &UNarrativaAIComponent::ReceiveNPCReply);
		
		LLMHandler->SendLLMRequest(Context, NarrativaAIRuntimeNode);

		OnGenerationRequested.Broadcast();
	}
}

void UNarrativaAIComponent::IntroduceNPC(UNarrativaAIRuntimeNode* Node)
{
	
	LLMHandler->OnAPIResponseReceived.RemoveAll(this);
	LLMHandler->OnAPIResponseReceived.AddDynamic(this, &UNarrativaAIComponent::ReceiveNPCReply);
	
	FString Introduction = CurrentNarrativaAIAsset->IntroductionRules;
	LLMHandler->SendLLMRequest(Introduction, Node);
}

void UNarrativaAIComponent::ReceiveNPCReply(bool bSuccess, const FString& ResponseContent,
                                               UNarrativaAIRuntimeNode* OwningNode)
{
	if (!bSuccess) return;

	
	if (bIsStartingUp)
	{
		IntroduceNPC(OwningNode);
		
		bIsStartingUp = false;
		return;
	}
	
	OnNPCReplyReceived.Broadcast(ResponseContent, OwningNode);
	
	GeneratePlayerOptions(OwningNode);
}

void UNarrativaAIComponent::GeneratePlayerOptions(UNarrativaAIRuntimeNode* Node)
{
	auto ConnectedNodes = GetAllConnectedNodes(Node);
	DisplayableNodes.Empty();

	if (ConnectedNodes.IsEmpty())
	{
		OnEndOfDialogueReached.Broadcast();
		CurrentNarrativaAIAsset = nullptr;
		return;
	}

	if (ConnectedNodes.Num() > 0 && ConnectedNodes[0]->NodeType == ENarrativaAINodeType::NPCReply)
	{
		// We are inside a NPC Reply node, so we need to grab the connected nodes to it.
		ConnectedNodes = GetAllConnectedNodes(ConnectedNodes[0]);
	}

	NodesWaitingForResponse = ConnectedNodes.Num();
	
	for (UNarrativaAIRuntimeNode* CurrentNode : ConnectedNodes)
	{
		if (!ProcessNodePreconditions(CurrentNode))
		{
			NodesWaitingForResponse--;
			continue;
		}

		DisplayableNodes.Add(CurrentNode);
		
		if(CurrentNode->NodeType == ENarrativaAINodeType::AINode)
		{
			const UNarrativaAISettings* LLMSettings = GetDefault<UNarrativaAISettings>();

			const UNarrativaAIAINodeInfo* AINodeInfo = Cast<UNarrativaAIAINodeInfo>(CurrentNode->NodeInfo);
			
			FString FinalPromptString = AINodeInfo->Prompt.ToString() + "Using the following personality: " + LLMSettings->PlayerPersonalityPrompt;
			EnqueueRequest(FinalPromptString, CurrentNode);
		}
		else if (CurrentNode->NodeType == ENarrativaAINodeType::HumanNode)
		{
			UNarrativaAIHumanNodeInfo* HumanNodeInfo = Cast<UNarrativaAIHumanNodeInfo>(CurrentNode->NodeInfo);
			EnqueueRequest(HumanNodeInfo->Text.ToString(), CurrentNode);
		}		
	}
}

void UNarrativaAIComponent::ReceivePlayerOptions(bool bSuccess, const FString& ResponseContent,
	UNarrativaAIRuntimeNode* OwningNode)
{
	NodesWaitingForResponse--;

	if (!bSuccess) return;
	
	if (OwningNode)
	{
		if(UNarrativaAIAINodeInfo* AINodeInfo = Cast<UNarrativaAIAINodeInfo>(OwningNode->NodeInfo))
		{
			AINodeInfo->GeneratedText = FText::FromString(ResponseContent);
		}
	}

	bIsProcessingRequests = false;
	ProcessNextRequest();

	CheckForCompletion();
}

void UNarrativaAIComponent::CheckForCompletion() const
{
	if (NodesWaitingForResponse == 0)
	{
		OnPlayerOptionsGenerated.Broadcast(DisplayableNodes);
		OnGenerationFinished.Broadcast();
	}
}

void UNarrativaAIComponent::EnqueueRequest(const FString& Context, UNarrativaAIRuntimeNode* Node)
{
	RequestQueue.Enqueue(TTuple<FString, UNarrativaAIRuntimeNode*>(Context, Node));

	if (!bIsProcessingRequests)
	{
		ProcessNextRequest();
	}
}

void UNarrativaAIComponent::ProcessNextRequest()
{
	if (RequestQueue.IsEmpty() || bIsProcessingRequests)
	{
		return;
	}

	TTuple<FString, UNarrativaAIRuntimeNode*> Request;
	if (RequestQueue.Dequeue(Request))
	{
		const FString Context = Request.Get<0>();
		UNarrativaAIRuntimeNode* Node = Request.Get<1>();

		if(Node->NodeType == ENarrativaAINodeType::AINode)
		{
			bIsProcessingRequests = true;

			LLMHandler->OnAPIResponseReceived.RemoveAll(this);
			LLMHandler->OnAPIResponseReceived.AddDynamic(this, &UNarrativaAIComponent::ReceivePlayerOptions);		
		
			LLMHandler->SendLLMRequest(Context, Node);
		}
		else if (Node->NodeType == ENarrativaAINodeType::HumanNode)
		{
			bIsProcessingRequests = true;
			ReceivePlayerOptions(true, Context, Node);
		}	
	}
}
