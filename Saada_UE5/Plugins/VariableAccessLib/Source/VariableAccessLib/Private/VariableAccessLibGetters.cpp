// Copyright © Pixeltale Motion 2021. All rights reserved.


#include "VariableAccessLibGetters.h"
#include "VariableAccessLib.h"
#include "VariableAccessLogging.h"



UVariableAccessLibGetters::UVariableAccessLibGetters(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}



/* SUPPORTED FUNCTION V1.0*/

/*Get Bool by Variable Name*/
bool UVariableAccessLibGetters::GetBoolByName(UObject* Target, UObject* WorldContextObject, FName VarName, bool &outBool)
{
	if (Target) //make sure Target was set in blueprints. 
	{
		bool FoundBool;
		FBoolProperty* BoolProp = FindFProperty<FBoolProperty>(Target->GetClass(), VarName);  
		if (BoolProp) //if we found variable
		{
			FoundBool = BoolProp->GetPropertyValue_InContainer(Target);  
			outBool = FoundBool;  
			return true; // we can return
		}
	}
	UVariableAccessLogging::UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false; // we haven't found variable return false
}


//Get Float by Variable Name
bool UVariableAccessLibGetters::GetFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, double& outFloat)
{
	if (Target) //make sure Target was set in blueprints. 
	{
		double FoundFloat;
		FDoubleProperty* FloatProp = FindFProperty<FDoubleProperty>(Target->GetClass(), VarName);  // try to find float property in Target named VarName
		if (FloatProp) //if we found variable
		{
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);  // get the value from FloatProp
			outFloat = FoundFloat;  // return float
			return true; // we can return
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false; // we haven't found variable return false
}

//Get Int by Variable Name
bool UVariableAccessLibGetters::GetIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int32 &outInt)
{
	if (Target)
	{
		int32 FoundInt;
		FIntProperty* IntProp = FindFProperty<FIntProperty>(Target->GetClass(), VarName);
		if (IntProp)
		{
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			outInt = FoundInt;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get Byte by Variable Name*/
bool UVariableAccessLibGetters::GetByteByName(UObject* Target, UObject* WorldContextObject, FName VarName, uint8 &outByte)
{
	if (Target)
	{
		uint8 FoundByte;
		FByteProperty* ByteProp = FindFProperty<FByteProperty>(Target->GetClass(), VarName);
		if (ByteProp)
		{
			FoundByte = ByteProp->GetPropertyValue_InContainer(Target);
			outByte = FoundByte;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get Vector by Variable Name*/
bool UVariableAccessLibGetters::GetVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector &outVector)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			void* StructAddress = Prop->ContainerPtrToValuePtr<void>(Target);

			if (StructAddress)
			{
				// Ensure this property is a struct
				if (FStructProperty * StructProp = CastField<FStructProperty>(Prop))
				{
					// Ensure this struct is a Vector
					if (StructProp->Struct == TBaseStructure<FVector>::Get())
					{
						StructProp->CopyCompleteValue(&outVector, StructAddress);
						return true;
					}
				}
			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}



/*Get String by Variable Name*/
bool UVariableAccessLibGetters::GetStringByName(UObject* Target, UObject* WorldContextObject, FName VarName, FString &outString)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			FString* FoundString = Prop->ContainerPtrToValuePtr <FString>(Target);

			if (FoundString)
			{
				outString = *FoundString;
				return true;

			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get Name by Variable Name*/
bool UVariableAccessLibGetters::GetNameByName(UObject* Target, UObject* WorldContextObject, FName VarName, FName &outName)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			FName* FoundName = Prop->ContainerPtrToValuePtr <FName>(Target);

			if (FoundName)
			{
				outName = *FoundName;
				return true;

			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get Text by Variable Name*/
bool UVariableAccessLibGetters::GetTextByName(UObject* Target, UObject* WorldContextObject, FName VarName, FText &outText)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			FText* FoundText = Prop->ContainerPtrToValuePtr <FText>(Target);

			if (FoundText)
			{
				outText = *FoundText;
				return true;

			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}


/*Get Rotator by Variable Name*/
bool UVariableAccessLibGetters::GetRotatorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FRotator &outRotator)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			void* StructAddress = Prop->ContainerPtrToValuePtr<void>(Target);

			if (StructAddress)
			{
				// Ensure this property is a struct
				if (FStructProperty * StructProp = CastField<FStructProperty>(Prop))
				{
					// Ensure this struct is a Rotator
					if (StructProp->Struct == TBaseStructure<FRotator>::Get())
					{
						StructProp->CopyCompleteValue(&outRotator, StructAddress);
						return true;
					}
				}
			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get Transform by Variable Name*/
bool UVariableAccessLibGetters::GetTransformByName(UObject* Target, UObject* WorldContextObject, FName VarName, FTransform &outTransform)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			void* StructAddress = Prop->ContainerPtrToValuePtr<void>(Target);

			if (StructAddress)
			{
				// Ensure this property is a struct
				if (FStructProperty * StructProp = CastField<FStructProperty>(Prop))
				{
					// Ensure this struct is a Transform
					if (StructProp->Struct == TBaseStructure<FTransform>::Get())
					{
						StructProp->CopyCompleteValue(&outTransform, StructAddress);
						return true;
					}
				}
			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get Linear Color by Variable Name*/
bool UVariableAccessLibGetters::GetLinearColorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FLinearColor &outLinearColor)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			void* StructAddress = Prop->ContainerPtrToValuePtr<void>(Target);

			if (StructAddress)
			{
				// Ensure this property is a struct
				if (FStructProperty * StructProp = CastField<FStructProperty>(Prop))
				{
					// Ensure this struct is a LinearColor
					if (StructProp->Struct == TBaseStructure<FLinearColor>::Get())
					{
						StructProp->CopyCompleteValue(&outLinearColor, StructAddress);
						return true;
					}
				}
			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}




/*Get Vector2d by Variable Name*/
bool UVariableAccessLibGetters::GetVector2dByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector2D &outVector2D)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			void* StructAddress = Prop->ContainerPtrToValuePtr<void>(Target);

			if (StructAddress)
			{
				// Ensure this property is a struct
				if (FStructProperty * StructProp = CastField<FStructProperty>(Prop))
				{
					// Ensure this struct is a Vector2D
					if (StructProp->Struct == TBaseStructure<FVector2D>::Get())
					{
						StructProp->CopyCompleteValue(&outVector2D, StructAddress);
						return true;
					}
				}
			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get IntPoint by Variable Name*/
bool UVariableAccessLibGetters::GetIntPointByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntPoint &outIntPoint)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			FIntPoint* FoundIntPoint = Prop->ContainerPtrToValuePtr <FIntPoint>(Target);

			if (FoundIntPoint)
			{
				outIntPoint = *FoundIntPoint;
				return true;

			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get IntVector by Variable Name*/
bool UVariableAccessLibGetters::GetIntVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntVector &outIntVector)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			FIntVector* FoundIntVector = Prop->ContainerPtrToValuePtr <FIntVector>(Target);

			if (FoundIntVector)
			{
				outIntVector = *FoundIntVector;
				return true;

			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get Int64 by Variable Name*/
bool UVariableAccessLibGetters::GetInt64ByName(UObject* Target, UObject* WorldContextObject, FName VarName, int64 &outInt64)
{
	if (Target)
	{
		int64 FoundInt64;
		FInt64Property* Int64Prop = FindFProperty<FInt64Property>(Target->GetClass(), VarName);
		if (Int64Prop)
		{
			FoundInt64 = Int64Prop->GetPropertyValue_InContainer(Target);
			outInt64 = FoundInt64;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get Object by Variable Name*/
bool UVariableAccessLibGetters::GetObjectByName(UObject* Target, UObject* WorldContextObject, FName VarName, UObject* &outObject)
{
	if (Target)
	{
		UObject* FoundObject;
		FObjectProperty* ObjectProp = FindFProperty<FObjectProperty>(Target->GetClass(), VarName);
		if (ObjectProp)
		{
			FoundObject = ObjectProp->GetPropertyValue_InContainer(Target);
			outObject = FoundObject;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}



//Get Wildcard Array from target by name
bool UVariableAccessLibGetters::GetWildcardArrayByName(UObject* Target, UObject* WorldContextObject, FName VarName, TArray<UProperty*>& outValue)
{
	if (Target)
	{
		FProperty* Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			TArray<UProperty*> *FoundArray = Prop->ContainerPtrToValuePtr <TArray<UProperty*>>(Target);
			outValue = *FoundArray;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

//Get Wildcard Map from target by name
bool UVariableAccessLibGetters::GetWildcardMapByName(UObject* Target, UObject* WorldContextObject, FName VarName, TMap<UProperty*, UProperty*>& outValue)
{
	if (Target)
	{

		FMapProperty* ArrayProp = FindFProperty<FMapProperty>(Target->GetClass(), VarName);
		if (ArrayProp)
		{
			const TMap<UProperty*, UProperty*> FoundArray = (const TMap<UProperty*, UProperty*>&) ArrayProp->GetPropertyValue_InContainer(Target);
			outValue = FoundArray;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

//Get Datatable Row Handle
bool UVariableAccessLibGetters::GetDataTableRowHandleByName(UObject* Target, UObject* WorldContextObject, FName VarName, FDataTableRowHandle& outDataTableRowHandle)
{
	if (Target)
	{
		FProperty* Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			void* StructAddress = Prop->ContainerPtrToValuePtr<void>(Target);

			if (StructAddress)
			{
				// Ensure this property is a struct
				if (FStructProperty* StructProp = CastField<FStructProperty>(Prop))
				{
					// Ensure this struct is a Vector2D
					if (StructProp->Struct == TBaseStructure<FDataTableRowHandle>::Get())
					{
						StructProp->CopyCompleteValue(&outDataTableRowHandle, StructAddress);
						return true;
					}
				}
			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;

}

/*Get DataTableRef by Variable Name*/
bool UVariableAccessLibGetters::GetDataTableRefByName(UObject* Target, UObject* WorldContextObject, FName VarName, UDataTable*& outDataTable)
{
	if (Target)
	{
		UObject* FoundObject;
		FObjectProperty* ObjectProp = FindFProperty<FObjectProperty>(Target->GetClass(), VarName);
		if (ObjectProp)
		{
			FoundObject = ObjectProp->GetPropertyValue_InContainer(Target);
			outDataTable = Cast<UDataTable>(FoundObject);
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Get DefinedObject by Variable Name*/
UObject* UVariableAccessLibGetters::GetDefinedObjectByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<UObject> DesiredClass, bool& VariableFound)
{
		UObject* FoundObject;
		FObjectProperty* ObjectProp = FindFProperty<FObjectProperty>(Target->GetClass(), VarName);
		if (ObjectProp)
		{
			FoundObject = ObjectProp->GetPropertyValue_InContainer(Target);

			VariableFound = true;
			return FoundObject;
		}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	VariableFound = false;
	return NULL;
}

/*Get Defined Actor by Variable Name*/
AActor* UVariableAccessLibGetters::GetDefinedActorByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<AActor> DesiredClass, bool& VariableFound)
{
	
	UObject* FoundObject;
	FObjectProperty* ObjectProp = FindFProperty<FObjectProperty>(Target->GetClass(), VarName);
	if (ObjectProp)
	{
		FoundObject = ObjectProp->GetPropertyValue_InContainer(Target);

		VariableFound = true;
		return Cast<AActor>(FoundObject);
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	VariableFound = false;
	return NULL;
}
/*Get Defined Widget by Variable Name*/
UUserWidget* UVariableAccessLibGetters::GetDefinedWidgetByName(UObject* Target, UObject* WorldContextObject, FName VarName, TSubclassOf<UUserWidget> DesiredClass, bool& VariableFound)
{
	UObject* FoundObject;
	FObjectProperty* ObjectProp = FindFProperty<FObjectProperty>(Target->GetClass(), VarName);
	if (ObjectProp)
	{
		FoundObject = ObjectProp->GetPropertyValue_InContainer(Target);

		VariableFound = true;
		return Cast<UUserWidget>(FoundObject);
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	VariableFound = false;
	return NULL;
}

bool UVariableAccessLibGetters::GetGameplayTagContainerByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTagContainer& outTagContainer)
{
		if (Target)
	{
		FProperty* Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			void* StructAddress = Prop->ContainerPtrToValuePtr<void>(Target);

			if (StructAddress)
			{
				// Ensure this property is a struct
				if (FStructProperty* StructProp = CastField<FStructProperty>(Prop))
				{
					// Ensure this struct is a Vector
					if (StructProp->Struct == TBaseStructure<FGameplayTagContainer>::Get())
					{
						StructProp->CopyCompleteValue(&outTagContainer, StructAddress);
						return true;
					}
				}
			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

bool UVariableAccessLibGetters::GetGameplayTagByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTag& outTag)
{
	if (Target)
	{
		FProperty* Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			void* StructAddress = Prop->ContainerPtrToValuePtr<void>(Target);

			if (StructAddress)
			{
				// Ensure this property is a struct
				if (FStructProperty* StructProp = CastField<FStructProperty>(Prop))
				{
					// Ensure this struct is a Vector
					if (StructProp->Struct == TBaseStructure<FGameplayTag>::Get())
					{
						StructProp->CopyCompleteValue(&outTag, StructAddress);
						return true;
					}
				}
			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}