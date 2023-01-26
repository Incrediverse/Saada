// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#include "ZibraManipulatorManager.h"
#include "ZibraLiquidEmitter.h"
#include "ZibraSDFCollider.h"
#include "ZibraAnalyticSDFCollider.h"
#include "Kismet/KismetMathLibrary.h"
#include "ZibraLiquidTypes.h"

UZibraManipulatorManager::UZibraManipulatorManager()
{
	ManipulatorParams = TArray<FManipulatorParams>();
	ConstAdditionalData = TArray<float>();
	ConstDataID = TArray<int32>();
	TypeIndex.Init(0, FManipulatorType::TypeNum + 1);
	ZibraMatrix mat;
}

FVector UZibraManipulatorManager::VectorClamp(FVector X, FVector Min, FVector Max)
{
	return X.ComponentMin(Max).ComponentMax(Min);
}

void UZibraManipulatorManager::UpdateDynamic(AZibraLiquid* Liquid, float DeltaTime)
{

	FVector ContainerPos = Liquid->GetContainerPosition();
	FVector ContainerSize = Liquid->GetContainerSize();

	int ID = 0;
	ManipulatorParams.Empty();

	// fill arrays
	for(UZibraManipulator* manipulator : Manipulators)
	{
		if (!manipulator)
		{
			FManipulatorParams manip;
			memset(&manip, 0, sizeof(manip));
			// Not really nececcary, since we just memset'ed it with 0
			// But for clarity, it's important that Enabled is 0
			manip.Enabled = 0;
			ManipulatorParams.Add(manip);
			continue;
		}

		manipulator->Initialize();

		FManipulatorParams manip;
		
#if ENGINE_MAJOR_VERSION == 4
		manip.Transform = manipulator->GetTransform();
		manip.Rotation = manipulator->GetRotation();
		manip.Scale = manipulator->GetScale();
		manip.Position = manipulator->GetPosition();
		manip.AdditionalData = manipulator->AdditionalData;
		manip.PreviousTransform = manipulator->PreviousTransform;
		manip.ToObjectSpace = manipulator->GetInverseTransform();
#elif ENGINE_MAJOR_VERSION == 5
		manip.Transform = (FMatrix44f)manipulator->GetTransform();
		manip.Rotation = (FVector4f)manipulator->GetRotation();
		manip.Scale = (FVector3f)manipulator->GetScale();
		manip.Position = (FVector3f)manipulator->GetPosition();
		manip.AdditionalData = (FVector4f)manipulator->AdditionalData;
		manip.PreviousTransform = (FMatrix44f)manipulator->PreviousTransform;
		manip.ToObjectSpace = (FMatrix44f)manipulator->GetInverseTransform();
#endif
		manip.ConstDataBegin = FMath::Max(ConstDataID[ID], 0);
		manip.Type = (int)manipulator->ManipType;
		manip.Enabled = manipulator->bIsEnabled ? 1 : 0;
		

		if (manipulator && manipulator->IsA(UZibraLiquidEmitter::StaticClass()))
		{
			UZibraLiquidEmitter* emitter = dynamic_cast<UZibraLiquidEmitter*> (manipulator);
			
			// clamp the emitter to the fluid container
#if ENGINE_MAJOR_VERSION == 4
			FVector manipMin = manip.Position - manip.Scale * 0.5f;
			FVector manipMax = manip.Position + manip.Scale * 0.5f;
#elif ENGINE_MAJOR_VERSION == 5
			FVector3f manipMin = manip.Position - manip.Scale * 0.5;
			FVector3f manipMax = manip.Position + manip.Scale * 0.5;
#endif
			FVector containerMin = ContainerPos - ContainerSize * 0.5;
			FVector containerMax = ContainerPos + ContainerSize * 0.5;

#if ENGINE_MAJOR_VERSION == 4
			manipMin =
				VectorClamp(manipMin, containerMin, containerMin.ComponentMax(containerMax - manip.Scale));
			manipMax =
				VectorClamp(manipMax, containerMax.ComponentMin(containerMin + manip.Scale), containerMax);
#elif ENGINE_MAJOR_VERSION == 5
			manipMin =
				(FVector3f)VectorClamp((FVector)manipMin, containerMin, containerMin.ComponentMax(containerMax - (FVector)manip.Scale));
			manipMax =
				(FVector3f)VectorClamp((FVector)manipMax, containerMax.ComponentMin(containerMin + (FVector)manip.Scale), containerMax);
#endif

			manip.Scale = manipMax - manipMin;
			manip.Position = 0.5f * (manipMin + manipMax);
			
			float particlesPerSec = emitter->VolumePerSimTime / Liquid->GetCellSize() / Liquid->GetCellSize() /
				Liquid->GetCellSize() * Liquid->GetParticleDensity() *
				Liquid->GetSimulationSpeed();
			manip.AdditionalData.X = FMath::Floor(particlesPerSec * DeltaTime * 10000/*Compensate scaling in UE*/);
		}

		ManipulatorParams.Add(manip);
		ID++;
	}
	// ManipulatorParams.ToArray();
	Elements = Manipulators.Num();
}

float UZibraManipulatorManager::INT2Float(int32 a)
{
	const float MAX_INT = 2147483647.0f;
	const float F2I_MAX_VALUE = 5000.0f;
	const float F2I_SCALE = (MAX_INT / F2I_MAX_VALUE);

	return a / F2I_SCALE;
}

int32 UZibraManipulatorManager::GetStatIndex(int32 id, int32 offset)
{
	return id * STATISTICS_PER_MANIPULATOR + offset;
}


void UZibraManipulatorManager::UpdateConst(TArray<UZibraManipulator*> CurrentManipulators, TArray<UZibraSDFCollider*> sdfObjects)
{
	Manipulators = CurrentManipulators;

	// add all colliders to the manipulator list
	for(UZibraSDFCollider* collider : sdfObjects)
	{
		if (collider == nullptr)
			continue;

		if (!Manipulators.Contains(collider))
		{
			Manipulators.Add(collider);
		}
	}

	// first sort the manipulators
	Manipulators.Sort([](const UZibraManipulator& A, const UZibraManipulator& B)
		{
			return A.ManipType < B.ManipType;
		});

	// compute prefix sum
	for (int i = 0; i < (int)FManipulatorType::TypeNum; i++)
	{
		int id = 0;
		for(UZibraManipulator* manipulator : Manipulators)
		{
			if ((int)manipulator->ManipType >= i)
			{
				TypeIndex[i] = id;
				break;
			}
			id++;
		}

		if (id == Manipulators.Num())
		{
			TypeIndex[i] = Manipulators.Num();
		}
	}

	// set last as the total number of manipulators
	TypeIndex[(int)FManipulatorType::TypeNum] = Manipulators.Num();

	ManipulatorIndices.EmitterIndex = TypeIndex[(int)FManipulatorType::Emitter];
	ManipulatorIndices.EmitterIndexEnd = TypeIndex[(int)FManipulatorType::Emitter + 1];
	ManipulatorIndices.VoidIndex = TypeIndex[(int)FManipulatorType::Void];
	ManipulatorIndices.VoidIndexEnd = TypeIndex[(int)FManipulatorType::Void + 1];
	ManipulatorIndices.ForceFieldIndex = TypeIndex[(int)FManipulatorType::ForceField];
	ManipulatorIndices.ForceFieldIndexEnd = TypeIndex[(int)FManipulatorType::ForceField + 1];
	ManipulatorIndices.AnalyticColliderIndex = TypeIndex[(int)FManipulatorType::AnalyticCollider];
	ManipulatorIndices.AnalyticColliderIndexEnd = TypeIndex[(int)FManipulatorType::AnalyticCollider + 1];
	ManipulatorIndices.ZibraNeuralColliderIndex = TypeIndex[(int)FManipulatorType::ZibraNeuralCollider];
	ManipulatorIndices.ZibraNeuralColliderIndexEnd = TypeIndex[(int)FManipulatorType::ZibraNeuralCollider + 1];
	ZibraNeuralColliders = ManipulatorIndices.ZibraNeuralColliderIndexEnd - ManipulatorIndices.ZibraNeuralColliderIndex;
	ManipulatorIndices.DetectorIndex = TypeIndex[(int)FManipulatorType::Detector];
	ManipulatorIndices.DetectorIndexEnd = TypeIndex[(int)FManipulatorType::Detector + 1];
	ManipulatorIndices.PortalIndex = TypeIndex[(int)FManipulatorType::Portal];
	ManipulatorIndices.PortalIndexEnd = TypeIndex[(int)FManipulatorType::Portal + 1];

	if (ConstDataID.Num() != 0)
	{
		ConstAdditionalData.Empty();
		Embeddings.Empty();
		SDFGrid.Empty();
		ConstDataID.Empty();
	}

	for(UZibraManipulator* manipulator : Manipulators)
	{
		if (manipulator == nullptr)
			continue;

		manipulator->InitializeConstData();
		int ID = ConstAdditionalData.Num();
		ConstDataID.Add(ID);
		ConstAdditionalData.Insert(manipulator->ConstAdditionalData, ID);

	}
}

void UZibraManipulatorManager::RemoveManipulator(UZibraManipulator* Manipulator)
{
	if (Manipulators.Contains(Manipulator))
	{
		int32 index = Manipulators.Find(Manipulator);
		Manipulators[index] = nullptr;
	}
}

void UZibraManipulatorManager::RemoveCollider(UZibraSDFCollider* Collider)
{
	if (Manipulators.Contains(Collider))
	{
		int32 index = Manipulators.Find(Collider);
		Manipulators[index] = nullptr;
	}
}
