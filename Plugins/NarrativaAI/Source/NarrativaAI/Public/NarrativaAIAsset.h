// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NarrativaAIRuntimeGraph.h"
#include "NarrativaAIAsset.generated.h"

/**
 * 
 */
UCLASS()
class NARRATIVAAI_API UNarrativaAIAsset : public UObject
{
	GENERATED_BODY()
public:
	/* This asset properties */
	
	/* Data that is displayed in the top right panel */
	UPROPERTY(EditDefaultsOnly)
	FString DialogName = "";

	UPROPERTY(EditDefaultsOnly, meta = (MultiLine="true"))
	FText DialogueDescription = FText();

	/*
 	* Used as instruction to the LLM to generate text
 	*/
	UPROPERTY(EditDefaultsOnly, Category="Rules", meta = (MultiLine="true"))
	FString ReplyRules = TEXT("");
		

	/*
	 * Define how this NPC should introduce itself.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Rules", meta = (MultiLine="true"))
	FString IntroductionRules = "";

	UPROPERTY()
	UNarrativaAIRuntimeGraph* Graph;

public:
	/* Plugin's interface */

	/* Function to set what is called when clicking the "Save" button */
	void SetPreSaveListener(TFunction<void()> OnPreSaveListenerFunction) {  }

	/* End interface */

public:
	/* UObject Interface */
	
	/* Function that is actually triggered when clicking the "Save" button */
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;

	/* End interface */
	
private:
	TFunction<void()> OnPreSaveListener = nullptr;
};
