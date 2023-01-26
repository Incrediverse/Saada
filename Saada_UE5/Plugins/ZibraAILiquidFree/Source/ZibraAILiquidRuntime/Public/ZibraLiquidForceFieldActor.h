// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "ZibraLiquidForceFieldActor.generated.h"

class UZibraLiquidForceField;

UCLASS(ComponentWrapperClass)
class ZIBRAAILIQUIDRUNTIME_API AZibraLiquidForceFieldActor : public AActor
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY(Category=ZibraAI, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UZibraLiquidForceField* LiquidForceField;

public:
	FORCEINLINE UZibraLiquidForceField* GetLiquidForceFieldComponent() const { return LiquidForceField; }
};
