// Copyright 2023 xist.gg
// @see https://github.com/XistGG/LyraMouseTutorial

#include "XistedUIActionRouter.h"

#include "XistedLogMacros.h"
#include "Widgets/SViewport.h"


void UXistedUIActionRouter::ApplyUIInputConfig(const FUIInputConfig& NewConfig, bool bForceRefresh)
{
	// DO NOT CALL SUPER !! Instead, this is a COMPLETE OVERRIDE of the base functionality
	//-Super::ApplyUIInputConfig(NewConfig, bForceRefresh);

	ULocalPlayer& LocalPlayer = *GetLocalPlayerChecked();

	// possibly nullptrs
	APlayerController* PC = LocalPlayer.GetPlayerController(GetWorld());
	UGameViewportClient* GameViewportClient = LocalPlayer.ViewportClient;

	// The input mode that was previously set
	const ECommonInputMode PreviousInputMode = GetActiveInputMode();

	// Here we're comparing the new intended settings for look/move input ignore
	// with the CURRENT PLAYER CONTROLLER settings, NOT WITH the ActiveInputConfig value,
	// which may not necessarily reflect reality.

	const bool bIsIgnoreLookChanged = PC && NewConfig.bIgnoreLookInput != PC->IsLookInputIgnored();
	const bool bIsIgnoreMoveChanged = PC && NewConfig.bIgnoreMoveInput != PC->IsMoveInputIgnored();

	// Note FUIInputConfig's operator== DOES NOT INCLUDE Move/Look input ignore,
	// but that is important to us so we explicitly check for it here

	const bool bIsInputChanged = !ActiveInputConfig.IsSet()
	                          || NewConfig != ActiveInputConfig.GetValue()
	                          || bIsIgnoreLookChanged
	                          || bIsIgnoreMoveChanged;

	if (!bForceRefresh && !bIsInputChanged)
	{
		// Nothing to do!  Early out.
		return;
	}

	if (!GameViewportClient || !PC)
	{
		// Missing required components!  Can't do anything.
		XISTED_WARNING_LOG(TEXT("Missing GameViewportClient (%s) or PC (%s)"),
			BOOL2TEXT(!GameViewportClient), BOOL2TEXT(!PC));
		return;
	}

	TSharedPtr<SViewport> ViewportWidget = GameViewportClient->GetGameViewportWidget();
	if (!ViewportWidget)
	{
		// Missing required component!  Can't do anything.
		XISTED_WARNING_LOG(TEXT("Failed to commit change! ViewportWidget is null."));
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	///  We have all of the data we need to set the new UI Input Config.
	///  Set the new config now.
	//////////////////////////////////////////////////////////////////////////

	XISTED_DISPLAY_LOG(TEXT("UIInputConfig being changed. bForceRefresh: %s"), BOOL2TEXT(bForceRefresh));
	XISTED_DISPLAY_LOG(TEXT("\tInputMode: Previous (%s), New (%s), IgnoreLook=%s IgnoreMove=%s"),
		ActiveInputConfig.IsSet() ? *StaticEnum<ECommonInputMode>()->GetValueAsString(ActiveInputConfig->GetInputMode()) : TEXT("None"),
		*StaticEnum<ECommonInputMode>()->GetValueAsString(NewConfig.GetInputMode()),
		BOOL2TEXT(NewConfig.bIgnoreLookInput), BOOL2TEXT(NewConfig.bIgnoreMoveInput));

	ActiveInputConfig = NewConfig;

	auto isCaptured = [] (const EMouseCaptureMode& Mode) { return Mode == EMouseCaptureMode::CapturePermanently || Mode == EMouseCaptureMode::CapturePermanently_IncludingInitialMouseDown; };

	const bool bWasPermanentlyCaptured = isCaptured(GameViewportClient->GetMouseCaptureMode());
	const bool bIsPermanentlyCaptured = isCaptured(NewConfig.GetMouseCaptureMode());

	const bool bHasPermanentCaptureModeChanged = bWasPermanentlyCaptured != bIsPermanentlyCaptured;

	bool bShowCursor {false};
	bool bLockMouse {false};
	EMouseLockMode MouseLockMode {EMouseLockMode::DoNotLock};

	if (PC->ShouldShowMouseCursor())
	{
		bShowCursor = true;
	}
	else
	{
		// Pim exists and Cursor is Not Visible, lock mouse to viewport
		bLockMouse = true;

		// Cursor is invisible, lock it in place
		MouseLockMode = EMouseLockMode::LockAlways;
	}

	GameViewportClient->SetMouseCaptureMode(NewConfig.GetMouseCaptureMode());
	GameViewportClient->SetHideCursorDuringCapture(NewConfig.HideCursorDuringViewportCapture() && !ShouldAlwaysShowCursor());
	GameViewportClient->SetMouseLockMode(MouseLockMode);

	// Perform SlateOperations on the Viewport Widget

	FReply& SlateOperations = LocalPlayer.GetSlateOperations();
	TSharedRef<SViewport> ViewportWidgetRef = ViewportWidget.ToSharedRef();

	SlateOperations.UseHighPrecisionMouseMovement(ViewportWidgetRef);
	SlateOperations.SetUserFocus(ViewportWidgetRef);

	// Lock or Unlock the mouse

	if (bLockMouse)
	{
		SlateOperations.LockMouseToWidget(ViewportWidgetRef);
	}
	else
	{
		SlateOperations.ReleaseMouseLock();
	}

	// Capture or Uncapture the mouse

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
		XISTED_LOG(TEXT("Set Ignore Look Input for UI Config"));
		PC->SetIgnoreLookInput(true);
	}
	else
	{
		PC->ResetIgnoreLookInput();
	}

	// Ignore Move Input?

	if (NewConfig.bIgnoreMoveInput)
	{
		XISTED_LOG(TEXT("Set Ignore Move Input for UI Config"));
		PC->SetIgnoreMoveInput(true);
	}
	else
	{
		PC->ResetIgnoreMoveInput();
	}

	// If the mouse cursor visibility state doesn't match the intended state, fix it:

	if (PC->ShouldShowMouseCursor() != bShowCursor)
	{
		PC->SetShowMouseCursor(bShowCursor);
	}

	// Finally, broadcast OnActiveInputModeChanged

	XISTED_LOG(TEXT("Broadcast Event: OnActiveInputModeChanged"));
	OnActiveInputModeChanged().Broadcast(NewConfig.GetInputMode());
}
