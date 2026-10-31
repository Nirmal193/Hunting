#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HuntAnimal.generated.h"

class UStaticMeshComponent;

UCLASS(Blueprintable)
class AHuntAnimal : public ACharacter
{
	GENERATED_BODY()

public:
	AHuntAnimal();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Hunt")
	float CurrentHealth = 300.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hunt", meta=(ClampMin="1.0"))
	float MaxHealth = 300.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category="Hunt")
	bool bIsHunted = false;

	UFUNCTION(BlueprintPure, Category="Hunt")
	float GetHealthPercent() const;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> BodyMesh;

	UPROPERTY(VisibleAnywhere, Category="Components")
	TObjectPtr<UStaticMeshComponent> HeadMesh;

	FTimerHandle MovementTimer;

	void ChooseMovementTarget();
	void FinishHunt();
};