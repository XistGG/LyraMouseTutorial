// Copyright 2023-2025 Xist.GG LLC
// @see https://github.com/XistGG/LyraMouseTutorial

#include "XistedInputModeStatics.h"

#include "XistedLogMacros.h"
#include "XistedUIActionRouter.h"


void UXistedInputModeStatics::XistedSetInputMode(const APlayerController* PlayerController, const ECommonInputMode& CommonInputMode);
{
	check(PlayerController);

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogXimUI, Error, TEXT("Cannot SetInputMode for non-local player [%s]"), *PlayerController->GetName());
		return;
	}

	UCommonUIActionRouterBase* ActionRouter = LocalPlayer->GetSubsystem<UCommonUIActionRouterBase>();
	if (!ActionRouter)
	{
		UE_LOG(LogXimUI, Error, TEXT("CommonUIActionRouter is not available, cannot SetInputMode"));
		return;
	}

	FUIInputConfig InputConfig;
	if (CommonInputMode == ECommonInputMode::Game)
	{
		// Game mode means invisible mouse, permanently captured
		constexpr bool bHideCursorDuringViewportCapture = true;
		InputConfig = FUIInputConfig(CommonInputMode, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown, bHideCursorDuringViewportCapture);
	}
	else
	{
		// Menu or All modes mean visible mouse, not permanently captured
		constexpr bool bHideCursorDuringViewportCapture = false;
		InputConfig = FUIInputConfig(CommonInputMode, EMouseCaptureMode::CaptureDuringMouseDown, bHideCursorDuringViewportCapture);
	}

	ActionRouter->SetActiveUIInputConfig(InputConfig);
}
