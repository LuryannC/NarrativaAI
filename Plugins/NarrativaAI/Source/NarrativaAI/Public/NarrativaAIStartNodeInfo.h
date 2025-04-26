#pragma once

#include "CoreMinimal.h"
#include "NarrativaAINodeInfoBase.h"
#include "NarrativaAIStartNodeInfo.generated.h"

UCLASS(BlueprintType)
class NARRATIVAAI_API UNarrativaAIStartNodeInfo : public UNarrativaAINodeInfoBase
{
	GENERATED_BODY()
public:
	/*
	 * Dialogue backstory.
	 * Used to generate the initial flow.
	 */
	UPROPERTY(EditAnywhere, meta = (MultiLine="true"))
	FText Backstory;
};
