// Copyright © Pixeltale Motion 2021. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Math/TransformNonVectorized.h"
#include "Math/IntPoint.h"
#include "Math/IntVector.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "VariableAccessLibSetters.generated.h"


UCLASS()
class UVariableAccessLibSetters : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


	//Set Bool from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetBoolByName(UObject* Target, UObject* WorldContextObject, FName VarName, bool NewValue, bool &outBool);

	//Set Float from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, double NewValue, double& outFloat);

	//Set Int from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int32 NewValue, int32 &outInt);

	//Set Byte from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetByteByName(UObject* Target, UObject* WorldContextObject, FName VarName, uint8 NewValue, uint8 &outByte);

	//Set Vector from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector NewValue, FVector &outVector);



	//Set String from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetStringByName(UObject* Target, UObject* WorldContextObject, FName VarName, FString NewValue, FString &outString);

	//Set Text from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetTextByName(UObject* Target, UObject* WorldContextObject, FName VarName, FText NewValue, FText &outText);

	//Set Name from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetNameByName(UObject* Target, UObject* WorldContextObject, FName VarName, FName NewValue, FName &outName);

	//Set Rotator from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetRotatorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FRotator NewValue, FRotator &outRotator);

	//Set Transform from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetTransformByName(UObject* Target, UObject* WorldContextObject, FName VarName, FTransform NewValue, FTransform &outTransform);

	//Set LinearColor from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetLinearColorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FLinearColor NewValue, FLinearColor &outLinearColor);



	//Set Vector2d from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetVector2dByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector2D NewValue, FVector2D &outVector2D);

	//Set IntPoint from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetIntPointByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntPoint NewValue, FIntPoint &outIntPoint);

	//Set IntVector from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetIntVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntVector NewValue, FIntVector &outIntVector);

	//Set Int64 from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetInt64ByName(UObject* Target, UObject* WorldContextObject, FName VarName, int64 NewValue, int64 &outInt64);

	//Set Object from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetObjectByName(UObject* Target, UObject* WorldContextObject, FName VarName, UObject* NewValue, UObject* &outObject);


	//Set Wildcard Array from target by name
	UFUNCTION(BlueprintCallable, meta = (ArrayTypeDependentParams = "NewArray", ArrayParm = "outArray", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetWildcardArrayByName(UObject* Target, UObject* WorldContextObject, FName VarName, TArray<UProperty*> NewArray, TArray<UProperty*>& outArray);

	//Set Wildcard Map from target by name
	UFUNCTION(BlueprintCallable, meta = (MapKeyParam = "NewMap", MapParam= "outMap", AutoCreateRefTerm="NewMap", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetWildcardMapByName(UObject* Target, UObject* WorldContextObject, FName VarName, TMap<UProperty*, UProperty*> NewMap, TMap<UProperty*, UProperty*>& outMap);

	//Set DataTableRowHandle from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (MapParam = "outMap", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool SetDataTableRowHandleByName(UObject* Target, UObject* WorldContextObject, FName VarName, FDataTableRowHandle NewValue, FDataTableRowHandle& outDataTableRowHandle);

	//Set DataTableRef from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool SetDataTableRefByName(UObject* Target, UObject* WorldContextObject, FName VarName, UDataTable* NewValue, UDataTable*& outDataTable);


	//Set GameplayTagContainer from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool SetGameplayTagContainerByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTagContainer NewValue, FGameplayTagContainer& outTagContainer);

	//Set GameplayTagContainer from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool SetGameplayTagByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTag NewValue, FGameplayTag& outTag);

};

