#pragma once

#include "SGraphNode.h"
#include "SGraphPin.h"

// Narrativa AI pin
class NARRATIVAAIEDITOR_API SNarrativaAIPin : public SGraphPin
{
public:
	SLATE_BEGIN_ARGS(SNarrativaAIPin){}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphPin* InPin);
protected:
	//~ Begin SGraphPin Interface
	virtual FSlateColor GetPinColor() const override;
	virtual TSharedRef<SWidget>	GetDefaultValueWidget() override;
	//~ End SGraphPin Interface

	const FSlateBrush* GetPinBorder() const;
};


class NARRATIVAAIEDITOR_API SNarrativaAISGraphNode : public SGraphNode
{
public:
	SLATE_BEGIN_ARGS(SNarrativaAISGraphNode) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, UEdGraphNode* InNode);
	virtual void UpdateGraphNode() override;

	virtual void AddPin(const TSharedRef<SGraphPin>& PinToAdd) override;

	FLinearColor GetNodeOutlineColour() const;

	//Begin helper functions based on node type
	TSharedRef<SWidget> GetNodeBody();
	TSharedRef<SWidget> GetNodeInfoArea();
	FVector2f GetNodeMinSize() const;
	FVector2f GetNodeMaxSize() const;
	static FText HandleTextBreak(const FText& Text, int32 Length);
	// End helper functions
protected:
	void AddNodePin();

	TSharedPtr<SHorizontalBox> OutputPinBox;

private:
	// Shared settings
	float DialogueNodesTextWrapValue = 120; 
};