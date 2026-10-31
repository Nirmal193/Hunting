#include "HuntPlayerController.h"

#include "HuntMobileControls.h"

void AHuntPlayerController::BeginPlay()
{
	Super::BeginPlay();
	bool bShowTouchControls = false;
#if PLATFORM_ANDROID || PLATFORM_IOS || WITH_EDITOR
	bShowTouchControls = true;
#endif
	if (IsLocalController() && bShowTouchControls)
	{
		MobileControls = CreateWidget<UHuntMobileControls>(this, UHuntMobileControls::StaticClass());
		if (MobileControls)
		{
			MobileControls->AddToViewport(10);
		}
	}
}