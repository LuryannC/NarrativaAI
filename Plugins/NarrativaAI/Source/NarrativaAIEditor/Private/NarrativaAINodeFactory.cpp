#include "NarrativaAINodeFactory.h"

#include "NarrativaAIAINode.h"
#include "NarrativaAINodeBase.h"
#include "NarrativaAIStartNode.h"

TSharedPtr<class SGraphNode> FNarrativaAINodeFactory::CreateNode(UEdGraphNode* Node) const
{
	//if (UNarrativaAIStartNode* NarrativaStartNode = Cast<UNarrativaAIStartNode>(Node))
	if (UNarrativaAINodeBase* NarrativaStartNode = Cast<UNarrativaAINodeBase>(Node))
	{
		return SNew(SNarrativaAISGraphNode, Node);
	}
	// else if (UNarrativaAIAINode* NarrativaAINode = Cast<UNarrativaAIAINode>(Node))
	// {
	// 	
	// }

	return nullptr;
}
