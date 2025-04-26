#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "NarrativaAIGraphSchema.generated.h"


UCLASS()
class NARRATIVAAIEDITOR_API UNarrativaAIGraphSchema : public UEdGraphSchema 
{
	GENERATED_BODY()
	
public:
	/* Creates and display the options when right-clicking the graph */
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual void CreateDefaultNodesForGraph(UEdGraph& Graph) const override;
	virtual class FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;


	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotifcation) const override;
	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;

	/* Creating an in between node */
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
	void InsertIntermediateNode(UEdGraphPin* A, UEdGraphPin* B) const;
};

USTRUCT()
struct FNewNarrativaAINodeAction : public FEdGraphSchemaAction
{
	GENERATED_BODY()

public:
	FNewNarrativaAINodeAction(){}
	FNewNarrativaAINodeAction(UClass* ClassTemplate, FText NodeCategory, FText MenuDesc, FText ToolTip, const int32 Grouping)
	: FEdGraphSchemaAction(NodeCategory, MenuDesc, ToolTip, Grouping), ClassTemplate(ClassTemplate) {}

	virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode) override;

protected:
	UClass* ClassTemplate = nullptr;
};
