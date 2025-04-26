#include "NarrativaAIEditor.h"
#include "NarrativaAIAssetAction.h"
#include "ToolMenus.h"
#include "IAssetTools.h"
#include "NarrativaAINodeFactory.h"
#include "NarrativaAISettings.h"
#include "EdGraph/EdGraphPin.h"
#include "Interfaces/IPluginManager.h"
#include "Interfaces/IProjectManager.h"
#include "SBlueprintEditorToolbar.h"
#include "ISettingsModule.h"
#include "Styling/SlateStyleRegistry.h"
#include "NarrativaAIEditor/Public/NarrativaAISettingsPage.h"

#define LOCTEXT_NAMESPACE "FNarrativaAIEditorModule"

void FNarrativaAIEditorModule::StartupModule()
{
	// RegisterSettings();
	
	// Register the Narrativa AI asset
	IAssetTools& AssetToolsModule = IAssetTools::Get();
	EAssetTypeCategories::Type AssetType = AssetToolsModule.RegisterAdvancedAssetCategory(FName(TEXT("NarrativaAI")), FText::FromString("Narrativa AI"));
	TSharedPtr<FNarrativaAIAssetAction> NarrativaAIAssetAction = MakeShareable(new FNarrativaAIAssetAction(AssetType));
	AssetToolsModule.RegisterAssetTypeActions(NarrativaAIAssetAction.ToSharedRef());

	// Custom images like thumbnails and icons
	StyleSet = MakeShareable(new FSlateStyleSet(NarrativaAIEditorStyle));
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin("NarrativaAI");
	FString ContentDir = Plugin->GetContentDir();
	StyleSet->SetContentRoot(ContentDir);

	FSlateImageBrush* ThumbnailBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("NarrativaAI_AssetIcon"), TEXT(".png")), FVector2D(128.0f, 128.0f));
	FSlateImageBrush* IconBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("NarrativaAI_AssetIcon"), TEXT(".png")), FVector2D(128.0f, 128.0f));
	FSlateImageBrush* NodeBorderBrush = new FSlateImageBrush(StyleSet->RootToContentDir(TEXT("NodeBorder"), TEXT(".png")), FVector2D(300.0f, 150.0f));

	StyleSet->Set(TEXT("ClassThumbnail.NarrativaAIAsset"), ThumbnailBrush);
	StyleSet->Set(TEXT("ClassIcon.NarrativaAIAsset"), IconBrush);
	StyleSet->Set(TEXT("NodeBorder.NarrativaAIAsset"), NodeBorderBrush);
	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet);

	// Register Node visual
	TSharedPtr<FNarrativaAINodeFactory> NodeFactory = MakeShared<FNarrativaAINodeFactory>();
	FEdGraphUtilities::RegisterVisualNodeFactory(NodeFactory);
	
	// Register a function to be called when menu system is initialized
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FNarrativaAIEditorModule::RegisterMenuExtensions));

}

void FNarrativaAIEditorModule::ShutdownModule()
{
	// if (UObjectInitialized())
	// {
	// 	UnregisterSettings();
	// }
	
	// Unregister the startup function
	UToolMenus::UnRegisterStartupCallback(this);
 
	// Unregister all our menu extensions
	UToolMenus::UnregisterOwner(this);

	// Unregister the style
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet);
	
}

void FNarrativaAIEditorModule::RegisterMenuExtensions()
{
	// // Use the current object as the owner of the menus
	// // This allows us to remove all our custom menus when the 
	// // module is unloaded (see ShutdownModule below)
	// FToolMenuOwnerScoped OwnerScoped(this);
	//
	// UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	// FToolMenuSection& ToolbarSection = ToolbarMenu->FindOrAddSection("NarrativaAI");
	//
	// ToolbarSection.AddEntry(FToolMenuEntry::InitToolBarButton(TEXT("NarrativaAISettings"), FExecuteAction::CreateLambda([this]()
	// {
	// 	HandleOpenSettings();
	// }),
	// INVTEXT("NarrativaAI Settings"),
	// INVTEXT("Settings for NarrativaAI")));
}

void FNarrativaAIEditorModule::HandleOpenSettings()
{
	UNarrativaAISettings* NarrativaAISettings = NewObject<UNarrativaAISettings>();

	TSharedRef<SWindow> Window = SNew(SWindow)
	.Title(FText::FromString("NarrativaAI Settings"))
	.ClientSize(FVector2D(400, 300))
	.SupportsMinimize(false)
	.SupportsMaximize(false)
	[
		SNew(SNarrativaAISettingsPage).SettingsObject(NarrativaAISettings)
	];

	FSlateApplication::Get().AddWindow(Window);
}

// void FNarrativaAIEditorModule::RegisterSettings()
// {
// 	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
// 	{
// 		// Register the runtime settings
// 		SettingsModule->RegisterSettings("Project", "Plugins", "NarrativaAI Settings",
// 			LOCTEXT("NarrativaAISettingsName", "NarrativaAI"),
// 			LOCTEXT("NarrativaAISettingsDescription", "Settings for NarrativaAI Plugin"),
// 			GetMutableDefault<UNarrativaAISettings>()
// 		);
// 	}
// }
//
// void FNarrativaAIEditorModule::UnregisterSettings()
// {
// 	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
// 	{
// 		SettingsModule->UnregisterSettings("Project", "Plugins", "NarrativaAI Settings");
// 	}
// }

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNarrativaAIEditorModule, NarrativaAIEditor)