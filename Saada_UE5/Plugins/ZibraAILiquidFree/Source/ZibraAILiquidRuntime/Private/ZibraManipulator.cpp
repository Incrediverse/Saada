// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraManipulator.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "ZibraLiquid.h"

TArray<UZibraManipulator*> UZibraManipulator::AllManipulators = TArray<UZibraManipulator*>();

UZibraManipulator::UZibraManipulator()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
	bTickInEditor = false;
}

void UZibraManipulator::InitializeConstData()
{
}

void UZibraManipulator::Init()
{
	if (!AllManipulators.Contains(this))
	{
		AllManipulators.Add(this);
	}
}

UZibraManipulator::~UZibraManipulator()
{
}

void UZibraManipulator::BeginPlay()
{
	Super::BeginPlay();
	Init();
	AActor* Owner = GetOwner();
	ParentActor = GetOwner();
	PreviousTransform = GetTransform();
	FTransform Transform = Owner->GetTransform();
}

void UZibraManipulator::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	AllManipulators.Remove(this);

	for (TObjectIterator<AZibraLiquid> Itr; Itr; ++Itr)
	{
		Itr->RemoveDestroyedManipulator(this);
	}
}

void UZibraManipulator::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	PreviousTransform = GetTransform();
}

FMatrix UZibraManipulator::GetTransform()
{
	return (FMatrix)GetComponentTransform().ToMatrixWithScale();
}

FMatrix UZibraManipulator::GetInverseTransform()
{
	return (FMatrix)GetComponentTransform().ToInverseMatrixWithScale();
}

FVector4 UZibraManipulator::GetRotation()
{
	FQuat rotation = GetComponentQuat();
	return FVector4(rotation.X, rotation.Y, rotation.Z, rotation.W);
}

FVector UZibraManipulator::GetPosition()
{
	return GetComponentLocation();
}

FVector UZibraManipulator::GetScale()
{
	return GetComponentScale();
}
