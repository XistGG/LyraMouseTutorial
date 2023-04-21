// Copyright 2023 xist.gg
// @see https://github.com/XistGG/LyraMouseTutorial

#include "XistedInputModeStatics.h"

#include "XistedLogMacros.h"
#include "XistedUIActionRouter.h"


void UXistedInputModeStatics::XistedSetInputMode(APlayerController* PlayerController, bool bMouseVisible, bool bIgnoreLookInput, bool bIgnoreMoveInput)
{
	// You must give us a valid PlayerController
	if (!IsValid(PlayerController))
	{
		XISTED_ERROR_LOG(TEXT("PlayerController [%s] is not valid"), *GetNameSafe(PlayerController));
		return;
	}

	// Make sure the controller is connected to a local player
	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		XISTED_ERROR_LOG(TEXT("LocalPlayer is null"));
		return;
	}

	// We only need the base action router, not our custom one, though none of this
	// will work the way we want if this hasn't been overridden to use our custom
	// action router subsystem.
	UCommonUIActionRouterBase* ActionRouter = LocalPlayer->GetSubsystem<UCommonUIActionRouterBase>();
	if (!ActionRouter)
	{
		XISTED_ERROR_LOG(TEXT("Common UI Action Router subsystem is not available"));
		return;
	}

	// Create the new desired UI Input Config
	FUIInputConfig NewInputConfig;
	if (bMouseVisible)
	{
		// Input settings when mouse is Visible
		constexpr bool bHideCursorDuringViewportCapture = false;
		NewInputConfig = FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::CaptureDuringMouseDown, bHideCursorDuringViewportCapture);
	}
	else
	{
		// Input settings when mouse is Invisible
		constexpr bool bHideCursorDuringViewportCapture = true;
		NewInputConfig = FUIInputConfig(ECommonInputMode::Game, EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown, bHideCursorDuringViewportCapture);
	}

	// Apply Look/Move ignore switches
	NewInputConfig.bIgnoreLookInput = bIgnoreLookInput;
	NewInputConfig.bIgnoreMoveInput = bIgnoreMoveInput;

	// Set mouse visibility just *before* we change the UI Input Config
	PlayerController->SetShowMouseCursor(bMouseVisible);

	// Tell the Action Router to use this new input config
	ActionRouter->SetActiveUIInputConfig(NewInputConfig);
}
