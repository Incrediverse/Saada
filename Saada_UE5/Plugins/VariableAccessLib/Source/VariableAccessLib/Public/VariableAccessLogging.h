// Copyright © Pixeltale Motion 2021. All rights reserved.

#pragma once
#include "Logging/MessageLog.h" 
#include "VariableAccessLogging.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(VariableAccess, Log, All);

UCLASS()
class UVariableAccessLogging : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


		//PrintErrorMessages
		UFUNCTION()
		static void PrintErrorMsg(FName VarName, UObject* Target, UObject* WorldContextObject);

		//FMessageLog PIELogger = FMessageLog(FName("VariableAccess"));

};