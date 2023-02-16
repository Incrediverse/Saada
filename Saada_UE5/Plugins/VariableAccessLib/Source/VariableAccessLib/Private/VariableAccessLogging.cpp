// Copyright © Pixeltale Motion 2021. All rights reserved.

#pragma warning(disable: 4800)

#include "VariableAccessLogging.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "EngineGlobals.h"
#include "VariableAccessSettings.h"


DEFINE_LOG_CATEGORY(VariableAccess);

UVariableAccessLogging::UVariableAccessLogging(const FObjectInitializer& ObjectInitlaizer)
	: Super(ObjectInitlaizer)
{

}

void  UVariableAccessLogging::PrintErrorMsg(FName VarName, UObject* Target, UObject* WorldContextObject)
{
	const UVariableAccessSettings* Settings = GetDefault<UVariableAccessSettings>();
	check(Settings);
	bool PrintErrorLog = Settings->bPrintErrorsToLog;
	bool PrintErrorScreen = Settings->bPrintErrorsToScreen;
	FString OwnerName = *WorldContextObject->GetName();
	
	if (PrintErrorLog == true)
	{
		if (Target)
		{
			FString ClassName = *Target->GetName();
			FString Message = FString::Printf(TEXT("%s: The variable '%s' couldn't be found in target '%s' . Please check the name or the target!"), *OwnerName, *VarName.ToString(), *ClassName);
			UE_LOG(VariableAccess, Warning, TEXT("%s"), *Message);
			//FMessageLog(FName("VariableAccess")).Error(FText::FromString(FString::Printf(TEXT("Test"), *Message)));
		}

		else {
			FString Message = FString::Printf(TEXT("%s: The referenced target is empty or doesn't exist!"), *OwnerName);
			UE_LOG(VariableAccess, Error, TEXT("%s"), *Message);
			//FMessageLog(FName("VariableAccess")).Error(FText::FromString(FString::Printf(TEXT("%s"), *Message)));
		}
	}

	if (PrintErrorScreen == true)
	{
		if (Target)
		{
			FString ClassName = *Target->GetName();
			FString Message = FString::Printf(TEXT("%s: The variable '%s' couldn't be found in target '%s' . Please check the name or the target!"), *OwnerName, *VarName.ToString(), *ClassName);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("%s"), *Message));
			//FMessageLog(FName("VariableAccess")).Error(FText::FromString(FString::Printf(TEXT("%s"), *Message)));
		}

		else {
			FString Message = FString::Printf(TEXT("%s: The referenced target is empty or doesn't exist!"), *OwnerName);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s"), *Message));
			//FMessageLog(FName("VariableAccess")).Error(FText::FromString(FString::Printf(TEXT("%s"), *Message)));
		}

	}

}


