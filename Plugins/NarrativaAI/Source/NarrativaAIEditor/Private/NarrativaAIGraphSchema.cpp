#include "NarrativaAIGraphSchema.h"

#include "NarrativaAIAINode.h"
#include "NarrativaAIConnectionDrawingPolicy.h"
#include "NarrativaAIHumanNode.h"
#include "NarrativaAIReplyNode.h"
#include "NarrativaAIStartNode.h"

void UNarrativaAIGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	TSharedPtr<FNewNarrativaAINodeAction> NewHumanAuthoredNode(new FNewNarrativaAINodeAction(
		UNarrativaAIHumanNode::StaticClass(),
		FText::FromString(TEXT("Nodes")),
		FText::FromString(TEXT("Human-Authored Node")),
		FText::FromString(TEXT("Add a new Human-Authored node to the graph")),
		0
		));

	TSharedPtr<FNewNarrativaAINodeAction> NewAIAuthoredNode(new FNewNarrativaAINodeAction(
		UNarrativaAIAINode::StaticClass(),
		FText::FromString(TEXT("Nodes")),
		FText::FromString(TEXT("AI-Authored Node")),
		FText::FromString(TEXT("Add a new AI-Authored node to the graph")),
		0
		));

	ContextMenuBuilder.AddAction(NewHumanAuthoredNode);
	ContextMenuBuilder.AddAction(NewAIAuthoredNode);
}

void UNarrativaAIGraphSchema::CreateDefaultNodesForGraph(UEdGraph& Graph) const
{
	// Creates the initial node for the graph.
	UNarrativaAIStartNode* StartNode = NewObject<UNarrativaAIStartNode>(&Graph);
	StartNode->CreateNewGuid();
	StartNode->NodePosX = 0;
	StartNode->NodePosY = 0;

	StartNode->CreateNarrativaPin(EGPD_Output, FName(TEXT("Start")));

	Graph.AddNode(StartNode, true, true);
	Graph.Modify();
}

const FPinConnectionResponse UNarrativaAIGraphSchema::CanCreateConnection(const UEdGraphPin* A,
	const UEdGraphPin* B) const
{
	if(!A || !B) return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid pin"));
	
	if (A->Direction == B->Direction)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Inputs can only connect to outputs"));
	}
	
	// Nodes
	UNarrativaAINodeBase* AINodeA = Cast<UNarrativaAINodeBase>(A->GetOwningNode());
	UNarrativaAINodeBase* AINodeB = Cast<UNarrativaAINodeBase>(B->GetOwningNode());
	
	// Nodes info
	UNarrativaAINodeInfoBase* AINodeInfoA = AINodeA->GetNodeInfo();
	UNarrativaAINodeInfoBase* AINodeInfoB = AINodeB->GetNodeInfo();
	
	if (!AINodeInfoA)
	{
		AINodeA->InitNodeInfo(AINodeA);
	}
	
	// Set the parent for the Node
	if (AINodeInfoB)
	{		
		AINodeInfoB->ParentInfo = AINodeA->GetNodeInfo();
		
		// Add children to parent node.
		AINodeA->GetNodeInfo()->Children.AddUnique(AINodeInfoB);
	}

	// Get node types
	ENarrativaAINodeType TypeA = AINodeA->GetNarrativaAINodeType();
	ENarrativaAINodeType TypeB = AINodeB->GetNarrativaAINodeType();

	if (TypeA == ENarrativaAINodeType::StartNode)
	{
		if (TypeB == ENarrativaAINodeType::AINode || TypeB == ENarrativaAINodeType::HumanNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
		}
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Start Node can only connect to AI-Authored Node or Human-Authored Node"));
	}

	if (TypeA == ENarrativaAINodeType::NPCReply)
	{
		if (TypeB == ENarrativaAINodeType::AINode || TypeB == ENarrativaAINodeType::HumanNode)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
		}
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("NPCReply can only connect to AINode or HumanNode"));
	}

	if (TypeA == ENarrativaAINodeType::AINode || TypeA == ENarrativaAINodeType::HumanNode)
	{
		if (TypeB == ENarrativaAINodeType::NPCReply)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
		}
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("AINode/HumanNode can only receive from StartNode or NPCReply and output to NPCReply | A new NPC Reply Node will be created"));
	}
	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW,TEXT(""));
}

UEdGraphNode* FNewNarrativaAINodeAction::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UNarrativaAINodeBase* NewNode = NewObject<UNarrativaAINodeBase>(ParentGraph, ClassTemplate);
	NewNode->CreateNewGuid();

	NewNode->NodePosX = Location.X;
	NewNode->NodePosY = Location.Y;

	NewNode->InitNodeInfo(NewNode);
	
	UEdGraphPin* InputPin = NewNode->CreateNarrativaPin(EGPD_Input, FName(TEXT("Input")));
	NewNode->CreateNarrativaPin(EGPD_Output, FName(TEXT("Output")));

	if (InputPin)
	{
		NewNode->GetSchema()->TryCreateConnection(FromPin, InputPin);
	}

	ParentGraph->Modify();
	ParentGraph->AddNode(NewNode, true);

	return NewNode;
}

class FConnectionDrawingPolicy* UNarrativaAIGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID,
	int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect,
	class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	return new FNarrativaAIConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor,  InClippingRect, InDrawElements, InGraphObj);
}

void UNarrativaAIGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	UNarrativaAINodeBase* AINodeTarget = Cast<UNarrativaAINodeBase>(&TargetNode);
	AINodeTarget->GetNodeInfo()->ParentInfo = nullptr;
	AINodeTarget->GetNodeInfo()->Children.Empty();
	
	Super::BreakNodeLinks(TargetNode);
}

void UNarrativaAIGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const
{
	UNarrativaAINodeBase* AINodeTarget = Cast<UNarrativaAINodeBase>(TargetPin.GetOwningNode());
	AINodeTarget->GetNodeInfo()->ParentInfo = nullptr;
	AINodeTarget->GetNodeInfo()->Children.Empty();
	
	Super::BreakPinLinks(TargetPin, bSendsNodeNotifcation);
}

void UNarrativaAIGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	UNarrativaAINodeBase* AISourcePin = Cast<UNarrativaAINodeBase>(SourcePin->GetOwningNode());
	UNarrativaAINodeBase* AITargetPin = Cast<UNarrativaAINodeBase>(TargetPin->GetOwningNode());
	AITargetPin->GetNodeInfo()->ParentInfo = nullptr;

	AISourcePin->GetNodeInfo()->Children.Remove(AITargetPin->GetNodeInfo());

	UEdGraphNode* MiddleNode = nullptr;
	for (UEdGraphPin* ConnectedPin : SourcePin->LinkedTo)
	{
		if (ConnectedPin->GetOwningNode() != SourcePin->GetOwningNode() &&
			ConnectedPin->GetOwningNode() != TargetPin->GetOwningNode())
		{
			MiddleNode = ConnectedPin->GetOwningNode();
			break;
		}
	}

	// If found and has only two connections, delete it
	if (MiddleNode && MiddleNode->Pins.Num() == 2)
	{
		MiddleNode->DestroyNode();
	}
	
	Super::BreakSinglePinLink(SourcePin, TargetPin);
}

bool UNarrativaAIGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	if (!A || !B || A->GetOwningNode() == B->GetOwningNode())
	{
		return false;
	}

	UEdGraphNode* NodeA = A->GetOwningNode();
	UEdGraphNode* NodeB = B->GetOwningNode();
	
	if (A->Direction == EGPD_Input && B->Direction == EGPD_Output)
	{
		// I know it is bad but it is quick.
		std::swap(A,B);
	}	

	UNarrativaAINodeBase* AINodeA = Cast<UNarrativaAINodeBase>(NodeA);
	UNarrativaAINodeBase* AINodeB = Cast<UNarrativaAINodeBase>(NodeB);

	if (!AINodeA || !AINodeB)
	{
		return false;
	}

	ENarrativaAINodeType TypeA = AINodeA->GetNarrativaAINodeType();
	ENarrativaAINodeType TypeB = AINodeB->GetNarrativaAINodeType();

	// **StartNode Logic**
	if (TypeA == ENarrativaAINodeType::StartNode)
	{
		if (TypeB == ENarrativaAINodeType::AINode || TypeB == ENarrativaAINodeType::HumanNode)
		{
			return UEdGraphSchema::TryCreateConnection(A, B);
		}
		return false;
	}

	// **NPCReply Logic**
	if (TypeA == ENarrativaAINodeType::NPCReply)
	{
		if (TypeB == ENarrativaAINodeType::AINode || TypeB == ENarrativaAINodeType::HumanNode)
		{
			// if (A->LinkedTo.Num() > 0) // NPCReply can only have one outgoing connection
			// {
			// 	BreakPinLinks(*A, false);
			// 	A->MakeLinkTo(B);
			// 	return UEdGraphSchema::TryCreateConnection(A, B);;
			// }
			return UEdGraphSchema::TryCreateConnection(A, B);
		}
		return false;
	}

	// **AINode & HumanNode Logic**
	if (TypeA == ENarrativaAINodeType::AINode || TypeA == ENarrativaAINodeType::HumanNode)
	{

		
		if (TypeB == ENarrativaAINodeType::NPCReply)
		{
			if (A->LinkedTo.Num() > 1)
			{
				BreakPinLinks(*A, false);
				// A->MakeLinkTo(B);
				return UEdGraphSchema::TryCreateConnection(A, B);;
			}
			return UEdGraphSchema::TryCreateConnection(A, B);;
		}
		if (TypeB ==  ENarrativaAINodeType::AINode || TypeB == ENarrativaAINodeType::HumanNode)
		{
			if (A->LinkedTo.Num() > 0)
			{
				BreakPinLinks(*A, false);
			}
			InsertIntermediateNode(A, B);
			return UEdGraphSchema::TryCreateConnection(A, B);;
		}
	}
	return false;
}

void UNarrativaAIGraphSchema::InsertIntermediateNode(UEdGraphPin* A, UEdGraphPin* B) const
{
	UEdGraph* Graph = A->GetOwningNode()->GetGraph();
	if (!Graph)	return;
	
	UNarrativaAIReplyNode* NPCReplyNode = NewObject<UNarrativaAIReplyNode>(Graph);
	NPCReplyNode->CreateNewGuid();

	FVector2D PosA(A->GetOwningNode()->NodePosX, A->GetOwningNode()->NodePosY);
	FVector2D PosB(B->GetOwningNode()->NodePosX, B->GetOwningNode()->NodePosY);
	FVector2D MidPos = (PosA + PosB) * 0.5f;

	NPCReplyNode->NodePosX = MidPos.X;
	NPCReplyNode->NodePosY = MidPos.Y;

	A->BreakLinkTo(B);

	UEdGraphPin* InputPin = NPCReplyNode->CreateNarrativaPin(EGPD_Input, TEXT("Input"));
	UEdGraphPin* OutputPin = NPCReplyNode->CreateNarrativaPin(EGPD_Output, TEXT("Output"));

	A->MakeLinkTo(InputPin);

	OutputPin->MakeLinkTo(B);

	Graph->AddNode(NPCReplyNode, false, false);
	
	
	Graph->Modify();
}
