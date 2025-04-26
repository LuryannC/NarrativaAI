// Copyright Luryann A. Cervi. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Styling/SlateStyle.h"

class NARRATIVAAIEDITOR_API FNarrativaAIEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	void RegisterMenuExtensions();

	TSharedPtr<FSlateStyleSet> StyleSet = nullptr;
	TSharedPtr<struct FCustomPinFactory> PinFactory = nullptr;

	FName NarrativaAIEditorStyle = "NarrativaAIAssetEditorStyle";

	void HandleOpenSettings();
};
