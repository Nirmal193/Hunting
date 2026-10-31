#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HuntHunterCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UENUM(BlueprintType)
enum class EHuntWeapon : uint8
{
	Sprayer UMETA(DisplayName="Sprayer"),
	Scatter UMETA(DisplayName="Scatter Blaster"),
	Charger UMETA(DisplayName="Ink Charger")
};

UCLASS(Blueprintable)
class AHuntHunterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AHuntHunterCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Hunt|Weapon")
	EHuntWeapon SelectedWeapon = EHuntWeapon::Sprayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Hunt|Weapon")
	float Ink = 100.0f;

	UFUNCTION(BlueprintCallable, Category="Hunt|Input")
	void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category="Hunt|Input")
	void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category="Hunt|Weapon")
	void FireWeapon();

	UFUNCTION(BlueprintCallable, Category="Hunt|Weapon")
	void CycleWeapon();

	UFUNCTION(BlueprintPure, Category="Hunt|Weapon")
	FText GetWeaponName() const;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	float LastShotTime = -100.0f;
	FTimerHandle InkRechargeTimer;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookYaw(float Value);
	void LookPitch(float Value);
	void RechargeInk();
	void FireWeaponAuthority();
	void CycleWeaponAuthority();

	UFUNCTION(Server, Reliable)
	void ServerFireWeapon();

	UFUNCTION(Server, Reliable)
	void ServerCycleWeapon();
};