// Copyright Zibra AI, Inc 2022. All Rights Reserved.
#pragma once

namespace BridgeStructs
{
	enum class LogLevel
	{
		Verbose = 0,
		Info = 1,
		Performance = 2,
		Warning = 3,
		Error = 4,
	};

	struct DebugMessage
	{
		const char* text = nullptr;
		LogLevel level = LogLevel::Info;
	};

	struct LoggerSettings
	{
		void* PFNCallback;
		LogLevel LogLevel;
	};

	void DebugLogCallback(void* request);
}