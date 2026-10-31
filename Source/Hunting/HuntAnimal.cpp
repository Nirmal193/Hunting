#include "HuntAnimal.h"

#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "NavigationSystem.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AHuntAnimal::AHuntAnimal()
{
	bReplicates = true;
	SetReplicateMovement(true);
	PrimaryActorTick.bCanEverTick = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();
	GetCharacterMovement()->MaxWalkSpeed = 360.0f;
	GetCapsuleComponent()->InitCapsuleSize(55.0f, 65.0f);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetRootComponent());
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 15.0f));
	BodyMesh->SetRelativeScale3D(FVector(1.0f, 0.58f, 0.52f));
	if (SphereMesh.Succeeded())
	{
		BodyMesh->SetStaticMesh(SphereMesh.Object);
	}

	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(GetRootComponent());
	HeadMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HeadMesh->SetRelativeLocation(FVector(42.0f, 0.0f, 45.0f));
	HeadMesh->SetRelativeScale3D(FVector(0.42f, 0.34f, 0.34f));
	if (SphereMesh.Succeeded())
	{
		HeadMesh->SetStaticMesh(SphereMesh.Object);
	}
}

void AHuntAnimal::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(MovementTimer, this, &AHuntAnimal::ChooseMovementTarget, 1.8f, true, 0.5f);
	}
}

float AHuntAnimal::GetHealthPercent() const
{
	return MaxHealth > 0.0f ? FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f) : 0.0f;
}

float AHuntAnimal::TakeDamage(const float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!HasAuthority() || bIsHunted || DamageAmount <= 0.0f)
	{
		return 0.0f;
	}

	const float AppliedDamage = FMath::Min(CurrentHealth, DamageAmount);
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - AppliedDamage);
	if (CurrentHealth <= 0.0f)
	{
		FinishHunt();
	}
	return AppliedDamage;
}

void AHuntAnimal::ChooseMovementTarget()
{
	if (!HasAuthority() || bIsHunted)
	{
		return;
	}

	FVector FleeDirection = FVector::ZeroVector;
	float NearestHunterDistance = 1100.0f;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		const APlayerController* PlayerController = It->Get();
		const APawn* Hunter = PlayerController ? PlayerController->GetPawn() : nullptr;
		if (!Hunter)
		{
			continue;
		}

		const FVector AwayFromHunter = GetActorLocation() - Hunter->GetActorLocation();
		const float Distance = AwayFromHunter.Size();
		if (Distance < NearestHunterDistance)
		{
			NearestHunterDistance = Distance;
			FleeDirection = AwayFromHunter.GetSafeNormal2D();
		}
	}

	FVector DesiredLocation;
	if (NearestHunterDistance < 900.0f)
	{
		DesiredLocation = GetActorLocation() + FleeDirection * FMath::RandRange(500.0f, 900.0f);
	}
	else
	{
		DesiredLocation = GetActorLocation() + FVector(FMath::RandRange(-700.0f, 700.0f), FMath::RandRange(-700.0f, 700.0f), 0.0f);
	}

	FNavLocation NavigableLocation;
	UNavigationSystemV1* Navigation = UNavigationSystemV1::GetCurrent(GetWorld());
	AAIController* AIController = Cast<AAIController>(GetController());
	if (Navigation && AIController && Navigation->ProjectPointToNavigation(DesiredLocation, NavigableLocation, FVector(500.0f)))
	{
		AIController->MoveToLocation(NavigableLocation.Location, 75.0f);
	}
}

void AHuntAnimal::FinishHunt()
{
	bIsHunted = true;
	GetWorldTimerManager().ClearTimer(MovementTimer);
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->StopMovement();
	}
	GetCharacterMovement()->DisableMovement();
	SetLifeSpan(10.0f);
}

void AHuntAnimal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHuntAnimal, CurrentHealth);
	DOREPLIFETIME(AHuntAnimal, bIsHunted);
}