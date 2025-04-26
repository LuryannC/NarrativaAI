#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class NARRATIVAAIEDITOR_API FNarrativaAIAppMode : public FApplicationMode
{
public:
	FNarrativaAIAppMode(TSharedPtr<class FNarrativaAIAssetEditorApp> App);

	/* FApplicationMode interface */
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void PostActivateMode() override;
	virtual void PreDeactivateMode() override;
	/* End interface */

private:
	TWeakPtr<class FNarrativaAIAssetEditorApp> EditorApp;
	FWorkflowAllowedTabSet Tabs;
};
