#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

class NARRATIVAAIEDITOR_API FNarrativaAIGraphTabFactory : public FWorkflowTabFactory
{
public:
	FNarrativaAIGraphTabFactory(TSharedPtr<class FNarrativaAIAssetEditorApp> App);

	/* FWorkflowTabFactory interface */
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
	/* End interface */

private:
	TWeakPtr<class FNarrativaAIAssetEditorApp> EditorApp;

	FName TabIdentifier = TEXT("NarrativeAIGraphTab");
};
