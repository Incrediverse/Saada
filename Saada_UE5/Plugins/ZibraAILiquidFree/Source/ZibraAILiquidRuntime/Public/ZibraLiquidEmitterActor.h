// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Actor.h"
#include "ZibraLiquidEmitterActor.generated.h"

class UZibraLiquidEmitter;

UCLASS(ComponentWrapperClass)
class ZIBRAAILIQUIDRUNTIME_API AZibraLiquidEmitterActor : public AActor
{
	GENERATED_UCLASS_BODY()

private:
	UPROPERTY(Category=ZibraAI, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	UZibraLiquidEmitter* LiquidEmitter;

public:
	FORCEINLINE UZibraLiquidEmitter* GetLiquidEmitterComponent() const { return LiquidEmitter; }
};
