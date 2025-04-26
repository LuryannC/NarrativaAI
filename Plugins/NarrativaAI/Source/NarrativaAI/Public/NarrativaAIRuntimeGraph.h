#pragma once

#include "CoreMinimal.h"
#include "NarrativaAINodeInfoBase.h"
#include "NarrativaAINodeType.h"
#include "UObject/NameTypes.h"
#include "NarrativaAIRuntimeGraph.generated.h"

UCLASS()
class NARRATIVAAI_API UNarrativaAIRuntimePin : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FName PinName;

	UPROPERTY()
	FGuid PinId;
		
	UPROPERTY()
	TArray<TObjectPtr<UNarrativaAIRuntimePin>> Connections;

	UPROPERTY()
	class UNarrativaAIRuntimeNode* Parent = nullptr;
};

UCLASS(BlueprintType, Blueprintable)
class NARRATIVAAI_API UNarrativaAIRuntimeNode : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	ENarrativaAINodeType NodeType = ENarrativaAINodeType::Unknown;

	UPROPERTY()
	UNarrativaAIRuntimePin* InputPin;

	UPROPERTY()
	TArray<UNarrativaAIRuntimePin*> OutputPins;

	UPROPERTY()
	FVector2D Position;

	/* TODO: RESEARCH THIS */
	// FRuntimeNodeData

	UPROPERTY(BlueprintReadOnly)
	UNarrativaAINodeInfoBase* NodeInfo = nullptr;
};


UCLASS()
class NARRATIVAAI_API UNarrativaAIRuntimeGraph : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<UNarrativaAIRuntimeNode*> Nodes;
};
