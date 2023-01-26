// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "ZibraLiquidTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZibraAILiquidRuntime.h"
#include "ZibraManipulator.generated.h"

UENUM()
enum FManipulatorType
{
	None,
	Emitter,
	Void,
	ForceField,
	AnalyticCollider,
	ZibraNeuralCollider,
	Detector,
	Portal,
	TypeNum
};

UCLASS()
class ZIBRAAILIQUIDRUNTIME_API UZibraManipulator : public USceneComponent
{
	GENERATED_BODY()
public:
	UZibraManipulator();
	~UZibraManipulator();

	virtual void BeginPlay() override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Initialize() {};

	UPROPERTY(NonTransactional)
	TArray<float> ConstAdditionalData;

	FVector4 AdditionalData;

	FManipulatorType ManipType;

	static TArray<UZibraManipulator*> AllManipulators;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Is Enabled", Category = "Zibra Liquid Manipulator"))
	bool bIsEnabled = true;

	bool bIsInitialized = false;

	AActor* ParentActor;

	FMatrix PreviousTransform;

	virtual void InitializeConstData();;

	void Init();

	FMatrix GetTransform();
	FMatrix GetInverseTransform();
	FVector4 GetRotation();
	FVector GetPosition();
	FVector GetScale();
};


