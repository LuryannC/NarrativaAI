#pragma once

#include "NarrativaAIHumanNodeInfo.h"
#include "NarrativaAINodeBase.h"
#include "EdGraph/EdGraphNode.h"
#include "NarrativaAIHumanNode.generated.h"

UCLASS()
class NARRATIVAAIEDITOR_API UNarrativaAIHumanNode : public UNarrativaAINodeBase
{
	GENERATED_BODY()
	
public:
	
	public: // UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual bool CanUserDeleteNode() const override;
	
	public: // UNarrativaAINodeBase interface
	virtual UEdGraphPin* CreateNarrativaPin(EEdGraphPinDirection InDirection, FName InName) override;
	virtual ENarrativaAINodeType GetNarrativaAINodeType() const override { return ENarrativaAINodeType::HumanNode; }
	
	virtual void InitNodeInfo(UObject* Outer) override { NodeInfo = NewObject<UNarrativaAIHumanNodeInfo>(Outer); }
	virtual void SetNodeInfo(UNarrativaAINodeInfoBase* InNodeInfo) override { NodeInfo = Cast<UNarrativaAIHumanNodeInfo>(InNodeInfo); }
};
