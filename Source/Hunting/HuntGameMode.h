#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HuntGameMode.generated.h"

class AHuntAnimal;

UCLASS()
class AHuntGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHuntGameMode();

	UFUNCTION(BlueprintPure, Category="Hunt")
	AHuntAnimal* GetHuntAnimal() const;

protected:
	virtual void BeginPlay() override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Hunt", meta=(ClampMin="1", ClampMax="4"))
	int32 MaxHunters = 4;

	UPROPERTY(EditDefaultsOnly, Category="Hunt")
	TSubclassOf<AHuntAnimal> AnimalClass;

	UPROPERTY(EditDefaultsOnly, Category="Hunt", meta=(ClampMin="500.0"))
	float AnimalSpawnRadius = 1600.0f;

	UPROPERTY()
	TObjectPtr<AHuntAnimal> HuntAnimal;

	void SpawnHuntAnimal();
};