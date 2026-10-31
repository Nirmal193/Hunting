#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HuntPlayerController.generated.h"

class UHuntMobileControls;

UCLASS()
class AHuntPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TObjectPtr<UHuntMobileControls> MobileControls;
};