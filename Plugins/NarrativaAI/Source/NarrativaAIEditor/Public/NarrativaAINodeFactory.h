#pragma once

#include "GraphEditor.h"
#include "EdGraphUtilities.h"
#include "NarrativaAISGraphNode.h"

class NARRATIVAAIEDITOR_API FNarrativaAINodeFactory : public FGraphPanelNodeFactory
{
public: // FGraphPanelNodeFactory interface
	virtual TSharedPtr<SGraphNode> CreateNode(UEdGraphNode* Node) const override;	
};
