// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraLiquidForceField.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#include "Editor.h"
#endif

UZibraLiquidForceField::UZibraLiquidForceField()
{
	ManipType = FManipulatorType::ForceField;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
	bTickInEditor = true;

	SetRelativeScale3D(FVector(100, 100, 100));

#if WITH_EDITOR
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (SpriteComponent)
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexture(TEXT("Texture2D'/ZibraAILiquid/ZibraLiquidContent/Icons/ForceField.ForceField'"));
		SpriteComponent->SetSprite(IconTexture.Object);
		SpriteComponent->SetupAttachment(this);
		SpriteComponent->SetRelativeScale3D_Direct(FVector(0.5f, 0.5f, 0.5f));
		SetRelativeLocation(FVector(0, 0, 0));
		SpriteComponent->SetRelativeLocation(FVector(0, 0, 0));
	}
#endif
}

void UZibraLiquidForceField::BeginPlay()
{
	Super::BeginPlay();
}

void UZibraLiquidForceField::OnComponentCreated()
{
	Super::OnComponentCreated();

	SetRelativeLocation(FVector(0, 0, 0));
}

void UZibraLiquidForceField::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AdditionalData.X = (int)Type;
	AdditionalData.Y = Strength;
	AdditionalData.Z = DistanceDecay;
	AdditionalData.W = (int)Shape;

#if WITH_EDITOR

	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		FQuat Quat = FQuat::Identity;

		if (GetOwner())
		{
			Quat = GetOwner()->GetActorRotation().Quaternion();
		}

		FVector ActorLocation = GetOwner()->GetActorLocation();
		ActorLocation.Z += FMath::Abs(GetScale().Z) + 30;
		SpriteComponent->SetWorldLocation(ActorLocation);
		switch (Shape)
		{
			case EForceFieldShape::EForceFieldShape_Cube:
				DrawDebugBox(GEditor->GetEditorWorldContext().World(), GetPosition(), GetScale() * .5, Quat, FColor::White, false, 0, 0U, 2);
				break;
			case EForceFieldShape::EForceFieldShape_Sphere:
				DrawDebugSphere(GEditor->GetEditorWorldContext().World(), GetPosition(), 0.5f * GetScale().Y, 12, FColor::White, false, 0, 0U, 2);
				break;
			default:
				break;
		}

		if (FMath::Abs(Strength) < STRENGTH_DRAW_THRESHOLD)
			return;

		switch (Type)
		{
			case EForceFieldType_Radial:
				DrawArrowsSphereRadial(GetPosition(), Strength, 32, FColor::Blue);
				break;
			case EForceFieldType_Directional:
				DrawArrowsSphereDirectional(GetPosition(), FVector::RightVector * Strength, 32, FColor::Blue);
				break;
			case EForceFieldType_Swirl:
				DrawArrowsSphereTangent(GetPosition(), FVector::RightVector * Strength, 32, FColor::Blue);
				break;
			default:
				break;
		}
	}
#endif
}

void UZibraLiquidForceField::Enable()
{
	bIsEnabled = true;
}

void UZibraLiquidForceField::Disable()
{
	bIsEnabled = false;
}

void UZibraLiquidForceField::SetType(TEnumAsByte<EForceFieldType> NewType)
{
	Type = NewType;
}

EForceFieldType UZibraLiquidForceField::GetType()
{
	return Type;
}

void UZibraLiquidForceField::SetShape(TEnumAsByte<EForceFieldShape> NewValue)
{
	Shape = NewValue;
}

TEnumAsByte<EForceFieldShape> UZibraLiquidForceField::GetShape()
{
	return Shape;
}

float UZibraLiquidForceField::GetStrength()
{
	return Strength;
}

void UZibraLiquidForceField::SetStrength(float NewValue)
{
	Strength = NewValue;
}

float UZibraLiquidForceField::GetDistanceDecay()
{
	return DistanceDecay;
}

void UZibraLiquidForceField::SetDistanceDecay(float NewValue)
{
	DistanceDecay = NewValue;
}

void UZibraLiquidForceField::DrawArrowsSphereRadial(FVector origin, float length, int32 n, FColor color)
{
#if WITH_EDITOR
	FRotator Rotator = GetOwner()->GetActorRotation();

	for (int i = 0; i < n; i++)
	{
		FVector point = FibonacciSpherePoint(i, n);
		DrawDebugDirectionalArrow(GEditor->GetEditorWorldContext().World(), origin + Rotator.RotateVector(point + GetScale() * length * point), origin + Rotator.RotateVector(point + GetScale() * length * point + (-length * point) * GetScale() / 3), 10, color, false, 0, 0U, 1);
	}
#endif
}

void UZibraLiquidForceField::DrawArrowsSphereTangent(FVector origin, FVector axis, int32 n, FColor color)
{
#if WITH_EDITOR
	FRotator Rotator = GetOwner()->GetActorRotation();

	for (int i = 0; i < n; i++)
	{
		FVector point = FibonacciSpherePoint(i, n);
		FVector direction = FVector::CrossProduct(point, axis);
		DrawDebugDirectionalArrow(GEditor->GetEditorWorldContext().World(), origin + Rotator.RotateVector(point * GetScale() - 0.5f * direction), origin + Rotator.RotateVector(point * GetScale() - 0.5f * direction + (direction) * GetScale() / 4), GetScale().GetAbsMax() / 5, color, false, 0, 0U, 1);
	}
#endif
}

void UZibraLiquidForceField::DrawArrowsSphereDirectional(FVector origin, FVector direction, int32 n, FColor color)
{
#if WITH_EDITOR
	FRotator Rotator = GetOwner()->GetActorRotation();

	for (int32 i = 0; i < n; i++)
	{
		FVector point = FibonacciSpherePoint(i, n);
		DrawDebugDirectionalArrow(GEditor->GetEditorWorldContext().World(), origin + Rotator.RotateVector(point * GetScale() - 0.5f * direction), origin + (Rotator.RotateVector(point * GetScale() - 0.5f * direction + (direction) * GetScale() / 4)), GetScale().GetAbsMax() / 5, color, false, 0, 0U, 1);
	}
#endif
}

FVector UZibraLiquidForceField::FibonacciSpherePoint(int32 i, int32 n)
{
	FVector2D pt = FVector2D(2.0f * M_Pi * FMath::Fmod(i / Phi, 1.0f), FMath::Acos(1.0f - (2.0f * i + 1.0f) / (float)n));
	return FVector(FMath::Cos(pt.X) * FMath::Sin(pt.Y), FMath::Sin(pt.X) * FMath::Sin(pt.Y), FMath::Cos(pt.Y));
}
