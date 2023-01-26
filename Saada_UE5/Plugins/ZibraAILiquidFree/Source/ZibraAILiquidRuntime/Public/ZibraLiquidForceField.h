// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "ZibraManipulator.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZibraLiquidForceField.generated.h"

UENUM()
enum EForceFieldType
{
	EForceFieldType_Radial UMETA(DisplayName = "Radial"),
	EForceFieldType_Directional UMETA(DisplayName = "Directional"),
	EForceFieldType_Swirl UMETA(DisplayName = "Swirl")
};

UENUM()
enum EForceFieldShape
{
	EForceFieldShape_Sphere UMETA(DisplayName = "Sphere"),
	EForceFieldShape_Cube UMETA(DisplayName = "Cube")
};

UCLASS(Blueprintable, ClassGroup = "ZibraAI", meta = (BlueprintSpawnableComponent, DisplayName = "Zibra Force Field"), hideCategories = (Rendering, Replication, Input, Actor, Base, Collision, Shape, OverlapCapsule, Physics, Cooking))
class ZIBRAAILIQUIDRUNTIME_API UZibraLiquidForceField : public UZibraManipulator
{
	GENERATED_BODY()
public:
	UZibraLiquidForceField();

	void BeginPlay() override;
	void OnComponentCreated() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Zibra Liquid Manipulator")
	TEnumAsByte<EForceFieldType> Type = EForceFieldType::EForceFieldType_Radial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zibra Liquid Manipulator")
	TEnumAsByte<EForceFieldShape> Shape;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "-1.0", ClampMax = "1.0", Tooltip = "The strength of the force acting on the liquid", Category = "Zibra Liquid Manipulator"))
	float Strength = 1.0f;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "10.0", Tooltip = "How fast does the force lose its strenght with distance to the center", Category = "Zibra Liquid Manipulator"))
	float DistanceDecay = 1.0f;

	//API
	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Force Field")
	void Enable();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Force Field")
	void Disable();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Force Field")
	void SetType(TEnumAsByte<EForceFieldType> NewValue);

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Force Field")
	EForceFieldType GetType();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Force Field")
	void SetShape(TEnumAsByte<EForceFieldShape> NewValue);

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Force Field")
	TEnumAsByte<EForceFieldShape> GetShape();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Force Field")
	float GetStrength();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Force Field")
	void SetStrength(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Force Field")
	float GetDistanceDecay();

	UFUNCTION(BlueprintCallable, Category = "Zibra Liquid Force Field")
	void SetDistanceDecay(float NewValue);
	//end API

protected:
	void DrawArrowsSphereRadial(FVector origin, float length, int32 n, FColor color);
	void DrawArrowsSphereTangent(FVector origin, FVector axis, int32 n, FColor color);
	void DrawArrowsSphereDirectional(FVector origin, FVector direction, int32 n, FColor color);
	FVector FibonacciSpherePoint(int32 i, int32 n);

	const float STRENGTH_DRAW_THRESHOLD = 0.001f;
	const float M_Pi = 3.14159265358979323846;
	const float Phi = 0.5f * (FMath::Sqrt(5.0f) + 1.0f);
};


