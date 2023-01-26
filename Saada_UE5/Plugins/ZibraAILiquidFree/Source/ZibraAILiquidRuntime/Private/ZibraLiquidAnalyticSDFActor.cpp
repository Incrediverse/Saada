// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraLiquidAnalyticSDFActor.h"
#include "ZibraAnalyticSDFCollider.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

AZibraLiquidAnalyticSDFActor::AZibraLiquidAnalyticSDFActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ZibraAnalyticSDFCollider = CreateDefaultSubobject<UZibraAnalyticSDFCollider>(TEXT("ZibraAnalyticSDFCollider"));

	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	UStaticMeshComponent* CubeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeMeshComponent"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	CubeComponent->SetStaticMesh(CubeMesh.Object);
	CubeComponent->SetupAttachment(RootComponent);
	ZibraAnalyticSDFCollider->SetupAttachment(CubeComponent);
	SetActorScale3D(FVector(1, 1, 1));
	ZibraAnalyticSDFCollider->SetRelativeScale3D(FVector(100, 100, 100));
	CubeComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CubeComponent->SetVisibility(false);
	CubeComponent->SetSimulatePhysics(true);
	ZibraAnalyticSDFCollider->SetPrimitiveComponent(CubeComponent);
}