#pragma once

#include "NarrativaAINodeInfoBase.h"
#include "NarrativaAINodeType.h"
#include "EdGraph/EdGraphNode.h"
#include "NarrativaAINodeBase.generated.h"

UCLASS()
class NARRATIVAAIEDITOR_API UNarrativaAINodeBase : public UEdGraphNode
{
	GENERATED_BODY()
public: // Plugin interface

	virtual UEdGraphPin* CreateNarrativaPin(EEdGraphPinDirection InDirection, FName InName) { /* Must be overriden in child class */  return nullptr;}
	virtual UEdGraphPin* CreateDefaultInputPin() { return nullptr; }
	virtual void CreateDefaultOutputPin() { /* Nothing by default */ }

	//virtual void InitNodeInfo(UObject* Outer) { /* Nothing by default */ }
	//virtual void SetNodeInfo(UNarrativaAINodeInfoBase* InNodeInfo) { /* Nothing by default */ }
	//virtual UNarrativaAINodeInfoBase* GetNodeInfo() const { /* None by default */ return nullptr; }

	virtual ENarrativaAINodeType GetNarrativaAINodeType() const { return ENarrativaAINodeType::Unknown; }
	virtual void OnPropertiesChanged() { /* Nothing by default */ }

	virtual void InitNodeInfo(UObject* Outer) { NodeInfo = NewObject<UNarrativaAINodeInfoBase>(Outer); }
	virtual void SetNodeInfo(UNarrativaAINodeInfoBase* InNodeInfo) { NodeInfo = InNodeInfo; }
	virtual UNarrativaAINodeInfoBase* GetNodeInfo() const { return NodeInfo; }

	
protected:
	UPROPERTY()
	UNarrativaAINodeInfoBase* NodeInfo = nullptr;
};
