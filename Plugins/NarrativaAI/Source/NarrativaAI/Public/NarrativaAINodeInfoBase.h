#pragma once

#include "CoreMinimal.h"
#include "NarrativaAINodeInfoBase.generated.h"

class UNarrativaAIPrecondition;

UCLASS(BlueprintType)
class NARRATIVAAI_API UNarrativaAINodeInfoBase : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY()
	TObjectPtr<UNarrativaAINodeInfoBase> ParentInfo;

	UPROPERTY()
	TArray<TObjectPtr<UNarrativaAINodeInfoBase>> Children;

	/* For now this will be used to check the active story line when there are multiple pins connected
	 * Basically when the player select a dialogue reply turn the connected node on.
	*/
	UPROPERTY()
	bool bIsActive = false;
};
