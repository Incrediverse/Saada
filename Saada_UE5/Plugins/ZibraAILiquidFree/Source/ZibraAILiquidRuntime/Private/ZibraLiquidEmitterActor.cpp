// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraLiquidEmitterActor.h"
#include "ZibraLiquidEmitter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AZibraLiquidEmitterActor::AZibraLiquidEmitterActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LiquidEmitter = CreateDefaultSubobject<UZibraLiquidEmitter>(TEXT("LiquidEmitter"));

	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
	MeshComponent->SetStaticMesh(CubeMesh.Object);
	MeshComponent->SetupAttachment(RootComponent);
	SetActorScale3D(FVector(1, 1, 1));
	LiquidEmitter->SetupAttachment(MeshComponent);
	LiquidEmitter->SetRelativeScale3D(FVector(100, 100, 100));
	LiquidEmitter->SetRelativeLocation(FVector(0, 0, 0));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetVisibility(false);
}