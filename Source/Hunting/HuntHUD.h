#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HuntHUD.generated.h"

UCLASS()
class AHuntHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};