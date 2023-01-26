// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "ZibraManipulator.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZibraLiquidEmitter.generated.h"

UCLASS(Blueprintable, ClassGroup = "ZibraAI", meta = (BlueprintSpawnableComponent, DisplayName = "Zibra Emitter"))//, hideCategories = (Rendering, Replication, Input, Actor, Base, Collision, Shape, OverlapCapsule, Physics, Cooking))
class ZIBRAAILIQUIDRUNTIME_API UZibraLiquidEmitter  : public UZibraManipulator
{
	GENERATED_BODY()
public:
	UZibraLiquidEmitter();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void OnComponentCreated() override;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SkipSerialization, meta = (DisplayName = "Total amount of created particles", ClampMin = "0.0", Tooltip = "Emitted particles total", Category = "Zibra Liquid Manipulator"))
	int64 CreatedParticlesTotal = 0;
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SkipSerialization, meta = (DisplayName = "Amount of created particles per frame", ClampMin = "0.0", Tooltip = "Emitted particles last frame", Category = "Zibra Liquid Manipulator"))
	int32 CreatedParticlesPerFrame = 0;

	UPROPERTY(EditAnywhere, NonTransactional, meta = (ClampMin = "0.0", Tooltip = "Emitted volume per simulation time unit", Category = "Zibra Liquid Manipulator"))
	float VolumePerSimTime = 1.25f;

	UPROPERTY(EditAnywhere, NonTransactional, meta = (Tooltip = "The velocity of the created liquid", Category = "Zibra Liquid Manipulator"))
	FVector InitialVelocity = FVector(0, 0, 0);


	//API
	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Emitter")
	void Enable();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Emitter")
	void Disable();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Emitter")
	void SetInitialVelocity(FVector NewValue);

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Emitter")
	FVector GetInitialVelocity();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Emitter")
	int64 GetCreatedParticlesTotal();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Emitter")
	int32 GetCreatedParticlesPerFrame();
	//end API
};
