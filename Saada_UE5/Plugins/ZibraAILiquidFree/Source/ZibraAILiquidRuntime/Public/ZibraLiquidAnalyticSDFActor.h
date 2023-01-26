// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "ZibraLiquidAnalyticSDFActor.generated.h"

class UZibraAnalyticSDFCollider;

UCLASS(ComponentWrapperClass)
class ZIBRAAILIQUIDRUNTIME_API AZibraLiquidAnalyticSDFActor : public AActor
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY(Category=ZibraAI, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UZibraAnalyticSDFCollider* ZibraAnalyticSDFCollider;

public:
	FORCEINLINE UZibraAnalyticSDFCollider* GetZibraAnalyticSDFColliderComponent() const { return ZibraAnalyticSDFCollider; }
};
