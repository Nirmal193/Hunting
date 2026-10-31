#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HuntMobileControls.generated.h"

UCLASS()
class UHuntMobileControls : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnFireClicked();

	UFUNCTION()
	void OnCycleClicked();
};