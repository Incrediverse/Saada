// Copyright Zibra AI, Inc 2022. All Rights Reserved.


#include "ZibraNotifications.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

TArray<FString> ZibraNotifications::ShownMessages = TArray<FString>();

ZibraNotifications::ZibraNotifications()
{
}

ZibraNotifications::~ZibraNotifications()
{
}

void ZibraNotifications::ResetShownMessages()
{
	ShownMessages.Empty();
}

void ZibraNotifications::AddNotification(FString Message, float Duration, bool bAllowRepeat)
{
	if (!bAllowRepeat && ShownMessages.Contains(Message))
		return;
	if (bAllowRepeat)
	{
		ShownMessages.Add(Message);
	}
	const FText NotificationErrorText = FText::FromString(Message);
	FNotificationInfo Info(NotificationErrorText);

	//Set a default expire duration
	Info.ExpireDuration = Duration;
	//And call Add Notification, this is pretty much it!
	FSlateNotificationManager::Get().AddNotification(Info);
}

