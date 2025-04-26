#include "NarrativaAIAINode.h"

#include "NarrativaAIAINodeInfo.h"

FText UNarrativaAIAINode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("AI-Authored Node");
}

FLinearColor UNarrativaAIAINode::GetNodeTitleColor() const
{
	return FLinearColor(FColor(0, 168, 107));
}

bool UNarrativaAIAINode::CanUserDeleteNode() const
{
	return true;
}

UEdGraphPin* UNarrativaAIAINode::CreateNarrativaPin(EEdGraphPinDirection InDirection, FName InName)
{
	FName Category = InDirection == EGPD_Input ? TEXT("Input") : TEXT("Output");
	FName SubCategory = InDirection == EGPD_Input ? TEXT("AINodeInPin") : TEXT("AINodeOutPin");

	UEdGraphPin* Pin = CreatePin(InDirection, Category, SubCategory, InName);
	Pin->PinType.PinSubCategory = SubCategory;
	
	return Pin;
}