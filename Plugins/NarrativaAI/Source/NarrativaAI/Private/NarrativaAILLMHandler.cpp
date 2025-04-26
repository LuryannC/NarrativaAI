#include "NarrativaAILLMHandler.h"

#include "HttpModule.h"
#include "NarrativaAIAINodeInfo.h"
#include "NarrativaAIRuntimeGraph.h"
#include "NarrativaAISettings.h"
#include "Interfaces/IHttpResponse.h"

DEFINE_LOG_CATEGORY_STATIC(NarrativaAILLMHandlerLog, Log, All);

UNarrativaAILLMHandler* UNarrativaAILLMHandler::Get()
{
	static UNarrativaAILLMHandler* Instance = NewObject<UNarrativaAILLMHandler>();
	Instance->AddToRoot();
	return Instance;
}

UNarrativaAILLMHandler::UNarrativaAILLMHandler()
{
	LLMSettings = GetDefault<UNarrativaAISettings>();
}

UNarrativaAILLMHandler::~UNarrativaAILLMHandler()
{
	OnAPIResponseReceived.Clear();
}

void UNarrativaAILLMHandler::SendLLMRequest(const FString& Content, UNarrativaAIRuntimeNode* OwningNode)
{
	if (!LLMSettings)
	{
		UE_LOG(NarrativaAILLMHandlerLog, Error, TEXT("SendAPIRequest() - Failed to retrieve settings"))
		return;
	}
	
	UE_LOG(NarrativaAILLMHandlerLog, Display, TEXT("UNarrativaAIAPIHandler::SendAPIRequest - Sending request"))

	TSharedPtr<IHttpRequest> HTTPRequestPtr = FHttpModule::Get().CreateRequest();
	
	// Add the request and the OwningNode to the request queue
	RequestQueue.Enqueue(TTuple<TSharedPtr<IHttpRequest>, UNarrativaAIRuntimeNode*>(HTTPRequestPtr, OwningNode));

	// Bind the response handler with the corresponding OwningNode
	HTTPRequestPtr->OnProcessRequestComplete().BindLambda([this, OwningNode](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
	{
		HandleLLMResponse(Response, OwningNode, bWasSuccessful);
	});

	
	HTTPRequestPtr->SetURL(LLMSettings->RequestURL);
	HTTPRequestPtr->SetVerb(TEXT("POST"));

	HTTPRequestPtr->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *LLMSettings->APIKey));
	HTTPRequestPtr->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// If this is the first request for the dialogue, we will be sending the Environment Setting.
	if (ConversationHistory.Num() == 0)
	{
		if (!LLMSettings->SettingPrompt.IsEmpty())
		{
			TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject());
			SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
			SystemMessage->SetStringField(TEXT("content"), FString::Printf(TEXT("%s"), *LLMSettings->SettingPrompt));
			ConversationHistory.Add(MakeShareable(new FJsonValueObject(SystemMessage)));
		}
		if (!LLMSettings->ConfigurationPrompt.IsEmpty())
		{
			TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject());
			SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
			SystemMessage->SetStringField(TEXT("content"), FString::Printf(TEXT("%s"), *LLMSettings->ConfigurationPrompt));
			ConversationHistory.Add(MakeShareable(new FJsonValueObject(SystemMessage)));
		}
	}
	// else
	{
		//Add the user message to the conversation history
		TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject());
		UserMessage->SetStringField(TEXT("role"), TEXT("user"));
		UserMessage->SetStringField(TEXT("content"), Content);
		ConversationHistory.Add(MakeShareable(new FJsonValueObject(UserMessage)));
	}
	
	// **Trim the conversation history if it exceeds token limit**
	const int32 MaxTokens = LLMSettings->MaxConversationHistoryCharacterLength;

	int32 TotalTokens = 0;
	for (const auto& Message : ConversationHistory)
	{
		TotalTokens += Message->AsObject()->GetStringField(TEXT("content")).Len();
	}

	if (TotalTokens > MaxTokens)
	{
		// Remove the oldest messages to fit within the token limit
		while (TotalTokens > MaxTokens && ConversationHistory.Num() > 1)
		{
			ConversationHistory.RemoveAt(0);
			TotalTokens = 0;
			for (const auto& Message : ConversationHistory)
			{
				TotalTokens += Message->AsObject()->GetStringField(TEXT("content")).Len();
			}
		}
	}
	// ** End Trim

	// Construct the request body
	TSharedPtr<FJsonObject> RequestBody = MakeShareable(new FJsonObject());
	RequestBody->SetStringField(TEXT("model"), *LLMSettings->Model);

	// Add all the conversation messages to the request
	RequestBody->SetArrayField(TEXT("messages"), ConversationHistory);

	// Set additional parameters like max tokens, temperature, etc.
	RequestBody->SetNumberField(TEXT("max_tokens"), LLMSettings->MaxTokens);
	RequestBody->SetNumberField(TEXT("n"), 1);
	RequestBody->SetStringField(TEXT("stop"), TEXT("null"));
	RequestBody->SetNumberField(TEXT("temperature"), 1);

	// Serialize the request body to a string
	FString RequestBodyString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBodyString);
	FJsonSerializer::Serialize(RequestBody.ToSharedRef(), Writer);

	HTTPRequestPtr->SetContentAsString(RequestBodyString);
	HTTPRequestPtr->ProcessRequest();
}

void UNarrativaAILLMHandler::HandleLLMResponse(FHttpResponsePtr ResponsePtr, UNarrativaAIRuntimeNode* OwningNode, bool bWasSuccessful)
{
	if (bWasSuccessful && ResponsePtr.IsValid())
	{
		// Parse the response body
		FString ResponseBody = ResponsePtr->GetContentAsString();
		TSharedPtr<FJsonObject> JsonResponse;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

		if (FJsonSerializer::Deserialize(Reader, JsonResponse))
		{
			const TArray<TSharedPtr<FJsonValue>>* ChoicesArray;
			if (JsonResponse->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray && ChoicesArray->Num() > 0)
			{
				// Get the assistant's response
				FString AssistantReply = JsonResponse->GetArrayField(TEXT("choices"))[0]
											 ->AsObject()
											 ->GetObjectField(TEXT("message"))
											 ->GetStringField(TEXT("content"));
				// Log the assistant's reply
				UE_LOG(NarrativaAILLMHandlerLog, Display, TEXT("HandleLLMResponse() - Assistant: %s"), *AssistantReply);

				//Add the assistant's response to the conversation history
				TSharedPtr<FJsonObject> AssistantMessage = MakeShareable(new FJsonObject());
				AssistantMessage->SetStringField(TEXT("role"), TEXT("assistant"));
				AssistantMessage->SetStringField(TEXT("content"), AssistantReply);
				ConversationHistory.Add(MakeShareable(new FJsonValueObject(AssistantMessage)));
			
				OnAPIResponseReceived.Broadcast(true, AssistantReply, OwningNode);

				// Handle any other actions after the response, e.g., update the UI, trigger further actions
			
				TTuple<TSharedPtr<IHttpRequest>, UNarrativaAIRuntimeNode*> DequeuedRequest;
				if (RequestQueue.Dequeue(DequeuedRequest))
				{
					// Optionally, handle any cleanup or logging here
				}
			}
			else
			{
				// Try to get an error message if present
				if (JsonResponse->HasField(TEXT("error")))
				{
					const TSharedPtr<FJsonObject> ErrorObject = JsonResponse->GetObjectField(TEXT("error"));
					if (ErrorObject.IsValid() && ErrorObject->HasField(TEXT("message")))
					{
						FString ErrorMessage = ErrorObject->GetStringField(TEXT("message"));
						UE_LOG(LogTemp, Error, TEXT("HandleLLMResponse() - API Error: %s"), *ErrorMessage);
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("HandleLLMResponse() - API returned an error, but no message was found."));
				}
			}
		}
	}
	else
	{
		// Handle request failure
		UE_LOG(NarrativaAILLMHandlerLog, Error, TEXT("HandleLLMResponse() - API Request failed!"));
	}	
	
	// UE_LOG(NarrativaAILLMHandlerLog, Display, TEXT("UNarrativaAIAPIHandler::HandleAPIResponse - Handle Response"))
	//
	// if (!bWasSuccessful || !ResponsePtr.IsValid())
	// {
	// 	UE_LOG(NarrativaAILLMHandlerLog, Warning, TEXT("UNarrativaAILLMHandler::OnResponseReceived - Request failed!"));
	// 	return;
	// }
	//
	// // it might be generating the text and applying to the next node as it gets rid of the current node
	// if (OwningNode)
	// {
	// 	if (UNarrativaAIAINodeInfo* AINodeInfo = Cast<UNarrativaAIAINodeInfo>(OwningNode->NodeInfo))
	// 	{
	// 		UE_LOG(NarrativaAILLMHandlerLog, Log, TEXT("UNarrativaAILLMHandler::OnResponseReceived - CurrentNode prompt: %s"), *AINodeInfo->Prompt.ToString());
	// 	}
	// }	
	//
	// // Parse the JSON response
	// const FString ResponseString = ResponsePtr->GetContentAsString();
	// UE_LOG(NarrativaAILLMHandlerLog, Log, TEXT("Response: %s"), *ResponseString);
	//
	// TSharedPtr<FJsonObject> JsonResponse;
	// TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseString);
	//
	// if (FJsonSerializer::Deserialize(Reader, JsonResponse))
	// {
	// 	if (JsonResponse.IsValid())
	// 	{
	// 		const FString LLMResponse = JsonResponse->GetArrayField("choices")[0]->AsObject()->GetObjectField("message")->GetStringField("content");
	// 		UE_LOG(NarrativaAILLMHandlerLog, Log, TEXT("ChatGPT Response: %s"), *LLMResponse);
	// 		OnAPIResponseReceived.Broadcast(true, LLMResponse, OwningNode);
	// 	}
	// }
	//
}

void UNarrativaAILLMHandler::AddToConversationHistory(FString Role, FString Text)
{
	// Add the user message to the conversation history
	// TSharedPtr<FJsonObject> Message = MakeShareable(new FJsonObject());
	// Message->SetStringField(TEXT("role"), Role);
	// Message->SetStringField(TEXT("content"), Text);
	// ConversationHistory.Add(MakeShareable(new FJsonValueObject(Message)));
}
