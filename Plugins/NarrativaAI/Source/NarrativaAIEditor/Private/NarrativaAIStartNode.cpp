#include "NarrativaAIStartNode.h"

FText UNarrativaAIStartNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Start");
}

FLinearColor UNarrativaAIStartNode::GetNodeTitleColor() const
{
	return FLinearColor(FColor(0, 168, 107));
}

bool UNarrativaAIStartNode::CanUserDeleteNode() const
{
	return false;
}

UEdGraphPin* UNarrativaAIStartNode::CreateNarrativaPin(EEdGraphPinDirection InDirection, FName InName)
{
	FName Category = TEXT("Output");
	FName SubCategory = TEXT("StartPin");

	UEdGraphPin* Pin = CreatePin(InDirection, Category, SubCategory, InName);
	Pin->PinType.PinSubCategory = SubCategory;
	
	return Pin;
}

ENarrativaAINodeType UNarrativaAIStartNode::GetNarrativaAINodeType() const
{
	return ENarrativaAINodeType::StartNode;
}
