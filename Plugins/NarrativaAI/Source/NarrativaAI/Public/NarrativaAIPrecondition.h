#pragma once

#include "CoreMinimal.h"
#include "NarrativaAIPrecondition.generated.h"

USTRUCT(BlueprintType)
struct FActorInfoStructure
{
	GENERATED_BODY()

	FActorInfoStructure(){}
	FActorInfoStructure(AActor* InOwningActor, class UNarrativaAIComponent* InNarrativaAIComponent)
	{
		OwningActor = InOwningActor;
		NarrativaAIComponent = InNarrativaAIComponent;
	}
	
	UPROPERTY(BlueprintReadWrite)
	AActor* OwningActor = nullptr;

	UPROPERTY(BlueprintReadWrite)
	class UNarrativaAIComponent* NarrativaAIComponent = nullptr;
};

UCLASS(Blueprintable)
class UNarrativaAIPrecondition : public UObject
{

	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, DisplayName = "CheckPrecondition")
	bool CheckPrecondition(FActorInfoStructure InActorInfoStructure);

	virtual bool CheckPrecondition_Implementation(FActorInfoStructure InActorInfoStructure);

protected:
	UPROPERTY(BlueprintReadOnly)
	FActorInfoStructure ActorInfoStructure;
};
