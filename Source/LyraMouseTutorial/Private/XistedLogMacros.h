// Copyright 2023 xist.gg
// @see https://github.com/XistGG/LyraMouseTutorial
//
// For more ideas about ways to format UE logs:
// @see https://x157.github.io/UE5/Dev/DebuggingTips#XistLogFormat
//

#pragma once

#include "CoreMinimal.h"
#include "LyraLogChannels.h"  // for LogLyra


#define BOOL2TEXT(b)  ((b) ? *TrueString : *FalseString)

EXTERN_C const FString FalseString;
EXTERN_C const FString TrueString;


/**
 * @brief Write a log of the specified level to the LogLyra log
 * Note: You will want to modify this in your project, don't write to Lyra's log! I'm just doing it here because I'm lazy.
 * @param Level UE_LOG Level
 * @param fmt FString::Printf format string
 */
#define XISTED_LEVEL_LOG(Level, fmt, ...) \
	UE_LOG(LogLyra, Level, TEXT("%s"), \
		*FString::Printf(TEXT("%05llu %s:%i %s"), \
			GFrameCounter, *FString(__FUNCTION__), __LINE__, \
			*FString::Printf(fmt, ##__VA_ARGS__)))


#define XISTED_ERROR_LOG(fmt, ...)       XISTED_LEVEL_LOG(Error, fmt, ##__VA_ARGS__)
#define XISTED_DISPLAY_LOG(fmt, ...)     XISTED_LEVEL_LOG(Display, fmt, ##__VA_ARGS__)
#define XISTED_LOG(fmt, ...)             XISTED_LEVEL_LOG(Log, fmt, ##__VA_ARGS__)
#define XISTED_WARNING_LOG(fmt, ...)     XISTED_LEVEL_LOG(Warning, fmt, ##__VA_ARGS__)
