#pragma once

#include "NarrativaAINodeInfoBase.h"
#include "NarrativaAINodeInfoChild.generated.h"

UCLASS()
class NARRATIVAAI_API UNarrativaAINodeInfoChild : public UNarrativaAINodeInfoBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category="Properties")
	TArray<TSubclassOf<UNarrativaAIPrecondition>> Preconditions;
};
