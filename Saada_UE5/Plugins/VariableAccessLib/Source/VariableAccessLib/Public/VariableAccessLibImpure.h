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
#include "VariableAccessLibImpure.generated.h"


UCLASS()
class UVariableAccessLibImpure : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()


		//Get Bool from target by name (Impure) (Impure)
		UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureBoolByName(UObject* Target, UObject* WorldContextObject,  FName VarName, bool& outBool);

	//Get Float from target by name (Impure) (Impure)
	UFUNCTION(BlueprintCallable,meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, double& outFloat);

	//Get Int from target by name (Impure) (Impure)
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureIntByName(UObject* Target, UObject* WorldContextObject,  FName VarName, int32& outInt);

	//Get Byte from target by name (Impure)
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureByteByName(UObject* Target, UObject* WorldContextObject,  FName VarName, uint8& outByte);

	//Get Vector from target by name (Impure)
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector& outVector);




	//Get Name from target by name (Impure) 
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureNameByName(UObject* Target, UObject* WorldContextObject, FName VarName, FName& outName);

	//Get Text from target by name (Impure)
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureTextByName(UObject* Target, UObject* WorldContextObject, FName VarName, FText& outText);

	//Get String from target by name (Impure)
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureStringByName(UObject* Target, UObject* WorldContextObject, FName VarName, FString& outString);

	//Get Rotator from target by name (Impure)
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureRotatorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FRotator& outRotator);

	//Get Transform from target by name (Impure)
	UFUNCTION(BlueprintCallable,  meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureTransformByName(UObject* Target, UObject* WorldContextObject, FName VarName, FTransform& outTransform);

	//Get LinearColor from target by name (Impure)
	UFUNCTION(BlueprintCallable,  meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureLinearColorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FLinearColor& outColor);





	//Get Vector2d from target by name (Impure)
	UFUNCTION(BlueprintCallable,  meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureVector2dByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector2D& outVector2D);

	//Get IntPoint from target by name (Impure)
	UFUNCTION(BlueprintCallable,  meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureIntPointByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntPoint& outIntPoint);

	//Get IntVector from target by name (Impure)
	UFUNCTION(BlueprintCallable,  meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureIntVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntVector& outIntVector);

	//Get Int64 from target by name (Impure)
	UFUNCTION(BlueprintCallable,  meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureInt64ByName(UObject* Target, UObject* WorldContextObject, FName VarName, int64& outInt);

	//Get Object from target by name (Impure)
	UFUNCTION(BlueprintCallable,  meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureObjectByName(UObject* Target, UObject* WorldContextObject, FName VarName, UObject*& outObject);


	//Get Wildcard Array from target by name (Impure)
	UFUNCTION(BlueprintCallable,  meta = (ArrayParm = "outArray", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureWildcardArrayByName(UObject* Target, UObject* WorldContextObject, FName VarName, TArray<UProperty*>& outArray);

	//Get Wildcard Map from target by name (Impure)
	UFUNCTION(BlueprintCallable, meta = (MapParam = "outMap", HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureWildcardMapByName(UObject* Target, UObject* WorldContextObject, FName VarName, TMap<UProperty*, UProperty*>& outMap);

	//Get DataTableRowHandle from target by name (Impure)
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Impure")
		static bool GetImpureDataTableRowHandleByName(UObject* Target, UObject* WorldContextObject, FName VarName, FDataTableRowHandle& outDataTableRowHandle);

	//Get DataTableRef from target by name (Impure)
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetImpureDataTableRefByName(UObject* Target, UObject* WorldContextObject, FName VarName, UDataTable*& outDataTable);

	//Get DefinedObject from target by name (Impure)
	//Select your desired class to get references to custom objects
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DeterminesOutputType = "DesiredClass"), Category = "VariableAccessLib | Get")
		static class UObject* GetImpureDefinedObjectByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<UObject> DesiredClass, bool& VariableFound);
	 
	//Get Defined Actor from target by name (Impure)
	//Select your desired class to get references to custom actors
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DeterminesOutputType = "DesiredClass"), Category = "VariableAccessLib | Get")
		static class AActor* GetImpureDefinedActorByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<AActor> DesiredClass, bool& VariableFound);

	//Get Defined UserWidget from target by name (Impure)
	//Select your desired class to get references to custom widgets
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", DeterminesOutputType = "DesiredClass"), Category = "VariableAccessLib | Get")
		static class UUserWidget* GetImpureDefinedWidgetByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<UUserWidget> DesiredClass, bool& VariableFound);


	//Get GameplayTagContainer from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetImpureGameplayTagContainerByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTagContainer& outTagContainer);

	//Get GameplayTagContainer from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Get")
		static bool GetImpureGameplayTagByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTag& outTag);
};


