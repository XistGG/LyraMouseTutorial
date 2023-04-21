// Copyright 2023 xist.gg
// @see https://github.com/XistGG/LyraMouseTutorial

#pragma once

#include "CoreMinimal.h"
#include "XistedInputModeStatics.generated.h"


/**
 * XistEd Input Mode Statics
 *
 * You don't have to use a blueprint function library to do this, I'm just doing it this way
 * for example purposes.  The key is in the implementation of the XistedSetInputMode method,
 * which you can implement anywhere in your code that you want.
 */
UCLASS()
class LYRAMOUSETUTORIAL_API UXistedInputModeStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Set a new Input Mode for this player
	 * @param PlayerController Player Controller whose input mode to set
	 * @param bMouseVisible Whether the mouse should be visible (true) or invisible (false)
	 * @param bIgnoreLookInput Whether look input should now be ignored
	 * @param bIgnoreMoveInput Whether move input should now be ignored
	 */
	UFUNCTION(BlueprintCallable)
	static void XistedSetInputMode(APlayerController* PlayerController, bool bMouseVisible, bool bIgnoreLookInput, bool bIgnoreMoveInput);

};
