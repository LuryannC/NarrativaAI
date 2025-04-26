#pragma once

#include "CoreMinimal.h"
#include "NarrativaAINodeInfoBase.h"
#include "NarrativaAINodeInfoChild.h"
#include "NarrativaAIAINodeInfo.generated.h"

UCLASS(BlueprintType)
class NARRATIVAAI_API UNarrativaAIAINodeInfo : public UNarrativaAINodeInfoChild
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category="Properties", meta = (MultiLine="true"))
	FText Prompt;

	UPROPERTY(BlueprintReadOnly, Category="Properties")
	FText GeneratedText = FText::GetEmpty();
};
