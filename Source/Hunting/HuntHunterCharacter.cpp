#include "HuntHunterCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMesh.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	struct FWeaponTuning
	{
		float Damage;
		float Cooldown;
		float InkCost;
		float Range;
		float SpreadDegrees;
		int32 Pellets;
	};

	FWeaponTuning GetWeaponTuning(const EHuntWeapon Weapon)
	{
		switch (Weapon)
		{
		case EHuntWeapon::Scatter:
			return { 7.0f, 0.75f, 9.0f, 1500.0f, 7.0f, 7 };
		case EHuntWeapon::Charger:
			return { 58.0f, 1.25f, 18.0f, 4200.0f, 0.4f, 1 };
		default:
			return { 9.0f, 0.16f, 3.0f, 2600.0f, 1.1f, 1 };
		}
	}
}

AHuntHunterCharacter::AHuntHunterCharacter()
{
	bReplicates = true;
	GetCapsuleComponent()->InitCapsuleSize(35.0f, 90.0f);
	GetCharacterMovement()->MaxWalkSpeed = 520.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 360.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> HunterMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
	if (HunterMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(HunterMesh.Object);
		GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));
		GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}
}

void AHuntHunterCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(InkRechargeTimer, this, &AHuntHunterCharacter::RechargeInk, 0.2f, true);
	}
}

void AHuntHunterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AHuntHunterCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AHuntHunterCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookYaw"), this, &AHuntHunterCharacter::LookYaw);
	PlayerInputComponent->BindAxis(TEXT("LookPitch"), this, &AHuntHunterCharacter::LookPitch);
	PlayerInputComponent->BindAction(TEXT("HuntFire"), IE_Pressed, this, &AHuntHunterCharacter::FireWeapon);
	PlayerInputComponent->BindAction(TEXT("HuntCycleWeapon"), IE_Pressed, this, &AHuntHunterCharacter::CycleWeapon);
}

void AHuntHunterCharacter::MoveForward(const float Value)
{
	DoMove(0.0f, Value);
}

void AHuntHunterCharacter::MoveRight(const float Value)
{
	DoMove(Value, 0.0f);
}

void AHuntHunterCharacter::LookYaw(const float Value)
{
	DoLook(Value, 0.0f);
}

void AHuntHunterCharacter::LookPitch(const float Value)
{
	DoLook(0.0f, Value);
}

void AHuntHunterCharacter::DoMove(const float Right, const float Forward)
{
	if (Controller)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Forward);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Right);
	}
}

void AHuntHunterCharacter::DoLook(const float Yaw, const float Pitch)
{
	if (Controller)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AHuntHunterCharacter::FireWeapon()
{
	if (HasAuthority())
	{
		FireWeaponAuthority();
	}
	else
	{
		ServerFireWeapon();
	}
}

void AHuntHunterCharacter::ServerFireWeapon_Implementation()
{
	FireWeaponAuthority();
}

void AHuntHunterCharacter::FireWeaponAuthority()
{
	if (!Controller)
	{
		return;
	}

	const FWeaponTuning Tuning = GetWeaponTuning(SelectedWeapon);
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastShotTime < Tuning.Cooldown || Ink < Tuning.InkCost)
	{
		return;
	}

	LastShotTime = Now;
	Ink = FMath::Max(0.0f, Ink - Tuning.InkCost);
	const FVector TraceStart = FollowCamera->GetComponentLocation();
	const FVector AimDirection = Controller->GetControlRotation().Vector();
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(HuntWeaponTrace), true, this);
	QueryParams.AddIgnoredActor(this);

	for (int32 PelletIndex = 0; PelletIndex < Tuning.Pellets; ++PelletIndex)
	{
		const FVector ShotDirection = FMath::VRandCone(AimDirection, FMath::DegreesToRadians(Tuning.SpreadDegrees));
		const FVector TraceEnd = TraceStart + ShotDirection * Tuning.Range;
		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams) && Hit.GetActor())
		{
			UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Tuning.Damage, ShotDirection, Hit, Controller, this, UDamageType::StaticClass());
		}
	}
}

void AHuntHunterCharacter::CycleWeapon()
{
	if (HasAuthority())
	{
		CycleWeaponAuthority();
	}
	else
	{
		ServerCycleWeapon();
	}
}

void AHuntHunterCharacter::ServerCycleWeapon_Implementation()
{
	CycleWeaponAuthority();
}

void AHuntHunterCharacter::CycleWeaponAuthority()
{
	SelectedWeapon = static_cast<EHuntWeapon>((static_cast<uint8>(SelectedWeapon) + 1) % 3);
}

void AHuntHunterCharacter::RechargeInk()
{
	Ink = FMath::Min(100.0f, Ink + 4.0f);
}

FText AHuntHunterCharacter::GetWeaponName() const
{
	switch (SelectedWeapon)
	{
	case EHuntWeapon::Scatter:
		return FText::FromString(TEXT("SCATTER BLASTER"));
	case EHuntWeapon::Charger:
		return FText::FromString(TEXT("INK CHARGER"));
	default:
		return FText::FromString(TEXT("SPRAYER"));
	}
}

void AHuntHunterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHuntHunterCharacter, SelectedWeapon);
	DOREPLIFETIME(AHuntHunterCharacter, Ink);
}