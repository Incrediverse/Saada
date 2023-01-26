// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "ZibraSDFCollider.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZibraAnalyticSDFCollider.generated.h"

UCLASS(Blueprintable, ClassGroup = "ZibraAI", meta = (BlueprintSpawnableComponent, DisplayName = "Zibra Analytic SDF Collider"), hideCategories = (Rendering, Replication, Input, Actor, Base, Collision, Shape, OverlapCapsule, Cooking))
class ZIBRAAILIQUIDRUNTIME_API UZibraAnalyticSDFCollider : public UZibraSDFCollider
{
	GENERATED_BODY()
public:
	UZibraAnalyticSDFCollider();

	void BeginPlay() override;

	void OnComponentCreated() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "SDF Type", Category = "Zibra Liquid Manipulator")
	TEnumAsByte<ESDFType> chosenSDFTypeUI;
};


