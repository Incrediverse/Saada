// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraLiquidDebug.h"

void BridgeStructs::DebugLogCallback(void* request)
{
	DebugMessage* message = (DebugMessage*)(request);
	FString text = message->text;
	switch (message->level)
	{
	case LogLevel::Verbose:
		UE_LOG(LogZibraAILiquidRuntime, Verbose, TEXT("ZibraLiquid[silly]: %s"), *text);
		break;
	case LogLevel::Info:
		UE_LOG(LogZibraAILiquidRuntime, Display, TEXT("ZibraLiquid: %s"), *text);
		break;
	case LogLevel::Warning:
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("ZibraLiquid: %s"), *text);
		break;
	case LogLevel::Performance:
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("ZibraLiquid | Performance Warning: %s"), *text);
		break;
	case LogLevel::Error:
		UE_LOG(LogZibraAILiquidRuntime, Error, TEXT("ZibraLiquid: %s"), *text);
		break;
	default:
		UE_LOG(LogZibraAILiquidRuntime, Warning, TEXT("ZibraLiquid | Incorrect native log data format."));
		break;
	}
}