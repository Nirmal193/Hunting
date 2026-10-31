#include "HuntHUD.h"

#include "HuntAnimal.h"
#include "HuntHunterCharacter.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/GameStateBase.h"

void AHuntHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas)
	{
		return;
	}

	const AHuntHunterCharacter* Hunter = Cast<AHuntHunterCharacter>(GetOwningPawn());
	if (!Hunter)
	{
		return;
	}

	UFont* Font = GEngine ? GEngine->GetSmallFont() : nullptr;
	if (!Font)
	{
		return;
	}

	const int32 SquadSize = GetWorld()->GetGameState() ? GetWorld()->GetGameState()->PlayerArray.Num() : 1;
	DrawText(FString::Printf(TEXT("SQUAD  %d / 4"), SquadSize), FLinearColor(0.68f, 1.0f, 0.42f), 36.0f, 30.0f, Font, 1.35f);
	DrawText(FString::Printf(TEXT("%s   |   INK %d%%"), *Hunter->GetWeaponName().ToString(), FMath::RoundToInt(Hunter->Ink)), FLinearColor::White, 36.0f, 58.0f, Font, 1.1f);

	for (TActorIterator<AHuntAnimal> It(GetWorld()); It; ++It)
	{
		const AHuntAnimal* Animal = *It;
		const FString Status = Animal->bIsHunted
			? TEXT("HUNT COMPLETE  -  NICE TEAMWORK")
			: FString::Printf(TEXT("TRACK THE ANIMAL   %d%%"), FMath::RoundToInt((1.0f - Animal->GetHealthPercent()) * 100.0f));
		DrawText(Status, Animal->bIsHunted ? FLinearColor(0.68f, 1.0f, 0.42f) : FLinearColor(1.0f, 0.78f, 0.3f), Canvas->SizeX * 0.5f - 145.0f, 34.0f, Font, 1.2f);
		break;
	}
}