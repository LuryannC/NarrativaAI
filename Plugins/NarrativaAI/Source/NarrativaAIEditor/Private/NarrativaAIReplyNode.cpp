#include "NarrativaAIReplyNode.h"

FText UNarrativaAIReplyNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return FText::FromString("NPC Reply");
}

FLinearColor UNarrativaAIReplyNode::GetNodeTitleColor() const
{
	return FLinearColor::White;
}

bool UNarrativaAIReplyNode::CanUserDeleteNode() const
{
	return true;
}

UEdGraphPin* UNarrativaAIReplyNode::CreateNarrativaPin(EEdGraphPinDirection InDirection, FName InName)
{
	FName Category = InDirection == EGPD_Input ? TEXT("Input") : TEXT("Output");
	FName SubCategory = InDirection == EGPD_Input ? TEXT("AINodeInPin") : TEXT("AINodeOutPin");

	UEdGraphPin* Pin = CreatePin(InDirection, Category, SubCategory, InName);
	Pin->PinType.PinSubCategory = SubCategory;
	
	return Pin;
}
