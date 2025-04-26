#pragma once
#include "NarrativaAINodeInfoChild.h"
#include "NarrativaAIHumanNodeInfo.generated.h"

UCLASS(BlueprintType)
class NARRATIVAAI_API UNarrativaAIHumanNodeInfo : public UNarrativaAINodeInfoChild
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine="true"), Category="Properties")
	FText Text;
};
