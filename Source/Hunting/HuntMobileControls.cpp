#include "HuntMobileControls.h"

#include "HuntHunterCharacter.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

void UHuntMobileControls::NativeConstruct()
{
	Super::NativeConstruct();
	if (!WidgetTree)
	{
		return;
	}

	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("TouchControls"));
	WidgetTree->RootWidget = Root;

	const auto AddTouchButton = [this, Root](const FName Name, const FText Label, const FVector2D Position, const FVector2D Size, const FLinearColor Color)
	{
		UButton* Button = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), Name);
		Button->SetBackgroundColor(Color);
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass());
		Text->SetText(Label);
		Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
		Text->SetJustification(ETextJustify::Center);
		Button->AddChild(Text);
		UCanvasPanelSlot* Slot = Root->AddChildToCanvas(Button);
		Slot->SetAnchors(FAnchors(1.0f, 0.0f));
		Slot->SetAlignment(FVector2D(1.0f, 0.0f));
		Slot->SetPosition(Position);
		Slot->SetSize(Size);
		return Button;
	};

	AddTouchButton(TEXT("FireButton"), FText::FromString(TEXT("SPLAT")), FVector2D(-34.0f, 132.0f), FVector2D(150.0f, 92.0f), FLinearColor(0.83f, 0.27f, 0.16f, 0.88f))->OnClicked.AddDynamic(this, &UHuntMobileControls::OnFireClicked);
	AddTouchButton(TEXT("WeaponButton"), FText::FromString(TEXT("WEAPON")), FVector2D(-46.0f, 240.0f), FVector2D(126.0f, 56.0f), FLinearColor(0.16f, 0.48f, 0.42f, 0.92f))->OnClicked.AddDynamic(this, &UHuntMobileControls::OnCycleClicked);
}

void UHuntMobileControls::OnFireClicked()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (AHuntHunterCharacter* Hunter = Cast<AHuntHunterCharacter>(PlayerController->GetPawn()))
		{
			Hunter->FireWeapon();
		}
	}
}

void UHuntMobileControls::OnCycleClicked()
{
	if (APlayerController* PlayerController = GetOwningPlayer())
	{
		if (AHuntHunterCharacter* Hunter = Cast<AHuntHunterCharacter>(PlayerController->GetPawn()))
		{
			Hunter->CycleWeapon();
		}
	}
}