#include "NarrativaAIHumanNode.h"

FText UNarrativaAIHumanNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("Human-Authored Node");
}

FLinearColor UNarrativaAIHumanNode::GetNodeTitleColor() const
{
	return FLinearColor(FColor(0, 168, 107));
}

bool UNarrativaAIHumanNode::CanUserDeleteNode() const
{
	return true;
}

UEdGraphPin* UNarrativaAIHumanNode::CreateNarrativaPin(EEdGraphPinDirection InDirection, FName InName)
{
	FName Category = InDirection == EGPD_Input ? TEXT("Input") : TEXT("Output");
	FName SubCategory = InDirection == EGPD_Input ? TEXT("AINodeInPin") : TEXT("AINodeOutPin");

	UEdGraphPin* Pin = CreatePin(InDirection, Category, SubCategory, InName);
	Pin->PinType.PinSubCategory = SubCategory;
	
	return Pin;
}