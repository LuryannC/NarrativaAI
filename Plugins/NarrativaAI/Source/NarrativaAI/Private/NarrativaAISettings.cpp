#include "NarrativaAISettings.h"

UNarrativaAISettings::UNarrativaAISettings(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Model = TEXT("gpt-3.5-turbo-0125");

	APIKey = TEXT("");

	RequestURL = TEXT("https://api.openai.com/v1/chat/completions");

	SettingPrompt = TEXT("");
	
	MaxTokens = 50;
	
	MaxConversationHistoryCharacterLength = 4000;
}
