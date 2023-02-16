// Copyright © Pixeltale Motion 2021. All rights reserved.


#include "VariableAccessLibGeneral.h"
#include "VariableAccessLib.h"
#include "Misc/OutputDeviceNull.h"
#include "VariableAccessLogging.h"



UVariableAccessLibGeneral::UVariableAccessLibGeneral(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}



/*Add Value to Float by name*/
bool UVariableAccessLibGeneral::AddFloatToFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, float AddValue, float & outFloat)
{
	if (Target)
	{
		float FoundFloat;
		FFloatProperty* FloatProp = FindFProperty<FFloatProperty>(Target->GetClass(), VarName);
		if (FloatProp)
		{
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
			float NewValue = FoundFloat + AddValue;
			FloatProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			outFloat = FoundFloat;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Add Value to Int by name*/
bool UVariableAccessLibGeneral::AddIntToIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int32 AddValue, int32 & outInt)
{
	if (Target)
	{
		int32 FoundInt;
		FIntProperty* IntProp = FindFProperty<FIntProperty>(Target->GetClass(), VarName);
		if (IntProp)
		{
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			int32 NewValue = FoundInt + AddValue;
			IntProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			outInt = FoundInt;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Add Value to a clamped float by name*/
bool UVariableAccessLibGeneral::AddFloatToClampedFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, float AddValue, float ClampMin, float ClampMax, float & outFloat)
{
	if (Target)
	{
		float FoundFloat;
		float NewValue;
		FFloatProperty* FloatProp = FindFProperty<FFloatProperty>(Target->GetClass(), VarName);
		if (FloatProp)
		{
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
			NewValue = FMath::Clamp((FoundFloat + AddValue), ClampMin, ClampMax);
			FloatProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			outFloat = FoundFloat;
			return true;
		}

	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Add Value to a clamped Int by name*/
bool UVariableAccessLibGeneral::AddIntToClampedIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int AddValue, int ClampMin, int ClampMax, int & outInt)
{
	if (Target)
	{
		int FoundInt;
		int NewValue;
		FIntProperty* IntProp = FindFProperty<FIntProperty>(Target->GetClass(), VarName);
		if (IntProp)
		{
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			NewValue = FMath::Clamp((FoundInt + AddValue), ClampMin, ClampMax);
			IntProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			outInt = FoundInt;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Multiply Value with Float by name*/
bool UVariableAccessLibGeneral::MultiplyFloatWithFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, float MultiplicationValue, float & outFloat)
{
	if (Target)
	{
		float FoundFloat;
		FFloatProperty* FloatProp = FindFProperty<FFloatProperty>(Target->GetClass(), VarName);
		if (FloatProp)
		{
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
			float NewValue = FoundFloat * MultiplicationValue;
			FloatProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			outFloat = FoundFloat;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Multiply Value with Int by name*/
bool UVariableAccessLibGeneral::MultiplyIntWithIntByName(UObject* Target, UObject* WorldContextObject, FName VarName, int32 MultiplicationValue, int32 & outInt)
{
	if (Target)
	{
		int32 FoundInt;
		FIntProperty* IntProp = FindFProperty<FIntProperty>(Target->GetClass(), VarName);
		if (IntProp)
		{
			FoundInt = IntProp->GetPropertyValue_InContainer(Target);
			int32 NewValue = FoundInt * MultiplicationValue;
			IntProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			outInt = FoundInt;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}

/*Divide Value with Float by name*/
bool UVariableAccessLibGeneral::DivideFloatWithFloatByName(UObject* Target, UObject* WorldContextObject, FName VarName, float DivisionValue, float &outFloat)
{
	if (Target)
	{
		float FoundFloat;
		FFloatProperty* FloatProp = FindFProperty<FFloatProperty>(Target->GetClass(), VarName);
		if (FloatProp)
		{
			FoundFloat = FloatProp->GetPropertyValue_InContainer(Target);
			float NewValue = FoundFloat / DivisionValue;
			FloatProp->SetPropertyValue_InContainer(Target, NewValue); //this actually sets the variable
			outFloat = FoundFloat;
			return true;
		}
	}
	UVariableAccessLogging::PrintErrorMsg(VarName, Target, WorldContextObject);
	return false;
}


/*Multiply Float with Vector by name*/
bool UVariableAccessLibGeneral::ScaleVectorByName(UObject* Target, UObject* WorldContextObject, FName VarName, float ScaleValue, FVector &outVector)
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
						StructProp->CopyCompleteValue(&outVector, StructAddress);
						FVector NewValue = outVector * ScaleValue;
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





//Set LocOfTransByName
bool UVariableAccessLibGeneral::SetLocOfTransByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector NewValue, FTransform& outTransform)
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

						outTransform.SetLocation(NewValue);
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

//Set Rot Of Trans By Name
bool UVariableAccessLibGeneral::SetRotOfTransByName(UObject* Target, UObject* WorldContextObject, FName VarName, FRotator NewValue, FTransform& outTransform)
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
						FQuat QuatRot = NewValue.Quaternion();
						outTransform.SetRotation(QuatRot);
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

//Set Scale Of Trans By Name
bool UVariableAccessLibGeneral::SetScaleOfTransByName(UObject* Target, UObject* WorldContextObject, FName VarName, FVector NewValue, FTransform& outTransform)
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

						outTransform.SetScale3D(NewValue);
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
//Get All Variables of Object

bool UVariableAccessLibGeneral::GetAllVariableNames(UObject* Target, UObject* WorldContextObject, bool GetInheritedVars, TArray<FName>& VarNames)
{
	UClass* OwnerClass;
	UClass* Class = Target->GetClass();
	OwnerClass = Class;
	for (TFieldIterator<FProperty> Prop(Class); Prop; ++Prop)
	{
		FProperty* Property = *Prop;
		if (!CastField<FMulticastDelegateProperty>(Property) && !CastField<FDelegateProperty>(Property) && !CastField<FInterfaceProperty>(Property))

		{
			UClass* OuterClass = Property->GetOwnerChecked<UClass>();
			if (GetInheritedVars)
			{
				OwnerClass = Class->GetSuperClass();
			}
			if (OuterClass == OwnerClass && !Property->GetName().Contains("UberGraph"))

			{
				VarNames.Add(*Property->GetName());
			}
		}


	}

	return VarNames.IsValidIndex(0);
}

//Execute Function by Name

bool UVariableAccessLibGeneral::ExecFunctionByName(UObject* Target, UObject* WorldContextObject, FName FunctionName, const TArray<FString>& Parameters)
{
	if (!IsValid(Target))
		return false;

	FString Command = FunctionName.ToString();
	for (FString Param : Parameters)
		Command += (" " + Param);

	FOutputDeviceNull Ar;
	return Target->CallFunctionByNameWithArguments(*Command, Ar, nullptr, true);
}



//Get All Variables of Object

bool UVariableAccessLibGeneral::GetAllFunctionNames(UObject* Target, UObject* WorldContextObject, bool GetParentFunctions, TArray<FName>& FuncNames)
{
	EFieldIteratorFlags::SuperClassFlags Parent;
	if(GetParentFunctions)
	{	

		Parent = EFieldIteratorFlags::SuperClassFlags::IncludeSuper;
	}
	else
	{

		Parent = EFieldIteratorFlags::SuperClassFlags::ExcludeSuper;
	}

	for (TFieldIterator<UFunction> Func(Target->GetClass(), Parent); Func; ++Func)
	{
		UFunction* Function = *Func;
		FuncNames.Add(*Func->GetName());

	}

	return FuncNames.IsValidIndex(0);
}
/*

//Get Material Instance from Content

UMaterialInterface* UVariableAccessLibGeneral::FindMatInstanceFromContent(FString Dir, FString Name, FString& FullPath)
{

	FString CName= Name + "." + Name;
	FullPath = "/Game/"+ Dir + "/" + CName;
	const TCHAR* Path = *FullPath;
	UMaterialInterface* FoundMat= LoadObject<UMaterialInterface>(NULL, Path, NULL, LOAD_None, NULL);
	return FoundMat;
}

UStaticMesh* UVariableAccessLibGeneral::FindMeshInstanceFromContent(FString Dir, FString Name, FString& FullPath)
{

	FString CName = Name + "." + Name;
	FullPath = "/Game/" + Dir + "/" + CName;
	const TCHAR* Path = *FullPath;
	UStaticMesh* FoundMesh = LoadObject<UStaticMesh>(nullptr, Path, NULL, LOAD_None);
	return FoundMesh;
}

UTexture2D* UVariableAccessLibGeneral::FindTextureFromContent(FString Dir, FString Name, FString& FullPath)
{

	FString CName = Name + "." + Name;
	FullPath = "/Game/" + Dir + "/" + CName;
	const TCHAR* Path = *FullPath;
	UTexture2D* FoundMesh = LoadObject<UTexture2D>(NULL, Path, NULL, LOAD_None, NULL);
	return FoundMesh;
}
*/


// GetCPPType();SameType() -> Use to find variable by type!!! 