// Fill out your copyright notice in the Description page of Project Settings.
#include "NarrativaAIAsset.h"
#include "UObject/ObjectSaveContext.h"

void UNarrativaAIAsset::PreSave(FObjectPreSaveContext SaveContext)
{	
	Super::PreSave(SaveContext);
	
	if (OnPreSaveListener)
	{
		OnPreSaveListener();
	}
	
}
