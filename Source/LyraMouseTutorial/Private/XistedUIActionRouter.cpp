// Copyright 2023-2025 Xist.GG LLC
// @see https://github.com/XistGG/LyraMouseTutorial

#include "XistedUIActionRouter.h"

#include "XistedLogMacros.h"
#include "Widgets/SViewport.h"


void UXistedUIActionRouter::ApplyUIInputConfig(const FUIInputConfig& InNewConfig, bool bForceRefresh)
{
	// DO NOT CALL SUPER !! Instead, this is a COMPLETE OVERRIDE of the base functionality
	//-Super::ApplyUIInputConfig(NewConfig, bForceRefresh);

	FUIInputConfig NewConfig = InNewConfig;

	ULocalPlayer& LocalPlayer = *GetLocalPlayerChecked();

	// possibly nullptrs
	APlayerController* PC = LocalPlayer.GetPlayerController(GetWorld());
	UGameViewportClient* GameViewportClient = LocalPlayer.ViewportClient;

	if (!GameViewportClient || !PC)
	{
		// Missing required components!  Can't do anything.
		UE_LOG(LogXimUIActionRouter, Warning, TEXT("Missing GameViewportClient (%s) or PC (%s)"),
			BOOL2TEXT(!GameViewportClient), BOOL2TEXT(!PC));
		return;
	}

	TSharedPtr<SViewport> ViewportWidget = GameViewportClient->GetGameViewportWidget();
	if (!ViewportWidget)
	{
		// Missing game viewport widget!  Can't do anything.
		UE_LOG(LogXimUIActionRouter, Error, TEXT("Failed to commit change! ViewportWidget is null."));
		return;
	}

	// Compute some things based on desired mouse cursor visibility
	const bool bShowCursor = NewConfig.GetInputMode() != ECommonInputMode::Game;
	if (bShowCursor)
	{
		// Force bIgnoreLookInput when showing the mouse cursor
		NewConfig.bIgnoreLookInput = true;
	}

	// Is the NewConfig the same as the current input mode?
	// This requires there to BE an active input config in the first place, which there may not be.
	const bool bIsSameAsActiveInputConfig = ActiveInputConfig.IsSet() && NewConfig == ActiveInputConfig.GetValue();

	if (!bForceRefresh && bIsSameAsActiveInputConfig)
	{
		// Nothing to do!  Early out.
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	///  We have all of the data we need to set the new UI Input Config.
	///  Set the new config now.
	//////////////////////////////////////////////////////////////////////////

	UE_LOG(LogXimUIActionRouter, Display, TEXT("UIInputConfig being changed. bForceRefresh: %s"), BOOL2TEXT(bForceRefresh));
	UE_LOG(LogXimUIActionRouter, Display, TEXT("\tInputMode: Previous (%s), New (%s)"),
		ActiveInputConfig.IsSet() ? LexToString(ActiveInputConfig->GetInputMode()) : TEXT("None"),
		LexToString(NewConfig.GetInputMode()));

#if WITH_XIM_INPUT_DEBUG
	TArray<FString> NewConfigDebugLines;
	constexpr bool bCullEmpty = true;
	NewConfig.ToString().ParseIntoArrayLines(OUT NewConfigDebugLines, bCullEmpty);

	for (const FString& DebugString : NewConfigDebugLines)
	{
		UE_LOG(LogXimUIActionRouter, Log, TEXT("\t\t%s"), *DebugString);
	}
#endif

	// Set ActiveInputConfig (this is the point of this entire method :)
	ActiveInputConfig = NewConfig;

	//////////////////////////////////////////////////////////////////////////
	///  Now change Unreal Engine to actually use this config
	//////////////////////////////////////////////////////////////////////////

	auto isCaptured = [] (const EMouseCaptureMode& Mode) { return Mode == EMouseCaptureMode::CapturePermanently || Mode == EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown; };

	const bool bWasPermanentlyCaptured = isCaptured(GameViewportClient->GetMouseCaptureMode());
	const bool bIsPermanentlyCaptured = isCaptured(NewConfig.GetMouseCaptureMode());

	// Lyra does stuff with this info, but we don't here.  You can use this if you like, or not.
	const bool bHasPermanentCaptureModeChanged = bWasPermanentlyCaptured != bIsPermanentlyCaptured;

	// Some settings differ based on whether the Player's mouse should be visible.

	const EMouseLockMode MouseLockMode = bShowCursor
		? EMouseLockMode::DoNotLock  // don't lock when cursor is visible
		: EMouseLockMode::LockAlways;  // do lock when cursor is invisible

	PC->SetShowMouseCursor(bShowCursor);

	GameViewportClient->SetMouseCaptureMode(NewConfig.GetMouseCaptureMode());
	GameViewportClient->SetHideCursorDuringCapture(NewConfig.HideCursorDuringViewportCapture() && !ShouldAlwaysShowCursor());
	GameViewportClient->SetMouseLockMode(MouseLockMode);

	// Perform SlateOperations on the Viewport Widget

	FReply& SlateOperations = LocalPlayer.GetSlateOperations();
	TSharedRef<SViewport> ViewportWidgetRef = ViewportWidget.ToSharedRef();

	SlateOperations.UseHighPrecisionMouseMovement(ViewportWidgetRef);
	SlateOperations.SetUserFocus(ViewportWidgetRef);

	// Lock or Unlock the mouse

	if (!bShowCursor)
	{
		// invisible cursor, lock mouse to viewport
		SlateOperations.LockMouseToWidget(ViewportWidgetRef);
	}
	else
	{
		// visible cursor, let the user put it where they want
		SlateOperations.ReleaseMouseLock();
	}

	// Capture or Release the mouse

	if (bIsPermanentlyCaptured)
	{
		SlateOperations.CaptureMouse(ViewportWidgetRef);
	}
	else
	{
		SlateOperations.ReleaseMouseCapture();
	}

	// Ignore Look Input?

	if (NewConfig.bIgnoreLookInput)
	{
		PC->SetIgnoreLookInput(true);
	}
	else
	{
		// Note: SetIgnoreLookInput(false) does NOT reliably remove look input ignore settings,
		// so instead we execute a complete reset
		PC->ResetIgnoreLookInput();
	}

	// Ignore Move Input?

	if (NewConfig.bIgnoreMoveInput)
	{
		PC->SetIgnoreMoveInput(true);
	}
	else
	{
		// Note: SetIgnoreMoveInput(false) does NOT reliably remove move input ignore settings,
		// so instead we execute a complete reset
		PC->ResetIgnoreMoveInput();
	}

	// Finally, broadcast OnActiveInputModeChanged

	UE_LOG(LogXimUIActionRouter, Verbose, TEXT("Broadcast Event: OnActiveInputModeChanged"));
	OnActiveInputModeChanged().Broadcast(NewConfig.GetInputMode());
}
