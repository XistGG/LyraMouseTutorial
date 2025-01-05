// Copyright 2023-2025 Xist.GG LLC
// @see https://github.com/XistGG/LyraMouseTutorial

#pragma once

#include "CoreMinimal.h"
#include "XistedInputModeStatics.generated.h"


enum class ECommonInputMode : uint8;


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
	 * Set a new UI Input Mode
	 * @param PlayerController Must be the local player's controller, or an exception will be thrown
	 * @param CommonInputMode The new input mode to set
	 */
	UFUNCTION(BlueprintCallable)
	static void XistedSetInputMode(const APlayerController* PlayerController, const ECommonInputMode& CommonInputMode);

};
