// Copyright © Pixeltale Motion 2021. All rights reserved.


#include "VariableAccessLibImpure.h"
#include "VariableAccessLib.h"
#include "VariableAccessLibGetters.h"
#include "VariableAccessLogging.h"



UVariableAccessLibImpure::UVariableAccessLibImpure(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}



/* SUPPORTED FUNCTION V1.0*/

/*Get Bool by Variable Name*/
bool UVariableAccessLibImpure::GetImpureBoolByName(UObject* Target, UObject* WorldContextObject, FName VarName, bool& outBool)
{
	return UVariableAccessLibGetters::GetBoolByName(Target, WorldContextObject, VarName, outBool);
}

//Get Float by Variable Name
bool UVariableAccessLibImpure::GetImpureFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, double& outFloat)
{
	return UVariableAccessLibGetters::GetFloatByName(Target, WorldContextObject, VarName, outFloat);
}

//Get Int by Variable Name
bool UVariableAccessLibImpure::GetImpureIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int32& outInt)
{
	return UVariableAccessLibGetters::GetIntByName(Target, WorldContextObject, VarName, outInt);
}

/*Get Byte by Variable Name*/
bool UVariableAccessLibImpure::GetImpureByteByName(UObject* Target, UObject* WorldContextObject, FName VarName, uint8& outByte)
{
	return UVariableAccessLibGetters::GetByteByName(Target, WorldContextObject, VarName, outByte);
}

/*Get Vector by Variable Name*/
bool UVariableAccessLibImpure::GetImpureVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector& outVector)
{

	return UVariableAccessLibGetters::GetVectorByName(Target, WorldContextObject, VarName, outVector);
}



/*Get String by Variable Name*/
bool UVariableAccessLibImpure::GetImpureStringByName(UObject* Target, UObject* WorldContextObject, FName VarName, FString& outString)
{

	return UVariableAccessLibGetters::GetStringByName(Target, WorldContextObject, VarName, outString);
}

/*Get Name by Variable Name*/
bool UVariableAccessLibImpure::GetImpureNameByName(UObject* Target, UObject* WorldContextObject, FName VarName, FName& outName)
{

	return UVariableAccessLibGetters::GetNameByName(Target, WorldContextObject, VarName, outName);
}

/*Get Text by Variable Name*/
bool UVariableAccessLibImpure::GetImpureTextByName(UObject* Target, UObject* WorldContextObject, FName VarName, FText& outText)
{

	return UVariableAccessLibGetters::GetTextByName(Target, WorldContextObject, VarName, outText);
}


/*Get Rotator by Variable Name*/
bool UVariableAccessLibImpure::GetImpureRotatorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FRotator& outRotator)
{

	return UVariableAccessLibGetters::GetRotatorByName(Target, WorldContextObject, VarName, outRotator);
}

/*Get Transform by Variable Name*/
bool UVariableAccessLibImpure::GetImpureTransformByName(UObject* Target, UObject* WorldContextObject, FName VarName, FTransform& outTransform)
{

	return UVariableAccessLibGetters::GetTransformByName(Target, WorldContextObject, VarName, outTransform);
}

/*Get Linear Color by Variable Name*/
bool UVariableAccessLibImpure::GetImpureLinearColorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FLinearColor& outLinearColor)
{

	return UVariableAccessLibGetters::GetLinearColorByName(Target, WorldContextObject, VarName, outLinearColor);
}




/*Get Vector2d by Variable Name*/
bool UVariableAccessLibImpure::GetImpureVector2dByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector2D& outVector2D)
{
	return UVariableAccessLibGetters::GetVector2dByName(Target, WorldContextObject, VarName, outVector2D);
}

/*Get IntPoint by Variable Name*/
bool UVariableAccessLibImpure::GetImpureIntPointByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntPoint& outIntPoint)
{
	return UVariableAccessLibGetters::GetIntPointByName(Target, WorldContextObject, VarName, outIntPoint);
}

/*Get IntVector by Variable Name*/
bool UVariableAccessLibImpure::GetImpureIntVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntVector& outIntVector)
{
	return UVariableAccessLibGetters::GetIntVectorByName(Target, WorldContextObject, VarName, outIntVector);
}

/*Get Int64 by Variable Name*/
bool UVariableAccessLibImpure::GetImpureInt64ByName(UObject* Target, UObject* WorldContextObject, FName VarName, int64& outInt64)
{
	return UVariableAccessLibGetters::GetInt64ByName(Target, WorldContextObject, VarName, outInt64);
}

/*Get Object by Variable Name*/
bool UVariableAccessLibImpure::GetImpureObjectByName(UObject* Target, UObject* WorldContextObject, FName VarName, UObject*& outObject)
{
	return UVariableAccessLibGetters::GetObjectByName(Target, WorldContextObject, VarName, outObject);
}



//Get Wildcard Array from target by name
bool UVariableAccessLibImpure::GetImpureWildcardArrayByName(UObject* Target, UObject* WorldContextObject, FName VarName, TArray<UProperty*>& outValue)
{
	return UVariableAccessLibGetters::GetWildcardArrayByName(Target, WorldContextObject, VarName, outValue);
}
//Get Wildcard Map from target by name
bool UVariableAccessLibImpure::GetImpureWildcardMapByName(UObject* Target, UObject* WorldContextObject, FName VarName, TMap<UProperty*, UProperty*>& outValue)
{
	return UVariableAccessLibGetters::GetWildcardMapByName(Target, WorldContextObject, VarName, outValue);
}

//Get Datatable Row Handle
bool UVariableAccessLibImpure::GetImpureDataTableRowHandleByName(UObject* Target, UObject* WorldContextObject, FName VarName, FDataTableRowHandle& outDataTableRowHandle)
{
	return UVariableAccessLibGetters::GetDataTableRowHandleByName(Target, WorldContextObject, VarName, outDataTableRowHandle);

}
/*Get DataTableRef by Variable Name*/
bool UVariableAccessLibImpure::GetImpureDataTableRefByName(UObject* Target, UObject* WorldContextObject, FName VarName, UDataTable*& outDataTable)
{
	return UVariableAccessLibGetters::GetDataTableRefByName(Target, WorldContextObject, VarName, outDataTable);
}

/*Get DefinedObject by Variable Name*/
UObject* UVariableAccessLibImpure::GetImpureDefinedObjectByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<UObject> DesiredClass, bool& VariableFound)
{
	return UVariableAccessLibGetters::GetDefinedObjectByName(Target, WorldContextObject, VarName, DesiredClass, VariableFound);
}

/*Get Defined Actor by Variable Name*/
AActor* UVariableAccessLibImpure::GetImpureDefinedActorByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<AActor> DesiredClass, bool& VariableFound)
{
	return UVariableAccessLibGetters::GetDefinedActorByName(Target, WorldContextObject, VarName, DesiredClass, VariableFound);
}
/*Get Defined Widget by Variable Name*/
UUserWidget* UVariableAccessLibImpure::GetImpureDefinedWidgetByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<UUserWidget> DesiredClass, bool& VariableFound)
{
	return UVariableAccessLibGetters::GetDefinedWidgetByName(Target, WorldContextObject, VarName, DesiredClass, VariableFound);
}


bool UVariableAccessLibImpure::GetImpureGameplayTagContainerByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTagContainer& outTagContainer)
{
	return UVariableAccessLibGetters::GetGameplayTagContainerByName(Target, WorldContextObject, VarName, outTagContainer);
}

bool UVariableAccessLibImpure::GetImpureGameplayTagByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTag& outTag)
{
	return UVariableAccessLibGetters::GetGameplayTagByName(Target, WorldContextObject, VarName, outTag);
}