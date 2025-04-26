#pragma once

#include "NarrativaAIAINodeInfo.h"
#include "NarrativaAINodeBase.h"
#include "EdGraph/EdGraphNode.h"
#include "NarrativaAIAINode.generated.h"

class UNarrativaAIAINodeInfo;

UCLASS()
class NARRATIVAAIEDITOR_API UNarrativaAIAINode : public UNarrativaAINodeBase
{
	GENERATED_BODY()
public: // UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool CanUserDeleteNode() const override;
	
public: // UNarrativaAINodeBase interface
	virtual UEdGraphPin* CreateNarrativaPin(EEdGraphPinDirection InDirection, FName InName) override;
	virtual ENarrativaAINodeType GetNarrativaAINodeType() const override { return ENarrativaAINodeType::AINode; }
	
	virtual void InitNodeInfo(UObject* Outer) override { NodeInfo = NewObject<UNarrativaAIAINodeInfo>(Outer); }
	virtual void SetNodeInfo(UNarrativaAINodeInfoBase* InNodeInfo) override { NodeInfo = Cast<UNarrativaAIAINodeInfo>(InNodeInfo); }
};
