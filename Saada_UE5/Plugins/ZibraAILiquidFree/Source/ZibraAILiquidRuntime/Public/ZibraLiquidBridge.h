// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

namespace ZibraLiquidDLL
{
    // Declare methods to store the DLL methods
    typedef void (*CreateLiquidInstance)(int);
    typedef bool (*IsPaidVersion)(void);
    typedef void (*RegisterParticlesBuffers)(int, const void*);
    typedef void (*RegisterRenderResources)(int, const void*);
	typedef void (*RegisterSolverBuffers)(int, const void*);
    typedef void (*UnrealPluginLoad)(void*, int);
	typedef void (*Draw)(int);
	typedef void (*ClearSDFAndIDGrid)(int);
	typedef void (*StepPhysics)(int);
	typedef void (*UpdateReadback)(int);
	typedef void (*UpdateLiquidParameters)(int, const void*);
	typedef void (*SetCameraParameters)(int, const void*);
	typedef void (*SetRenderParameters)(int, const void*);
	typedef void (*RegisterManipulators)(int, const void*);
    typedef void (*InitializeGpuReadback)(int, const void*);
	typedef void (*SaveAPIState)(int);
	typedef void (*RestoreAPIState)(int);
    typedef void (*ReleaseResources)(int);
    typedef void (*UpdateManipulatorParameters)(int, const void*);    
    typedef void* (*GPUReadbackGetData)(int, uint32);
	typedef void (*SetDebugLogWrapperPointer)(void*); 
	typedef void (*UpdateMeshRenderGlobalParameters)(int, const void*);
	typedef void (*InitializeGraphicsPipeline)(int);
	typedef void* (*GetCurrentAffineBufferIndex)(int);
	typedef int32 (*GarbageCollect)();

}

namespace ZibraLiquidBridge
{
    ZibraLiquidDLL::CreateLiquidInstance CreateLiquidInstance = NULL;
    ZibraLiquidDLL::IsPaidVersion IsPaidVersion = NULL;
    ZibraLiquidDLL::RegisterParticlesBuffers RegisterParticlesBuffers = NULL;
    ZibraLiquidDLL::RegisterRenderResources RegisterRenderResources = NULL;
	ZibraLiquidDLL::RegisterManipulators RegisterManipulators = NULL;
	ZibraLiquidDLL::SetRenderParameters SetRenderParameters = NULL;
    ZibraLiquidDLL::RegisterSolverBuffers RegisterSolverBuffers = NULL;
    ZibraLiquidDLL::UnrealPluginLoad UnrealPluginLoad = NULL;
    ZibraLiquidDLL::Draw Draw = NULL;
    ZibraLiquidDLL::ClearSDFAndIDGrid ClearSDFAndIDGrid = NULL;
	ZibraLiquidDLL::StepPhysics StepPhysics = NULL;
	ZibraLiquidDLL::UpdateReadback UpdateReadback = NULL;
    ZibraLiquidDLL::UpdateLiquidParameters UpdateLiquidParameters = NULL;
	ZibraLiquidDLL::SetCameraParameters SetCameraParameters = NULL;
	ZibraLiquidDLL::SaveAPIState SaveAPIState = NULL;
	ZibraLiquidDLL::RestoreAPIState RestoreAPIState = NULL;
    ZibraLiquidDLL::ReleaseResources ReleaseResources = NULL;
    ZibraLiquidDLL::InitializeGpuReadback InitializeGpuReadback = NULL;
    ZibraLiquidDLL::UpdateManipulatorParameters UpdateManipulatorParameters = NULL;
    ZibraLiquidDLL::GPUReadbackGetData GPUReadbackGetData = NULL;
	ZibraLiquidDLL::UpdateMeshRenderGlobalParameters UpdateMeshRenderGlobalParameters = NULL; 
	ZibraLiquidDLL::SetDebugLogWrapperPointer SetDebugLogWrapperPointer = NULL;
	ZibraLiquidDLL::InitializeGraphicsPipeline InitializeGraphicsPipeline = NULL;
	ZibraLiquidDLL::GetCurrentAffineBufferIndex GetCurrentAffineBufferIndex = NULL;
	ZibraLiquidDLL::GarbageCollect GarbageCollect = NULL;
}

