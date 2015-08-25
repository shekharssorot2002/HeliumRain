
#include "../../Flare.h"
#include "FlareMainMenu.h"
#include "../../Game/FlareGame.h"
#include "../../Game/FlareSaveGame.h"
#include "../../Player/FlareMenuPawn.h"
#include "../../Player/FlareMenuManager.h"
#include "../../Player/FlarePlayerController.h"


#define LOCTEXT_NAMESPACE "FlareMainMenu"


/*----------------------------------------------------
	Construct
----------------------------------------------------*/

void SFlareMainMenu::Construct(const FArguments& InArgs)
{
	// Data
	MenuManager = InArgs._MenuManager;
	const FFlareStyleCatalog& Theme = FFlareStyleSet::GetDefaultTheme();
	Game = MenuManager->GetPC()->GetGame();
	SaveSlotToDelete = -1;

	// Build structure
	ChildSlot
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SVerticalBox)

		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		.Padding(Theme.ContentPadding)
		[
			SNew(SHorizontalBox)

			// Icon
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SImage).Image(AFlareMenuManager::GetMenuIcon(EFlareMenu::MENU_Main))
			]

			// Title
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.Padding(Theme.ContentPadding)
			[
				SNew(STextBlock)
				.TextStyle(&Theme.TitleFont)
				.Text(LOCTEXT("MainMenu", "HELIUM RAIN"))
			]

			// Settings
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(Theme.TitleButtonPadding)
			.AutoWidth()
			[
				SNew(SFlareRoundButton)
				.Text(LOCTEXT("Settings", "Settings"))
				.HelpText(LOCTEXT("SettingsInfo", "Open the system settings"))
				.Icon(AFlareMenuManager::GetMenuIcon(EFlareMenu::MENU_Settings, true))
				.OnClicked(this, &SFlareMainMenu::OnOpenSettings)
			]

			// Quit
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(Theme.TitleButtonPadding)
			.AutoWidth()
			[
				SNew(SFlareRoundButton)
				.Text(LOCTEXT("Quit", "Quit game"))
				.HelpText(LOCTEXT("QuitInfo", "Quit the game and go back to the OS desktop"))
				.Icon(AFlareMenuManager::GetMenuIcon(EFlareMenu::MENU_Quit, true))
				.OnClicked(this, &SFlareMainMenu::OnQuitGame)
			]
		]

		// Separator
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(FMargin(200, 20))
		[
			SNew(SImage).Image(&Theme.SeparatorBrush)
		]

		// Info
		+ SVerticalBox::Slot()
		.AutoHeight()
		.Padding(Theme.ContentPadding)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Text(LOCTEXT("SaveSlotHint", "Pick a save slot to start the game."))
			.TextStyle(&Theme.SubTitleFont)
		]

		// Save slots
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SAssignNew(SaveBox, SHorizontalBox)
		]

		// Build info
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Bottom)
		.Padding(Theme.SmallContentPadding)
		[
			SNew(STextBlock)
			.Text(FText::FromString("Flare / Development build / " + FString(__DATE__)))
			.TextStyle(&Theme.SmallFont)
		]
	];

	// Add save slots
	for (int32 Index = 1; Index <= Game->GetSaveSlotCount(); Index++)
	{
		TSharedPtr<int32> IndexPtr(new int32(Index));

		SaveBox->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Top)
		[
			SNew(SVerticalBox)

			// Slot N°
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(Theme.TitlePadding)
			[
				SNew(STextBlock)
				.TextStyle(&Theme.TitleFont)
				.Text(FText::FromString(FString::FromInt(Index) + "/"))
			]

			// Company emblem
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(Theme.ContentPadding)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(HAlign_Center)
				[
					SNew(SImage)
					.Image(this, &SFlareMainMenu::GetSaveIcon, Index)
				]
			]

			// Description
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.TextStyle(&Theme.TextFont)
				.Text(this, &SFlareMainMenu::GetText, Index)
			]

			// Launch
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.AutoHeight()
			.Padding(Theme.SmallContentPadding)
			[
				SNew(SFlareButton)
				.Text(this, &SFlareMainMenu::GetButtonText, Index)
				.HelpText(LOCTEXT("StartGameInfo", "Start playing"))
				.Icon(this, &SFlareMainMenu::GetButtonIcon, Index)
				.OnClicked(this, &SFlareMainMenu::OnOpenSlot, IndexPtr)
				.Width(5)
				.Height(1)
			]

			// Delete
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Left)
				.VAlign(VAlign_Center)
				.AutoHeight()
				.Padding(Theme.SmallContentPadding)
				[
					SNew(SFlareButton)
					.Text(LOCTEXT("Delete", "Delete game"))
				.HelpText(LOCTEXT("DeleteInfo", "Delete this game, forever, without backup !"))
				.Icon(FFlareStyleSet::GetIcon("Delete"))
				.OnClicked(this, &SFlareMainMenu::OnDeleteSlot, IndexPtr)
				.Width(5)
				.Height(1)
				.Visibility(this, &SFlareMainMenu::GetDeleteButtonVisibility, Index)
			]
		];
	}
}


/*----------------------------------------------------
	Interaction
----------------------------------------------------*/

void SFlareMainMenu::Setup()
{
	SetEnabled(false);
	SetVisibility(EVisibility::Hidden);
}

void SFlareMainMenu::Enter()
{
	FLOG("SFlareMainMenu::Enter");
	Game->ReadAllSaveSlots();
	SetEnabled(true);
	SetVisibility(EVisibility::Visible);
}

void SFlareMainMenu::Exit()
{
	SetEnabled(false);
	SetVisibility(EVisibility::Hidden);
}


/*----------------------------------------------------
	Callbacks
----------------------------------------------------*/

FText SFlareMainMenu::GetText(int32 Index) const
{
	if (Game->DoesSaveSlotExist(Index))
	{
		const FFlareSaveSlotInfo& SaveSlotInfo = Game->GetSaveSlotInfo(Index);

		// Build info strings
		FString CompanyString = SaveSlotInfo.CompanyName.ToString();
		FString ShipString = FString::FromInt(SaveSlotInfo.CompanyShipCount) + " ";
		ShipString += (SaveSlotInfo.CompanyShipCount == 1 ? LOCTEXT("Ship", "ship").ToString() : LOCTEXT("Ships", "ships").ToString());
		FString MoneyString = FString::FromInt(SaveSlotInfo.CompanyMoney) + " " + LOCTEXT("Credits", "credits").ToString();

		return FText::FromString(CompanyString + "\n" + MoneyString + "\n" + ShipString + "\n");
	}
	else
	{
		return FText::FromString("\n\n\n");
	}
}

const FSlateBrush* SFlareMainMenu::GetSaveIcon(int32 Index) const
{
	return (Game->DoesSaveSlotExist(Index) ? &(Game->GetSaveSlotInfo(Index).EmblemBrush) : FFlareStyleSet::GetIcon("Help"));
}

EVisibility SFlareMainMenu::GetDeleteButtonVisibility(int32 Index) const
{
	return (Game->DoesSaveSlotExist(Index) ? EVisibility::Visible : EVisibility::Collapsed);
}

FText SFlareMainMenu::GetButtonText(int32 Index) const
{
	return (Game->DoesSaveSlotExist(Index) ? LOCTEXT("Load", "Load game") : LOCTEXT("Create", "New game"));
}

const FSlateBrush* SFlareMainMenu::GetButtonIcon(int32 Index) const
{
	return (Game->DoesSaveSlotExist(Index) ? FFlareStyleSet::GetIcon("Load") : FFlareStyleSet::GetIcon("New"));
}

void SFlareMainMenu::OnOpenSlot(TSharedPtr<int32> Index)
{
	AFlarePlayerController* PC = MenuManager->GetPC();
	if (PC && Game)
	{
		Game->SetCurrentSlot(*Index);

		// Load the world
		if (Game->DoesSaveSlotExist(*Index))
		{
			Game->LoadGame(PC);
			// TODO if active sector, fly ship
			MenuManager->OpenMenu(EFlareMenu::MENU_Orbit, PC->GetShipPawn());
		}

		// Create the world
		else
		{
			MenuManager->OpenMenu(EFlareMenu::MENU_NewGame);
		}
	}
}

void SFlareMainMenu::OnDeleteSlot(TSharedPtr<int32> Index)
{
	SaveSlotToDelete = *Index;
	MenuManager->Confirm(LOCTEXT("ConfirmExit", "Do you really want to delete this save slot ?"), FSimpleDelegate::CreateSP(this, &SFlareMainMenu::OnDeleteSlotConfirmed));
}

void SFlareMainMenu::OnDeleteSlotConfirmed()
{
	if (SaveSlotToDelete >= 0)
	{
		Game->DeleteSaveSlot(SaveSlotToDelete);
		Game->ReadAllSaveSlots();
		SaveSlotToDelete = -1;
	}
}

void SFlareMainMenu::OnOpenSettings()
{
	MenuManager->Notify(LOCTEXT("Unavailable", "Unavailable feature !"), LOCTEXT("Info", "The full game will be released later :)"));
	//MenuManager->OpenMenu(EFlareMenu::MENU_Settings);
}

void SFlareMainMenu::OnQuitGame()
{
	MenuManager->OpenMenu(EFlareMenu::MENU_Quit);
}


#undef LOCTEXT_NAMESPACE

