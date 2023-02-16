// Copyright © Pixeltale Motion 2021. All rights reserved.


#include "VariableAccessLibSetters.h"
#include "VariableAccessLib.h"
#include "VariableAccessLogging.h"



UVariableAccessLibSetters::UVariableAccessLibSetters(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}



/*set Bool by Variable Name*/
bool UVariableAccessLibSetters::SetBoolByName(UObject* Target, UObject* WorldContextObject, FName VarName, bool NewValue, bool & outBool)
{
	if (Target)
	{
		bool FoundBool;
		FBoolProperty* BoolProp = FindFProperty<FBoolProperty>(Target->GetClass(), VarName);
		if (BoolProp)
		{
			BoolProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			FoundBool = BoolProp->GetPropertyValue_InContainer(Target);
			outBool = FoundBool;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*set Float by Variable Name*/
bool UVariableAccessLibSetters::SetFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, double NewValue, double& outFloat)
{
	if (Target)
	{
		float FoundFloat;
		FDoubleProperty* FloatProp = FindFProperty<FDoubleProperty>(Target->GetClass(), VarName);
		if (FloatProp)
		{
			FloatProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
			outFloat = FoundFloat;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*set Int by Variable Name*/
bool UVariableAccessLibSetters::SetIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int32 NewValue, int32 & outInt)
{
	if (Target)
	{
		int32 FoundInt;
		FIntProperty* IntProp = FindFProperty<FIntProperty>(Target->GetClass(), VarName);
		if (IntProp)
		{
			IntProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			outInt = FoundInt;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*set Byte by Variable Name*/
bool UVariableAccessLibSetters::SetByteByName(UObject* Target, UObject* WorldContextObject, FName VarName, uint8 NewValue, uint8 & outByte)
{
	if (Target)
	{
		uint8 FoundByte;
		FByteProperty* ByteProp = FindFProperty<FByteProperty>(Target->GetClass(), VarName);
		if (ByteProp)
		{
			ByteProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			FoundByte = ByteProp->GetPropertyValue_InContainer(Target);
			outByte = FoundByte;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*set Vector by Variable Name*/
bool UVariableAccessLibSetters::SetVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector NewValue, FVector &outVector)
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
						// Set Vector to value
						outVector = NewValue;
						StructProp->CopyCompleteValue(StructAddress, &outVector);
						return true;
					}
				}
			}
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}



/* SUPPORTED FUNCTIONS V1.1 */



/*Set String by Variable Name*/
bool UVariableAccessLibSetters::SetStringByName(UObject* Target, UObject* WorldContextObject, FName VarName, FString NewValue, FString &outString)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			FString* FoundString = Prop->ContainerPtrToValuePtr <FString>(Target);

			if (FoundString)
			{
				*FoundString = NewValue;
				outString = *FoundString;
				return true;

			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Set Text by Variable Name*/
bool UVariableAccessLibSetters::SetTextByName(UObject* Target, UObject* WorldContextObject, FName VarName, FText NewValue, FText &outText)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			FText* FoundText = Prop->ContainerPtrToValuePtr <FText>(Target);

			if (FoundText)
			{
				*FoundText = NewValue;
				outText = *FoundText;
				return true;

			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

//*Set Name by Variable Name*/
bool UVariableAccessLibSetters::SetNameByName(UObject* Target, UObject* WorldContextObject, FName VarName, FName NewValue, FName &outName)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			FName* FoundName = Prop->ContainerPtrToValuePtr <FName>(Target);

			if (FoundName)
			{
				*FoundName = NewValue;
				outName = *FoundName;
				return true;

			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}


/*set Rotator by Variable Name*/
bool UVariableAccessLibSetters::SetRotatorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FRotator NewValue, FRotator &outRotator)
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
						// Set Rotator to value
						outRotator = NewValue;
						StructProp->CopyCompleteValue(StructAddress, &outRotator);
						return true;
					}
				}
			}
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*set Transform by Variable Name*/
bool UVariableAccessLibSetters::SetTransformByName(UObject* Target, UObject* WorldContextObject, FName VarName, FTransform NewValue, FTransform &outTransform)
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
						// Set Transform to value

						outTransform = NewValue;
						StructProp->CopyCompleteValue(StructAddress, &outTransform);
						return true;
					}
				}
			}
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*set LinearColor by Variable Name*/
bool UVariableAccessLibSetters::SetLinearColorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FLinearColor NewValue, FLinearColor &outLinearColor)
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
						// Set LinearColor to value
						outLinearColor = NewValue;
						StructProp->CopyCompleteValue(StructAddress, &outLinearColor);
						return true;
					}
				}
			}
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}







/*set Vector2d by Variable Name*/
bool UVariableAccessLibSetters::SetVector2dByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector2D NewValue, FVector2D &outVector2D)
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
					// Ensure this struct is a Vector2d
					if (StructProp->Struct == TBaseStructure<FVector2D>::Get())
					{
						// Set Vector2d to value
						outVector2D = NewValue;
						StructProp->CopyCompleteValue(StructAddress, &outVector2D);
						return true;
					}
				}
			}
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Set IntPoint by Variable Name*/
bool UVariableAccessLibSetters::SetIntPointByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntPoint NewValue, FIntPoint &outIntPoint)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			FIntPoint* FoundIntPoint = Prop->ContainerPtrToValuePtr <FIntPoint>(Target);

			if (FoundIntPoint)
			{
				*FoundIntPoint = NewValue;
				outIntPoint = *FoundIntPoint;
				return true;

			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Set IntVector by Variable Name*/
bool UVariableAccessLibSetters::SetIntVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, FIntVector NewValue, FIntVector &outIntVector)
{

	if (Target)
	{
		FProperty *Prop = Target->GetClass()->FindPropertyByName(VarName);

		if (Prop)
		{
			FIntVector* FoundIntVector = Prop->ContainerPtrToValuePtr <FIntVector>(Target);

			if (FoundIntVector)
			{
				*FoundIntVector = NewValue;
				outIntVector = *FoundIntVector;
				return true;

			}

		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*set Int64 by Variable Name*/
bool UVariableAccessLibSetters::SetInt64ByName(UObject* Target, UObject* WorldContextObject, FName VarName, int64 NewValue, int64 & outInt64)
{
	if (Target)
	{
		int64 FoundInt64;
		FInt64Property* Int64Prop = FindFProperty<FInt64Property>(Target->GetClass(), VarName);
		if (Int64Prop)
		{
			Int64Prop->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			FoundInt64 = Int64Prop->GetPropertyValue_InContainer(Target);
			outInt64 = FoundInt64;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*set Object by Variable Name*/
bool UVariableAccessLibSetters::SetObjectByName(UObject* Target, UObject* WorldContextObject, FName VarName, UObject* NewValue, UObject* & outObject)
{
	if (Target)
	{
		UObject* FoundObject;
		FObjectProperty* ObjectProp = FindFProperty<FObjectProperty>(Target->GetClass(), VarName);
		if (ObjectProp)
		{
			ObjectProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			FoundObject = ObjectProp->GetPropertyValue_InContainer(Target);
			outObject = FoundObject;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}


//Set Wildcard Array from target by name
bool UVariableAccessLibSetters::SetWildcardArrayByName(UObject* Target, UObject* WorldContextObject, FName VarName, TArray<UProperty*> NewArray, TArray<UProperty*>& outArray)
{
	if (Target)
	{
		FArrayProperty* ArrayProp = FindFProperty<FArrayProperty>(Target->GetClass(), VarName);
		if (ArrayProp)
		{
			TArray<UProperty*> FoundArray = (const TArray<UProperty*>&) ArrayProp->GetPropertyValue_InContainer(Target);
			FoundArray.Empty();
		
			for (int i = 0; i < NewArray.Num(); i++)

			{
				FoundArray.Add(NewArray[i]);
			}

			outArray = FoundArray;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

//Set Wildcard Map from target by name
bool UVariableAccessLibSetters::SetWildcardMapByName(UObject* Target, UObject* WorldContextObject, FName VarName, TMap<UProperty*, UProperty*> NewMap, TMap<UProperty*, UProperty*>& outMap)
{
	if (Target)
	{
		FMapProperty* ArrayProp = FindFProperty<FMapProperty>(Target->GetClass(), VarName);
		if (ArrayProp)
		{
			TMap<UProperty*, UProperty*> FoundArray = (const TMap<UProperty*, UProperty*>&) ArrayProp->GetPropertyValue_InContainer(Target);
			FoundArray.Empty();

			for (auto It = NewMap.CreateConstIterator(); It; ++It)

			{
				UProperty* Key= It.Key();
				UProperty* Value =It.Value();
				FoundArray.Add(Key,Value);
			}

			FoundArray=NewMap;


			outMap = FoundArray;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}


//Set DataTableRowHandle from target by name
bool UVariableAccessLibSetters::SetDataTableRowHandleByName(UObject* Target, UObject* WorldContextObject, FName VarName, FDataTableRowHandle NewValue, FDataTableRowHandle& outDataTableRowHandle)
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
					// Ensure this struct is a Transform
					if (StructProp->Struct == TBaseStructure<FTransform>::Get())
					{
						// Set Transform to value

						outDataTableRowHandle=NewValue;
						StructProp->CopyCompleteValue(StructAddress, &outDataTableRowHandle);
						return true;
					}
				}
			}
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;

}

/*set DataTableRef by Variable Name*/
bool UVariableAccessLibSetters::SetDataTableRefByName(UObject* Target, UObject* WorldContextObject, FName VarName, UDataTable* NewValue, UDataTable*& outDataTable)
{
	if (Target)
	{
		UObject* FoundObject;
		FObjectProperty* ObjectProp = FindFProperty<FObjectProperty>(Target->GetClass(), VarName);
		if (ObjectProp)
		{
			ObjectProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			FoundObject = ObjectProp->GetPropertyValue_InContainer(Target);
			outDataTable = Cast<UDataTable>(FoundObject);
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
	
	
}

/*set GameplayTagContainer by Variable Name*/
bool UVariableAccessLibSetters::SetGameplayTagContainerByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTagContainer NewValue, FGameplayTagContainer& outTagContainer)
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
						// Set Vector to value
						outTagContainer = NewValue;
						StructProp->CopyCompleteValue(StructAddress, &outTagContainer);
						return true;
					}
				}
			}
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*set GameplayTag by Variable Name*/
bool UVariableAccessLibSetters::SetGameplayTagByName(UObject* Target, UObject* WorldContextObject, FName VarName, FGameplayTag NewValue, FGameplayTag& outTag)
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
						// Set Vector to value
						outTag = NewValue;
						StructProp->CopyCompleteValue(StructAddress, &outTag);
						return true;
					}
				}
			}
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}
