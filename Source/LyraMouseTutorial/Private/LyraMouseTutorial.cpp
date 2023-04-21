// Copyright 2023 xist.gg
// @see https://github.com/XistGG/LyraMouseTutorial

#include "LyraMouseTutorial.h"

#define LOCTEXT_NAMESPACE "FLyraMouseTutorialModule"

void FLyraMouseTutorialModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FLyraMouseTutorialModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLyraMouseTutorialModule, LyraMouseTutorial)