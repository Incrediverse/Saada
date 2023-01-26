// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "ZibraManipulator.h"
#include "CoreMinimal.h"
#include "ZibraLiquidTypes.h"
#include "ZibraSDFCollider.h"
#include "ZibraManipulatorManager.generated.h"

class AZibraLiquid;

UCLASS(ClassGroup = "ZibraAI", meta = (BlueprintSpawnableComponent, DisplayName = "Zibra Manipulator Manager"))
class ZIBRAAILIQUIDRUNTIME_API UZibraManipulatorManager : public UObject
{
	GENERATED_BODY()

public:

	UZibraManipulatorManager();

	TArray<int32> TypeIndex;

	FManipulatorIndices ManipulatorIndices;
	int32  Elements = 0;
	int ZibraNeuralColliders = 0;

	const int32 STATISTICS_PER_MANIPULATOR = 8;

	TArray<FManipulatorParams> ManipulatorParams;

	TArray<float> ConstAdditionalData;
	TArray<int32> ConstDataID;

	UPROPERTY()
	TArray<uint8> SDFGrid;

	UPROPERTY()
	TArray<uint8> Embeddings;

	TArray<UZibraManipulator*> Manipulators;

	FVector VectorClamp(FVector X, FVector Min, FVector Max);

	void UpdateDynamic(AZibraLiquid* Liquid, float DeltaTime = 0.0f);

	float INT2Float(int32 a);

	int32 GetStatIndex(int32 id, int32 offset);

	void UpdateConst(TArray<UZibraManipulator*> CurrentManipulators, TArray<UZibraSDFCollider*> sdfObjects);

	void RemoveManipulator(UZibraManipulator* Manipulator);
	void RemoveCollider(UZibraSDFCollider* Collider);
};



