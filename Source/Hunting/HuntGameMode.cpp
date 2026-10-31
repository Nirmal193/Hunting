#include "HuntGameMode.h"

#include "HuntAnimal.h"
#include "HuntHUD.h"
#include "HuntHunterCharacter.h"
#include "HuntPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "GameFramework/PlayerStart.h"

AHuntGameMode::AHuntGameMode()
{
	DefaultPawnClass = AHuntHunterCharacter::StaticClass();
	PlayerControllerClass = AHuntPlayerController::StaticClass();
	HUDClass = AHuntHUD::StaticClass();
	AnimalClass = AHuntAnimal::StaticClass();
}

void AHuntGameMode::BeginPlay()
{
	Super::BeginPlay();
	SpawnHuntAnimal();
}

void AHuntGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (ErrorMessage.IsEmpty() && GetNumPlayers() >= MaxHunters)
	{
		ErrorMessage = TEXT("This hunt is full. A squad can have up to four hunters.");
	}
}

AHuntAnimal* AHuntGameMode::GetHuntAnimal() const
{
	return HuntAnimal;
}

void AHuntGameMode::SpawnHuntAnimal()
{
	if (!HasAuthority() || !AnimalClass || HuntAnimal)
	{
		return;
	}

	FVector Origin = FVector::ZeroVector;
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
	if (PlayerStarts.Num() > 0)
	{
		Origin = PlayerStarts[FMath::RandRange(0, PlayerStarts.Num() - 1)]->GetActorLocation();
	}

	const float Angle = FMath::FRandRange(0.0f, 2.0f * PI);
	const float Distance = FMath::FRandRange(AnimalSpawnRadius * 0.45f, AnimalSpawnRadius);
	FVector SpawnLocation = Origin + FVector(FMath::Cos(Angle) * Distance, FMath::Sin(Angle) * Distance, 0.0f);
	FNavLocation NavigableLocation;
	if (UNavigationSystemV1* Navigation = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		if (Navigation->ProjectPointToNavigation(SpawnLocation, NavigableLocation, FVector(1000.0f)))
		{
			SpawnLocation = NavigableLocation.Location;
		}
	}
	SpawnLocation.Z += 80.0f;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	HuntAnimal = GetWorld()->SpawnActor<AHuntAnimal>(AnimalClass, SpawnLocation, FRotator::ZeroRotator, SpawnParameters);
}