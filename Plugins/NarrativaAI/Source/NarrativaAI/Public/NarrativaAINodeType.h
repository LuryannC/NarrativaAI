#pragma once

UENUM()
enum class ENarrativaAINodeType
{
	Unknown,
	NPCReply,
	StartNode,
	AINode,
	HumanNode,
	EndNode // I might not need this if I explicit tell to end if there is not child node,
};