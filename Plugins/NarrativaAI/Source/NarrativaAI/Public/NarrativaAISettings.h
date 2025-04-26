#pragma once

#include "CoreMinimal.h"
#include "NarrativaAISettings.generated.h"

USTRUCT()
struct FRequestMessageStructure
{
	GENERATED_BODY()
	
	FString Role = "";
	FString Content = "";
};

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "NarrativaAI Settings"))
class NARRATIVAAI_API UNarrativaAISettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	virtual FName GetCategoryName() const override { return TEXT("Game"); }
	
	UNarrativaAISettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//Begin Request settings
	UPROPERTY(EditAnywhere, Config, Category = "Settings")
	FString Model = TEXT("gpt-3.5-turbo-0125");

	UPROPERTY(EditAnywhere, Config,  Category = "Settings")
	FString APIKey = TEXT("");

	UPROPERTY(EditAnywhere, Config, Category = "Settings")
	FString RequestURL = TEXT("https://api.openai.com/v1/chat/completions");

	UPROPERTY(EditAnywhere, Config, Category = "Settings")
	int32 MaxTokens = 50;

	UPROPERTY(EditAnywhere, Config, Category = "Settings")
	int32 MaxConversationHistoryCharacterLength = 4000;

	/*
	 * Used for setting the game environment.
	 */
	UPROPERTY(EditAnywhere, Config, meta = (MultiLine="true"), Category = "Settings|Tuning")
	FString SettingPrompt = TEXT("");

	/*
	 * Used for setting the game environment.
	 */
	UPROPERTY(EditAnywhere, Config, meta = (MultiLine="true"), Category = "Settings|Tuning")
	FString PlayerPersonalityPrompt = TEXT("");

	UPROPERTY(EditAnywhere, Config, meta = (MultiLine="true"), Category = "Settings|Tuning")
	FString ConfigurationPrompt = TEXT("");

	//End Request settings
};
