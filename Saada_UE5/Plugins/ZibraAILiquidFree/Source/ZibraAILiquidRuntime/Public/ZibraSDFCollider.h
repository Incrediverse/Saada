// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "ZibraManipulator.h"
#include "CoreMinimal.h"
#include "ZibraSDFCollider.generated.h"

UENUM()
enum ESDFType
{
	ESDFType_Sphere UMETA(DisplayName = "Sphere"),
	ESDFType_Box UMETA(DisplayName = "Box"),
	ESDFType_Capsule UMETA(DisplayName = "Capsule"),
	ESDFType_Torus UMETA(DisplayName = "Torus"),
	ESDFType_Cylinder UMETA(DisplayName = "Cylinder")
};

UCLASS(abstract, ClassGroup = "ZibraAI", meta = (BlueprintSpawnableComponent, DisplayName = "SDF Collider"), hideCategories = (Rendering, Replication, Input, Actor, Base, Collision, Shape, OverlapCapsule, Physics, Cooking))
class ZIBRAAILIQUIDRUNTIME_API UZibraSDFCollider : public UZibraManipulator
{
	GENERATED_BODY()
public:
	UZibraSDFCollider();

	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

	static TArray<UZibraSDFCollider*> AllColliders;

	UPROPERTY(EditAnywhere, NonTransactional, meta = (ClampMin = "0.0", ClampMax = "1.0", Tooltip = "0.0 fluid flows without friction, 1.0 fluid sticks to the surface (0 is hydrophobic, 1 is hydrophilic)", Category = "Zibra Liquid Manipulator"))
	float FluidFriction = 0.0;

	//UPROPERTY(EditAnywhere, NonTransactional, meta = (DisplayName = "Force Interaction", Tooltip = "Allows the fluid to apply force to the object", Category = "Zibra Liquid Manipulator"))
	bool bForceInteraction = false;

	UPROPERTY(EditAnywhere, NonTransactional, meta = (DisplayName = "Invert SDF", Tooltip = "Inverts collider so liquid can only exist inside", Category = "Zibra Liquid Manipulator"))
	bool bInvertSDF = false;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName = "SDF Type", Category = "Zibra Liquid Manipulator")
	TEnumAsByte<ESDFType> chosenSDFType;

	UPROPERTY()
	FMatrix ObjectTransform;


	virtual ULONG GetMemoryFootrpint();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetPrimitiveComponent(UPrimitiveComponent* NewPrimitiveComponent);
	FActorComponentActivatedSignature OnComponentActivated;
	FActorComponentDeactivateSignature OnComponentDeactivated;

protected:
	int32 ColliderIndex = 0;
	FColliderParams ColliderParams;	
	void* NativeDataPtr;
	UPrimitiveComponent* PrimitiveComponent;

	int NumberOfNotifications = 0;
	
};


