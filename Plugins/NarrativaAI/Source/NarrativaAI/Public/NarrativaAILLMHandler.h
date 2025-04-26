#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "NarrativaAILLMHandler.generated.h"

class UNarrativaAISettings;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAPIResponseReceived, bool, bSuccess, const FString&, ResponseContent,
                                               class UNarrativaAIRuntimeNode*, OwningNode);

UCLASS(Blueprintable)
class NARRATIVAAI_API UNarrativaAILLMHandler : public UObject
{
	GENERATED_BODY()

public:

	static UNarrativaAILLMHandler* Get();
	
	UNarrativaAILLMHandler();
	~UNarrativaAILLMHandler();

	UFUNCTION(BlueprintCallable, Category="API")
	void SendLLMRequest(const FString& Content, UNarrativaAIRuntimeNode* OwningNode);

	void HandleLLMResponse(FHttpResponsePtr ResponsePtr, UNarrativaAIRuntimeNode* OwningNode, bool bWasSuccessful);
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAPIResponseReceived OnAPIResponseReceived;

	UFUNCTION(BlueprintCallable, Category="API")
	void SetBackstory(FString InBackstory) { Backstory = InBackstory; }

	void ClearConversationHistory() { ConversationHistory.Empty(); }

	void AddToConversationHistory(FString Role, FString Text);

private:

	UPROPERTY()
	const UNarrativaAISettings* LLMSettings = nullptr;

	FString Backstory;
	
	TArray<TSharedPtr<FJsonValue>> ConversationHistory;
public:
	// QUEUE	
	TQueue<TTuple<TSharedPtr<IHttpRequest>, UNarrativaAIRuntimeNode*>> RequestQueue;
};
