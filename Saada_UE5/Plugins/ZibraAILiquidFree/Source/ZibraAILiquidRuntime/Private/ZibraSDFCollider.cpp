// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraSDFCollider.h"
#include "ZibraLiquid.h"
#include "ZibraNotifications.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

TArray<UZibraSDFCollider*> UZibraSDFCollider::AllColliders;
UZibraSDFCollider::UZibraSDFCollider()
{
	SetRelativeScale3D(FVector(1, 1, 1));
}

void UZibraSDFCollider::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	AdditionalData.Z = bInvertSDF ? -1.0f : 1.0f;
	AdditionalData.W = FluidFriction;
	if (NumberOfNotifications > 0)
	{
		NumberOfNotifications--;
	}
	
}

ULONG UZibraSDFCollider::GetMemoryFootrpint()
{
	return 0;
}


void UZibraSDFCollider::SetPrimitiveComponent(UPrimitiveComponent* NewPrimitiveComponent)
{
	PrimitiveComponent = NewPrimitiveComponent;
}

void UZibraSDFCollider::BeginPlay()
{
	Super::BeginPlay();

	if (!AllColliders.Contains(this))
	{
		AllColliders.Add(this);
	}

	NumberOfNotifications = 0;
}

void UZibraSDFCollider::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (AllColliders.Contains(this))
	{
		AllColliders.Remove(this);
	}
}

void UZibraSDFCollider::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	AllManipulators.Remove(this);

	for (TObjectIterator<AZibraLiquid> Itr; Itr; ++Itr)
	{
		Itr->RemoveDestroyedCollider(this);
	}
}
