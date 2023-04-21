// Copyright 2023 xist.gg
// @see https://github.com/XistGG/LyraMouseTutorial

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FLyraMouseTutorialModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
