#pragma once
#include "NarrativaAINodeBase.h"
#include "NarrativaAIReplyNode.generated.h"

UCLASS()
class NARRATIVAAIEDITOR_API UNarrativaAIReplyNode : public UNarrativaAINodeBase
{
	GENERATED_BODY()
public: // UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool CanUserDeleteNode() const override;
	
public: // UNarrativaAINodeBase interface
	virtual UEdGraphPin* CreateNarrativaPin(EEdGraphPinDirection InDirection, FName InName) override;
	virtual ENarrativaAINodeType GetNarrativaAINodeType() const override { return ENarrativaAINodeType::NPCReply; }
	
};
