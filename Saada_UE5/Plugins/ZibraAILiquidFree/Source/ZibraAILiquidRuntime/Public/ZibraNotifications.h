// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class ZIBRAAILIQUIDRUNTIME_API ZibraNotifications
{
public:
	ZibraNotifications();
	~ZibraNotifications();

	static TArray<FString> ShownMessages;
	static void AddNotification(FString Message, float Duration = 5, bool bAllowRepeat = true);
	static void ResetShownMessages();
};
