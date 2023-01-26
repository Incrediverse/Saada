// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraAnalyticSDFCollider.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#include "Editor.h"
#endif

UZibraAnalyticSDFCollider::UZibraAnalyticSDFCollider()
{
	ManipType = FManipulatorType::AnalyticCollider;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
	bTickInEditor = true;
	SetRelativeScale3D(FVector(100, 100, 100));

#if WITH_EDITOR
	SpriteComponent = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("Sprite"));
	if (SpriteComponent)
	{
		static ConstructorHelpers::FObjectFinder<UTexture2D> IconTexture(TEXT("Texture2D'/ZibraAILiquid/ZibraLiquidContent/Icons/AnalyticalCollider.AnalyticalCollider'"));
		SpriteComponent->SetSprite(IconTexture.Object);
		SpriteComponent->SetupAttachment(this);
		SpriteComponent->SetRelativeLocation(FVector(0, 0, 0));
		SpriteComponent->SetRelativeScale3D_Direct(FVector(0.5f, 0.5f, 0.5f));
	}
#endif
}

void UZibraAnalyticSDFCollider::BeginPlay()
{
	Super::BeginPlay();
	if (!PrimitiveComponent)
	{
		PrimitiveComponent = GetOwner()->FindComponentByClass<UPrimitiveComponent>();
	}
	AdditionalData.X = (int)chosenSDFType;
}


void UZibraAnalyticSDFCollider::OnComponentCreated()
{
	Super::OnComponentCreated();

	SetRelativeLocation(FVector(0, 0, 0));
}



void UZibraAnalyticSDFCollider::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	chosenSDFType = chosenSDFTypeUI;

	AdditionalData.X = (float)chosenSDFType;
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if WITH_EDITOR
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
		{
			FVector ActorLocation = GetOwner()->GetActorLocation();
			ActorLocation.Z += FMath::Abs(GetScale().Z) + 30;
			SpriteComponent->SetWorldLocation(ActorLocation);
			FRotator Rotator = FRotator::ZeroRotator;
			FQuat Quat = FQuat::Identity;
			FMatrix Transform = (FMatrix)FTransform::Identity.ToMatrixWithScale();
			if (GetOwner())
			{
				Rotator = GetComponentRotation();
				Quat = Rotator.Quaternion();
				Transform = (FMatrix)GetOwner()->GetActorTransform().ToMatrixWithScale();
			}
			switch (chosenSDFType)
			{
			case ESDFType_Sphere:
				DrawDebugSphere(GEditor->GetEditorWorldContext().World(), GetPosition(), 0.5f * GetScale().Y, 12, FColor::Cyan, false, 0, 0U, 1);
				break;

			case ESDFType_Box:
				DrawDebugBox(GEditor->GetEditorWorldContext().World(), GetPosition(), GetScale() * .5, Quat, FColor::Cyan, false, 0, 0U, 1);
				break;

			case ESDFType_Capsule:
				DrawDebugCapsule(GEditor->GetEditorWorldContext().World(), GetPosition(), 0.5f * GetScale().Z, 0.5f * GetScale().Y, Quat, FColor::Cyan, false, 0, 0U, 1);
				break;

			case ESDFType_Torus:
				DrawDebug2DDonut(GEditor->GetEditorWorldContext().World(), Transform, 0.5f, 1, 64, FColor::Cyan, false, 0, 0U, 1);
				break;

			case ESDFType_Cylinder:
				DrawDebugCylinder(GEditor->GetEditorWorldContext().World(), GetPosition() - Rotator.RotateVector(FVector(0, GetScale().Y / 2, 0)), GetPosition() + Rotator.RotateVector(FVector(0, GetScale().Y / 2, 0)), GetScale().X / 2, 12, FColor::Cyan, false, 0, 0U, 1);
				break;

			default:
				break;
			}
		}
	}
#endif
}