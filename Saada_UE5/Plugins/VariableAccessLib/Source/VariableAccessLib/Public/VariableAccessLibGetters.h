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
#include "Engine/UserDefinedStruct.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "UObject/SoftObjectPtr.h"
#include "VariableAccessLibGetters.generated.h"


UCLASS()
class UVariableAccessLibGetters : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


	//Get Bool from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetBoolByName(UObject* Target, UObject* WorldContextObject, FName VarName, bool &outBool);

	//Get Float from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, double& outFloat);

	//Get Int from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int32 &outInt);

	//Get Byte from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetByteByName(UObject* Target, UObject* WorldContextObject, FName VarName, uint8 &outByte);

	//Get Vector from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector &outVector);



	
	//Get Name from target by name 
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetNameByName(UObject* Target, UObject* WorldContextObject, FName VarName, FName &outName);

	//Get Text from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetTextByName(UObject* Target, UObject* WorldContextObject, FName VarName, FText &outText);

	//Get String from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetStringByName(UObject* Target, UObject* WorldContextObject, FName VarName, FString &outString);

	//Get Rotator from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetRotatorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FRotator &outRotator);

	//Get Transform from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetTransformByName(UObject* Target, UObject* WorldContextObject, FName VarName, FTransform &outTransform);

	//Get LinearColor from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetLinearColorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FLinearColor &outColor);



	

	//Get Vector2d from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetVector2dByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector2D &outVector2D);

	//Get IntPoint from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetIntPointByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntPoint &outIntPoint);

	//Get IntVector from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetIntVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntVector &outIntVector);

	//Get Int64 from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetInt64ByName(UObject* Target, UObject* WorldContextObject, FName VarName, int64 &outInt);

	//Get Object from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetObjectByName(UObject* Target, UObject* WorldContextObject, FName VarName, UObject*& outObject);

	//Get Wildcard Array from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (ArrayTypeDependentParams = "Type", ArrayParm = "outArray", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetWildcardArrayByName(UObject* Target, UObject* WorldContextObject, FName VarName, TArray<UProperty*>& outArray);

	//Get Wildcard Map from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (MapParam= "outMap", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetWildcardMapByName(UObject* Target, UObject* WorldContextObject, FName VarName, TMap<UProperty*, UProperty*>& outMap);

	//Get DataTableRef from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetDataTableRefByName(UObject* Target, UObject* WorldContextObject, FName VarName, UDataTable*& outDataTable);
		
	//Get DataTableRowHandle from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetDataTableRowHandleByName(UObject* Target, UObject* WorldContextObject, FName VarName, FDataTableRowHandle& outDataTableRowHandle);

	//Get DefinedObject from target by name
	//Select your desired class to get references to custom objects
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DeterminesOutputType = "DesiredClass"), Category = "VariableAccessLib | Get")
		static class  UObject* GetDefinedObjectByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<UObject> DesiredClass, bool& VariableFound);

	//Get Defined Actor from target by name
	//Select your desired class to get references to custom actors
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DeterminesOutputType = "DesiredClass"), Category = "VariableAccessLib | Get")
		static class AActor* GetDefinedActorByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<AActor> DesiredClass, bool& VariableFound);

	//Get Defined UserWidget from target by name
	//Select your desired class to get references to custom widgets
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DeterminesOutputType = "DesiredClass"), Category = "VariableAccessLib | Get")
		static class UUserWidget* GetDefinedWidgetByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<UUserWidget> DesiredClass, bool& VariableFound);

	//Get GameplayTagContainer from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetGameplayTagContainerByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTagContainer& outTagContainer);

	//Get GameplayTagContainer from target by name
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetGameplayTagByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTag& outTag);

};