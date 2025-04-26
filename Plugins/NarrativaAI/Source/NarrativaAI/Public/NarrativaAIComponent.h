#pragma once

#include "CoreMinimal.h"
#include "NarrativaAIAsset.h"
#include "NarrativaAIComponent.generated.h"

class UNarrativaAISettings;
class UNarrativaAILLMHandler;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInitResponseRequestSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndOfDialogueReachedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNPCReplyReceiveSignature, const FString, Text, UNarrativaAIRuntimeNode*, OwningNode);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPlayerOptionsGeneratedSignature, // The list of options as strings to display to the player
	const TArray<UNarrativaAIRuntimeNode*>&, ConnectedNodes // The list of nodes corresponding to each option
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGenerationRequestedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGenerationFinishedSignature);

UCLASS(ClassGroup=("NarrativaAI"), meta=(BlueprintSpawnableComponent))
class NARRATIVAAI_API UNarrativaAIComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UNarrativaAIComponent();

	UFUNCTION(BlueprintCallable)
	void RunNarrativaAI(UNarrativaAIAsset* NarrativaAIAsset);

	UFUNCTION(BlueprintCallable)
	void OnPlayerOptionSelected(FString Context, UObject* SelectedNode);
	
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnNPCReplyReceiveSignature OnNPCReplyReceived;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnPlayerOptionsGeneratedSignature OnPlayerOptionsGenerated;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnGenerationRequestedSignature OnGenerationRequested;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnGenerationFinishedSignature OnGenerationFinished;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnEndOfDialogueReachedSignature OnEndOfDialogueReached;
	
	UPROPERTY(BlueprintReadOnly)
	UNarrativaAILLMHandler* LLMHandler;

private:

	void RunNarrativaAI_Internal(UNarrativaAIAsset* NarrativaAIAsset);

	TArray<UNarrativaAINodeInfoBase*> GetAllConnectedInfos(UNarrativaAIRuntimeNode* InNode);

	TArray<UNarrativaAIRuntimeNode*> GetAllConnectedNodes(UNarrativaAIRuntimeNode* InNode);

	bool ProcessNodePreconditions(UNarrativaAIRuntimeNode* CurrentNode);

	UNarrativaAIRuntimeNode* FindStartNode(TArray<UNarrativaAIRuntimeNode*> InNodes);
	
	void InitDialogue(UNarrativaAIRuntimeNode* StartNode);

	void InitializeBackstory(const FString& Backstory, UNarrativaAIRuntimeNode* Node);

	void IntroduceNPC(UNarrativaAIRuntimeNode* Node);
	
	void GenerateNPCReply(const FString& Context, UNarrativaAIRuntimeNode* Node);

	UFUNCTION()
	void ReceiveNPCReply(bool bSuccess, const FString& ResponseContent, UNarrativaAIRuntimeNode* OwningNode);
		
	void GeneratePlayerOptions(UNarrativaAIRuntimeNode* Node);

	UFUNCTION()
	void ReceivePlayerOptions(bool bSuccess, const FString& ResponseContent, UNarrativaAIRuntimeNode* OwningNode);

	void CheckForCompletion() const;

	bool bIsStartingUp = true;

	int32 NodesWaitingForResponse = 0;

	UPROPERTY()
	TArray<UNarrativaAIRuntimeNode*> DisplayableNodes;
	
	// Queue System
	TQueue<TTuple<FString, UNarrativaAIRuntimeNode*>, EQueueMode::Spsc> RequestQueue;
	bool bIsProcessingRequests = false;

	void EnqueueRequest(const FString& Context, UNarrativaAIRuntimeNode* Node);
	void ProcessNextRequest();

	UPROPERTY()
	TObjectPtr<UNarrativaAIAsset> CurrentNarrativaAIAsset = nullptr;
};
