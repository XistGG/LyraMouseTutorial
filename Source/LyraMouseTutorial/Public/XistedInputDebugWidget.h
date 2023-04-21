// Copyright 2023 xist.gg
// @see https://github.com/XistGG/LyraMouseTutorial

#pragma once

#include "CoreMinimal.h"
#include "UI/LyraActivatableWidget.h"
#include "XistedInputDebugWidget.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FXistedOnActiveInputModeChanged);


/**
 * XistEd Input Debug Widget
 *
 * I created a widget in my Lyra's `Content/UI/W_XistedInputDebug`
 * based on this class.  Then I hooked it up to the ShooterCore's `B_ShooterGame_Elimination`
 * experience, registered to the `HUD.Slot.InfrequentAbilities` widget extension point.
 *
 * This is *NOT REQUIRED*, it is purely a hack whose intent is to make it easier for you to
 * see what the current input mode is and when it is being changed.
 */
UCLASS()
class LYRAMOUSETUTORIAL_API UXistedInputDebugWidget : public ULyraActivatableWidget
{
	GENERATED_BODY()

public:
	// Set Class Defaults
	UXistedInputDebugWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	/**
	 * The actual values for mouse capture state and mouse lock state are NOT available to BP,
	 * and rather than hack the Engine and Lyra to make them available, I'm just exposing
	 * text descriptions of their values here.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Get UI Input Config Data")
	void GetUIInputConfigData(FText& MouseCaptureModeText, FText& MouseLockModeText, bool& bIsCursorVisible, bool& bIsLookInputIgnored, bool& bIsMoveInputIgnored);

	/** Implement this in your Widget BP and update your widget values every time this event fires */
	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Active Input Mode Changed")
	void K2_OnActiveInputModeChanged();

	/** Called by the CommonUIActionRouterBase subsystem only when this widget is active */
	UFUNCTION()
	void NativeOnActiveInputModeChanged(ECommonInputMode NewInputMode) { K2_OnActiveInputModeChanged(); }

};
