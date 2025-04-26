#pragma once

#include "Widgets/SCompoundWidget.h"

class IDetailsView;
class UNarrativaAISettings;

class SNarrativaAISettingsPage : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SNarrativaAISettingsPage) {}
	SLATE_ARGUMENT(UNarrativaAISettings*, SettingsObject)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	TSharedPtr<IDetailsView> DetailsView;
	
};
