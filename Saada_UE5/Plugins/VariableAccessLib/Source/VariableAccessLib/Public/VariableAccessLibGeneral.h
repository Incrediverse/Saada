// Copyright © Pixeltale Motion 2021. All rights reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Math/TransformNonVectorized.h"
#include "Math/IntPoint.h"
#include "Math/IntVector.h"
#include "Containers/Array.h"
#include "Containers/Map.h"
#include "Materials/MaterialInstanceConstant.h"
#include "UObject/UObjectGlobals.h"
#include "Misc/EnumClassFlags.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "VariableAccessLibGeneral.generated.h"


UCLASS()
class UVariableAccessLibGeneral : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

		

	//Add Flot to Float from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Math")
		static bool AddFloatToFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, float AddValue, float& outFloat);

	//Add int to int from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Math")
		static bool AddIntToIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int32 AddValue, int32& outInt);

	//Add Float to Clamped Float from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Math")
		static bool AddFloatToClampedFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, float AddValue, float ClampMin, float ClampMax, float& outFloat);

	//Add Int to Clamped Int from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Math")
		static bool AddIntToClampedIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int AddValue, int ClampMin, int ClampMax, int& outFloat);


	//Multiply Flot with Float from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Math")
		static bool MultiplyFloatWithFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, float MultiplicationValue, float& outFloat);

	//Multiply Int with Int from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Math")
		static bool MultiplyIntWithIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int32 MultiplicationValue, int32& outInt);

	//Divide Float with Float from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Math")
		static bool DivideFloatWithFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, float DivisionValue, float& outFloat);

	//Scale Vector from target by name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Math")
		static bool ScaleVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, float ScaleValue, FVector& outVector);

	//Set Loc of Transform
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetLocOfTransByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector NewValue, FTransform& outTransform);

	//Set Rot of Transform
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetRotOfTransByName(UObject* Target, UObject* WorldContextObject, FName VarName, FRotator NewValue, FTransform& outTransform);

	//Set Scale of Transform
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Set")
		static bool SetScaleOfTransByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector NewValue, FTransform& outTransform);

	//Get All Variables of Target
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Helper")
		static bool GetAllVariableNames(UObject* Target, UObject* WorldContextObject, bool GetInheritedVars, TArray<FName>& VarNames);


	//Execute Function by Name
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Helper")
		static bool ExecFunctionByName(UObject* Target, UObject* WorldContextObject, FName FunctionName, const TArray<FString>& Parameters);

	//Get All Function Names of Target
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Helper")
		static bool GetAllFunctionNames(UObject* Target, UObject* WorldContextObject, bool GetParentFunctions, TArray<FName>& FuncNames);
/*
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Helper")
		static UMaterialInterface* FindMatInstanceFromContent(FString Dir, FString Name, FString& FullPath);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Helper")
		static UStaticMesh* FindMeshInstanceFromContent(FString Dir, FString Name, FString& FullPath);


	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject"), Category = "VariableAccessLib | Helper")
		static UTexture2D* FindTextureFromContent(FString Dir, FString Name, FString& FullPath);
		*/
};

