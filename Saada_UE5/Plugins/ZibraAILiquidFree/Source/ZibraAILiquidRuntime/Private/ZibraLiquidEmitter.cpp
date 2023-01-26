// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraLiquidEmitter.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#include "Editor.h"
#endif


UZibraLiquidEmitter::UZibraLiquidEmitter()
{
	ManipType = FManipulatorType::Emitter;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
	bTickInEditor = true;

	SetRelativeScale3D(FVector(100, 100, 100));


#if WITH_EDITOR
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (SpriteComponent)
	{	
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexture(TEXT("Texture2D'/ZibraAILiquid/ZibraLiquidContent/Icons/Emitter.Emitter'"));
		SpriteComponent->SetSprite(IconTexture.Object);
		SpriteComponent->SetupAttachment(this);
		SetRelativeLocation(FVector(0, 0, 0));
		SpriteComponent->SetRelativeLocation(FVector(0, 0, 0));
		SpriteComponent->SetRelativeScale3D_Direct(FVector(0.5f, 0.5f, 0.5f));
	}
#endif
}

void UZibraLiquidEmitter::OnComponentCreated()
{
	Super::OnComponentCreated();

	SetRelativeLocation(FVector(0, 0, 0));
}

void UZibraLiquidEmitter::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FVector rotatedVelocity = GetOwner()->GetActorRotation().Quaternion() * InitialVelocity;

	AdditionalData.Y = rotatedVelocity.X;
	AdditionalData.Z = rotatedVelocity.Y;
	AdditionalData.W = rotatedVelocity.Z;

#if WITH_EDITOR
	SpriteComponent->SetRelativeLocation(FVector(0, 0, (GetScale().Z + 30)) / (GetOwner()->GetActorScale().Z * 100));

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
		DrawDebugBox(GEditor->GetEditorWorldContext().World(), GetPosition(), GetScale() * .5, Quat, FColor::Blue, false, 0, 0U, 2);
		DrawDebugDirectionalArrow(GEditor->GetEditorWorldContext().World(), GetPosition(), GetPosition() + rotatedVelocity * 20, 12, FColor::Blue, false, 0, 0U, 2);
	}
#endif
}

void UZibraLiquidEmitter::Enable()
{
	bIsEnabled = true;
}

void UZibraLiquidEmitter::Disable()
{
	bIsEnabled = false;
}

//void UZibraLiquidEmitter::SetParticlesPerSecond(float NewValue)
//{
//	ParticlesPerSec = NewValue;
//}
//
//float UZibraLiquidEmitter::GetParticlePerSecond()
//{
//	return ParticlesPerSec;
//}

void UZibraLiquidEmitter::SetInitialVelocity(FVector NewValue)
{
	InitialVelocity = NewValue;
}

FVector UZibraLiquidEmitter::GetInitialVelocity()
{
	return InitialVelocity;
}

int64 UZibraLiquidEmitter::GetCreatedParticlesTotal()
{
	return CreatedParticlesTotal;
}

int32 UZibraLiquidEmitter::GetCreatedParticlesPerFrame()
{
	return CreatedParticlesPerFrame;
}
