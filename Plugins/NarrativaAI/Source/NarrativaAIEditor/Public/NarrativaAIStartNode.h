#pragma once

#include "NarrativaAINodeBase.h"
#include "NarrativaAIStartNodeInfo.h"
#include "EdGraph/EdGraphNode.h"
#include "NarrativaAIStartNode.generated.h"


UCLASS()
class NARRATIVAAIEDITOR_API UNarrativaAIStartNode : public UNarrativaAINodeBase
{
	GENERATED_BODY()
public: // UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool CanUserDeleteNode() const override;

public: // UNarrativaAINodeBase interface
	virtual UEdGraphPin* CreateNarrativaPin(EEdGraphPinDirection InDirection, FName InName) override;
	virtual ENarrativaAINodeType GetNarrativaAINodeType() const override;

	virtual void InitNodeInfo(UObject* Outer) override { NodeInfo = NewObject<UNarrativaAIStartNodeInfo>(Outer); }
	virtual void SetNodeInfo(UNarrativaAINodeInfoBase* InNodeInfo) override { NodeInfo = Cast<UNarrativaAIStartNodeInfo>(InNodeInfo); }
};
