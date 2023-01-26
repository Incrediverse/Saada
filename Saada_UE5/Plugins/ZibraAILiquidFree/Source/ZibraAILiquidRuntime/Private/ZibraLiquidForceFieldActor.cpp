// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraLiquidForceFieldActor.h"
#include "ZibraLiquidForceField.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AZibraLiquidForceFieldActor::AZibraLiquidForceFieldActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LiquidForceField = CreateDefaultSubobject<UZibraLiquidForceField>(TEXT("LiquidForceField"));

	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	UStaticMeshComponent* CubeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	CubeComponent->SetStaticMesh(CubeMesh.Object);
	CubeComponent->SetupAttachment(RootComponent);
	SetActorScale3D(FVector(1, 1, 1));
	LiquidForceField->SetupAttachment(CubeComponent);
	LiquidForceField->SetRelativeScale3D(FVector(100, 100, 100));
	LiquidForceField->SetRelativeLocation(FVector(0, 0, 0));
	CubeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CubeComponent->SetVisibility(false);
}