#include "NarrativaAISGraphNode.h"

#include "NarrativaAIAINode.h"
#include "NarrativaAIHumanNode.h"
#include "NarrativaAINodeBase.h"
#include "NarrativaAIReplyNode.h"
#include "NarrativaAIStartNode.h"
#include "SGraphPin.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"

// Start of NarrativaAIPin
void SNarrativaAIPin::Construct(const FArguments& InArgs, UEdGraphPin* InPin)
{
	this->SetCursor(EMouseCursor::Default);

	bShowLabel = true;

	GraphPinObj = InPin;
	check(GraphPinObj != NULL);

	const UEdGraphSchema* Schema = GraphPinObj->GetSchema();
	check(Schema);

	UNarrativaAINodeBase* NodeBase = CastChecked<UNarrativaAINodeBase>(InPin->GetOwningNode());
	if (NodeBase && NodeBase->GetNarrativaAINodeType() != ENarrativaAINodeType::NPCReply)
	{		
		SBorder::Construct(SBorder::FArguments()
			.BorderImage(this, &SNarrativaAIPin::GetPinBorder)
			.BorderBackgroundColor(this, &SNarrativaAIPin::GetPinColor)
			.OnMouseButtonDown(this, &SNarrativaAIPin::OnPinMouseDown)
			.Padding(FMargin(10.0f))
			);
	}
	else
	{
		SBorder::Construct(SBorder::FArguments()
			.BorderBackgroundColor(this, &SNarrativaAIPin::GetPinColor)
			.OnMouseButtonDown(this, &SNarrativaAIPin::OnPinMouseDown)
			.Padding(FMargin(8.0f))
			);
	}
}

FSlateColor SNarrativaAIPin::GetPinColor() const
{
	//return FSlateColor(IsHovered() ? FLinearColor(FColor(0, 168, 107)) : FLinearColor::Black);
	return FSlateColor(IsHovered() ? FLinearColor(0.1,0.1,0.1, 1) : FLinearColor(.03, .03, .03));
}

TSharedRef<SWidget> SNarrativaAIPin::GetDefaultValueWidget()
{
	return SNew(STextBlock);
}

const FSlateBrush* SNarrativaAIPin::GetPinBorder() const
{
	return FSlateStyleRegistry::FindSlateStyle("NarrativaAIAssetEditorStyle")->GetBrush("NodeBorder.NarrativaAIAsset");
	//return FAppStyle::GetBrush(TEXT("Graph.StateNode.Body"));
}
// End of NarrativaAIPin

void SNarrativaAISGraphNode::Construct(const FArguments& InArgs, UEdGraphNode* InNode)
{
	GraphNode = InNode;
	UpdateGraphNode();
}

void SNarrativaAISGraphNode::UpdateGraphNode()
{
	InputPins.Empty();
	OutputPins.Empty();

	const FMargin NodePadding = FMargin(1.0f);

	OutputPinBox.Reset();
	
	UNarrativaAINodeBase* NodeBase = CastChecked<UNarrativaAINodeBase>(GraphNode);

	this->ContentScale.Bind(this, &SGraphNode::GetContentScale);
	TSharedPtr<SVerticalBox> NodeBox = SNew(SVerticalBox);
	this->GetOrAddSlot(ENodeZone::Center)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SBox)
			.MinDesiredHeight(GetNodeMinSize().Y)
			//.MaxDesiredHeight(GetNodeMaxSize().Y)
			.MinDesiredWidth(GetNodeMinSize().X)
			.MaxDesiredWidth(GetNodeMaxSize().X)
			[
				SNew(SBorder)
				// .BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))
				.BorderImage(FSlateStyleRegistry::FindSlateStyle("NarrativaAIAssetEditorStyle")->GetBrush("NodeBorder.NarrativaAIAsset"))
				.BorderBackgroundColor(GetNodeOutlineColour())
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.Padding(1.0f)				
				[			
					SNew(SBorder)
					// .BorderImage(FAppStyle::GetBrush("Graph.StateNode.Body"))					
					.BorderImage(FSlateStyleRegistry::FindSlateStyle("NarrativaAIAssetEditorStyle")->GetBrush("NodeBorder.NarrativaAIAsset"))					
					.BorderBackgroundColor(FLinearColor(0.06,0.06,0.06, 1))
					.Padding(0.0f)
					[
						SNew(SOverlay)
						//Pins and node details
						+SOverlay::Slot()
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						[
							SNew(SVerticalBox)
							// Input pin area
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SBox)
								.MinDesiredHeight(NodePadding.Top)								
								[
									SAssignNew(LeftNodeBox, SVerticalBox)
								]
							]

							// Info area
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								GetNodeInfoArea()
							]
							
							// Content area
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								GetNodeBody()
							]
		
							// Output pin area
							+SVerticalBox::Slot()
							.AutoHeight()
							[
								SNew(SBox)
								.MinDesiredHeight(NodePadding.Bottom)
								[
									SAssignNew(RightNodeBox, SVerticalBox)
									+SVerticalBox::Slot()
									.HAlign(HAlign_Fill)
									.VAlign(VAlign_Fill)
									.Padding(20.0f, 0.0f)
									.FillHeight(1.0f)
									[
										SAssignNew(OutputPinBox, SHorizontalBox)
									]
								]
							]
						]
					]
				]
			]
		];
	
	AddNodePin();
}

void SNarrativaAISGraphNode::AddNodePin()
{
	UNarrativaAINodeBase* NodeBase = CastChecked<UNarrativaAINodeBase>(GraphNode);

	for(int32 PinIdx = 0; PinIdx < NodeBase->Pins.Num(); PinIdx++)
	{
		UEdGraphPin* MyPin = NodeBase->Pins[PinIdx];
		if (!MyPin->bHidden)
		{
			TSharedPtr<SGraphPin> NewPin = SNew(SNarrativaAIPin, MyPin);
			AddPin(NewPin.ToSharedRef());
		}
	}
}

void SNarrativaAISGraphNode::AddPin(const TSharedRef<SGraphPin>& PinToAdd)
{
	PinToAdd->SetOwner(SharedThis(this));

	const UEdGraphPin* PinObj = PinToAdd->GetPinObj();
	const bool bAdvancedParameter = PinObj && PinObj->bAdvancedView;
	if (bAdvancedParameter)
	{
		PinToAdd->SetVisibility( TAttribute<EVisibility>(PinToAdd, &SGraphPin::IsPinVisibleAsAdvanced) );
	}

	if (PinToAdd->GetDirection() == EEdGraphPinDirection::EGPD_Input)
	{
		LeftNodeBox->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			.FillHeight(1.0f)
			.Padding(20.0f,0.0f)
			[
				PinToAdd
			];
		InputPins.Add(PinToAdd);
	}
	else // Direction == EEdGraphPinDirection::EGPD_Output
	{
		OutputPinBox->AddSlot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		.FillWidth(1.0f)
		[
			PinToAdd
		];
		OutputPins.Add(PinToAdd);
	}
}

TSharedRef<SWidget> SNarrativaAISGraphNode::GetNodeInfoArea()
{
	UNarrativaAINodeBase* NarrativaAINode = Cast<UNarrativaAINodeBase>(GraphNode);
	const UNarrativaAINodeInfoChild* NodeInfo = Cast<UNarrativaAINodeInfoChild>(NarrativaAINode->GetNodeInfo());
	
	if (NodeInfo)
	{
		return SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(FMargin(5.0f))
			.HAlign(HAlign_Center)
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+SHorizontalBox::Slot()				
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString("Preconditions: "))
				]
				
				+SHorizontalBox::Slot()
				.AutoWidth()
				[
					SNew(STextBlock)
					.Text(FText::FromString(FString::Printf(TEXT("%i"), NodeInfo->Preconditions.Num())))
				]	
			];
	}
	return SNew(SVerticalBox);
}

TSharedRef<SWidget> SNarrativaAISGraphNode::GetNodeBody()
{
	UNarrativaAINodeBase* NarrativaAINode = Cast<UNarrativaAINodeBase>(GraphNode);
	switch (NarrativaAINode->GetNarrativaAINodeType())
	{
	case ENarrativaAINodeType::AINode:
		{
			const UNarrativaAIAINodeInfo* AINodeInfo = Cast<UNarrativaAIAINodeInfo>(NarrativaAINode->GetNodeInfo());
			FText Prompt = IsValid(AINodeInfo) ? AINodeInfo->Prompt : FText::FromString("None");
		
			return SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SOverlay)

					+ SOverlay::Slot()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(FMargin(5.0f))
					[					
						SNew(SBorder)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.BorderImage(FAppStyle::GetBrush(TEXT("Graph.StateNode.Body")))
						.BorderBackgroundColor(FLinearColor::Black)
						.Padding(5.0f, 5.0f)
						[
							SNew(STextBlock)
							.Text(HandleTextBreak(Prompt, 55))
							.ColorAndOpacity(FLinearColor::White)
							.AutoWrapText(true)
							.MinDesiredWidth(120.0f)
						]
					]
				];
		}

	case ENarrativaAINodeType::HumanNode:
		{
			const UNarrativaAIHumanNodeInfo* HumanNodeInfo = Cast<UNarrativaAIHumanNodeInfo>(NarrativaAINode->GetNodeInfo());
			FText Text = IsValid(HumanNodeInfo) ? HumanNodeInfo->Text : FText::FromString("None");

			return SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.VAlign(VAlign_Fill)
				[
					SNew(SOverlay)
		
					+ SOverlay::Slot()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(FMargin(5.0f))
					[					
						SNew(SBorder)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.BorderImage(FAppStyle::GetBrush(TEXT("Graph.StateNode.Body")))
						.BorderBackgroundColor(FLinearColor::Black)
						.Padding(10.0f, 10.0f)
						[
							SNew(STextBlock)
							.Text(HandleTextBreak(Text, 55))
							.ColorAndOpacity(FLinearColor::White)
							.AutoWrapText(true)
							.MinDesiredWidth(120.0f)
						]
					]
				];
		}
	
	case ENarrativaAINodeType::StartNode:
		{
			const UNarrativaAIStartNodeInfo* StartNodeInfo = Cast<UNarrativaAIStartNodeInfo>(NarrativaAINode->GetNodeInfo());
			FText DialogueTitle = FText::FromString("Start");
			FText Backstory = IsValid(StartNodeInfo) ? StartNodeInfo->Backstory : FText::FromString("None");
		
			return SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(DialogueTitle)
					.Justification(ETextJustify::Center)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SOverlay)

					+ SOverlay::Slot()
					.VAlign(VAlign_Fill)
					.HAlign(HAlign_Fill)
					.Padding(FMargin(5.0f))
					[					
						SNew(SBorder)
						.HAlign(HAlign_Fill)
						.VAlign(VAlign_Fill)
						.BorderImage(FAppStyle::GetBrush(TEXT("Graph.StateNode.Body")))
						.BorderBackgroundColor(FLinearColor::Black)
						.Padding(10.0f, 10.0f)
						[
							SNew(STextBlock)
							.Text(HandleTextBreak(Backstory, 150))
							.ColorAndOpacity(FLinearColor::White)
							.WrapTextAt(200)
						]					
					]			
				];
		}
	case ENarrativaAINodeType::NPCReply:
		{
			return SNew(SOverlay)
				+ SOverlay::Slot()
				[				
					SNew(STextBlock)
					.Text(FText::FromString(TEXT("NPC  Reply")))
					.ColorAndOpacity(FLinearColor::White)
					.Justification(ETextJustify::Center)
				];			
		}
	default:
		return SNew(SVerticalBox);
	}
}

FLinearColor SNarrativaAISGraphNode::GetNodeOutlineColour() const
{
	UNarrativaAINodeBase* NarrativaAINode = Cast<UNarrativaAINodeBase>(GraphNode);
	switch (NarrativaAINode->GetNarrativaAINodeType())
	{
	case ENarrativaAINodeType::AINode:
		return FLinearColor::Blue;
	case ENarrativaAINodeType::HumanNode:
		return FLinearColor::Green;
	case ENarrativaAINodeType::StartNode:
		return FLinearColor::Red;
	case ENarrativaAINodeType::NPCReply:
		return FLinearColor::Black;
	default:
		return FLinearColor::Black;
	}
}

FVector2f SNarrativaAISGraphNode::GetNodeMinSize() const
{
	UNarrativaAINodeBase* NarrativaAINode = Cast<UNarrativaAINodeBase>(GraphNode);
	switch (NarrativaAINode->GetNarrativaAINodeType())
	{
	case ENarrativaAINodeType::AINode:
		return FVector2f(120.0f, 80.0f);
	case ENarrativaAINodeType::HumanNode:
		return FVector2f(120.0f, 80.0f);
	case ENarrativaAINodeType::StartNode:
		return FVector2f(120.0f, 80.0f);
	case ENarrativaAINodeType::NPCReply:
		return FVector2f(100.0f, 50.0f);
	default:
		return FVector2f(0,0);
	}
}

FVector2f SNarrativaAISGraphNode::GetNodeMaxSize() const
{
	UNarrativaAINodeBase* NarrativaAINode = Cast<UNarrativaAINodeBase>(GraphNode);
	switch (NarrativaAINode->GetNarrativaAINodeType())
	{
	case ENarrativaAINodeType::AINode:
		return FVector2f(150.0f, 120.0f);
	case ENarrativaAINodeType::HumanNode:
		return FVector2f(150.0f, 120.0f);
	case ENarrativaAINodeType::StartNode:
		return FVector2f(300.0f, 150.0f);
	case ENarrativaAINodeType::NPCReply:
		return FVector2f(100.0f, 50.0f);
	default:
		return FVector2f(0,0);
	}
}

FText SNarrativaAISGraphNode::HandleTextBreak(const FText& Text, int32 Length)
{
	FString TrimmedTextString = Text.ToString();
	int32 TextLength = TrimmedTextString.Len();
	if (TextLength > Length)
	{
		TrimmedTextString = TrimmedTextString.Left(Length) + TEXT("...");
	}
	return FText::FromString(TrimmedTextString);
}
