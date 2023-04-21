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

	if (!GameViewportClient || !PC)
	{
		// Missing required components!  Can't do anything.
		XISTED_ERROR_LOG(TEXT("Missing GameViewportClient (%s) or PC (%s)"),
			BOOL2TEXT(!GameViewportClient), BOOL2TEXT(!PC));
		return;
	}

	TSharedPtr<SViewport> ViewportWidget = GameViewportClient->GetGameViewportWidget();
	if (!ViewportWidget)
	{
		// Missing game viewport widget!  Can't do anything.
		XISTED_ERROR_LOG(TEXT("Failed to commit change! ViewportWidget is null."));
		return;
	}

	// The input mode that was previously set.
	// We're not actively using this, since we expect that it may not be accurate,
	// but here it is if you want it.
	const ECommonInputMode PreviousInputMode = GetActiveInputMode();

	// Here we're comparing the new intended settings for look/move input ignore
	// with the CURRENT PLAYER CONTROLLER settings, NOT WITH the ActiveInputConfig value,
	// which may not necessarily reflect reality.

	const bool bIsIgnoreLookChanged = NewConfig.bIgnoreLookInput != PC->IsLookInputIgnored();
	const bool bIsIgnoreMoveChanged = NewConfig.bIgnoreMoveInput != PC->IsMoveInputIgnored();

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

	//////////////////////////////////////////////////////////////////////////
	///  We have all of the data we need to set the new UI Input Config.
	///  Set the new config now.
	//////////////////////////////////////////////////////////////////////////

	XISTED_DISPLAY_LOG(TEXT("UIInputConfig being changed. bForceRefresh: %s"), BOOL2TEXT(bForceRefresh));
	XISTED_DISPLAY_LOG(TEXT("\tInputMode: Previous (%s), New (%s), IgnoreLook=%s IgnoreMove=%s"),
		ActiveInputConfig.IsSet() ? *StaticEnum<ECommonInputMode>()->GetValueAsString(ActiveInputConfig->GetInputMode()) : TEXT("None"),
		*StaticEnum<ECommonInputMode>()->GetValueAsString(NewConfig.GetInputMode()),
		BOOL2TEXT(NewConfig.bIgnoreLookInput), BOOL2TEXT(NewConfig.bIgnoreMoveInput));

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

	// Some settings differ based on whether or not the Player's mouse should be visible.
	// To affect this, you need to call PC->SetShowMouseCursor *BEFORE* you call this->SetActiveUIInputConfig

	bool bLockMouse {false};
	EMouseLockMode MouseLockMode {EMouseLockMode::DoNotLock};

	const bool bShowCursor = PC->ShouldShowMouseCursor();
	if (!bShowCursor)
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

	XISTED_LOG(TEXT("Broadcast Event: OnActiveInputModeChanged"));
	OnActiveInputModeChanged().Broadcast(NewConfig.GetInputMode());
}
