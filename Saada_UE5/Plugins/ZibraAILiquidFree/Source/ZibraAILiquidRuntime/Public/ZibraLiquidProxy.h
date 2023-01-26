// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "ZibraLiquidTypes.h"
#include "RHIResources.h"
#include "Engine/TextureRenderTarget.h"
#include "Engine/TextureRenderTargetVolume.h"
#include "ZibraTexture2DDynamic.h"

struct ZibraLiquidProxy
{
#if ENGINE_MAJOR_VERSION == 4
	using ZibraStructuredBufferRHIRef = FStructuredBufferRHIRef;
    using ZibraIndexBufferRHIRef = FIndexBufferRHIRef;
	using ZibraVertexBufferRHIRef = FVertexBufferRHIRef;
#elif ENGINE_MAJOR_VERSION == 5
	using ZibraStructuredBufferRHIRef = FBufferRHIRef;
	using ZibraIndexBufferRHIRef = FBufferRHIRef;
	using ZibraVertexBufferRHIRef = FBufferRHIRef;
#endif

	int InstanceID;

	ZibraStructuredBufferRHIRef AtomicGridRHI;
	ZibraStructuredBufferRHIRef JFAGrid0RHI;
	ZibraStructuredBufferRHIRef JFAGrid1RHI;
	ZibraStructuredBufferRHIRef CountersRHI;
	ZibraStructuredBufferRHIRef VertexIDGridRHI;
	ZibraVertexBufferRHIRef VertexBuffer0RHI;
	ZibraVertexBufferRHIRef VertexBuffer1RHI;
	ZibraStructuredBufferRHIRef QuadBufferRHI;
	ZibraStructuredBufferRHIRef TransferDataBufferRHI;
	ZibraIndexBufferRHIRef MeshRenderIndexBufferRHI;
	ZibraVertexBufferRHIRef VertexPropertiesRHI;
	FTexture3DRHIRef GridNormalTextureRHI;
	FTexture3DRHIRef DensityTextureRHI;
	FTextureBridge GridNormalTextureBridge;
	FTextureBridge DensityTextureBridge;

	ZibraStructuredBufferRHIRef PositionMassRHI;
	ZibraStructuredBufferRHIRef PositionRadiusRHI; // in world space
	ZibraStructuredBufferRHIRef Affine0RHI;
	ZibraStructuredBufferRHIRef Affine1RHI;
	ZibraStructuredBufferRHIRef ParticleNumberRHI;

	ZibraStructuredBufferRHIRef GridDataRHI;
	ZibraStructuredBufferRHIRef IndexGridRHI;
	ZibraStructuredBufferRHIRef GridBlur0RHI;
	ZibraStructuredBufferRHIRef GridBlur1RHI;
	ZibraStructuredBufferRHIRef GridNormalRHI;
	ZibraStructuredBufferRHIRef GridSDFRHI;
	ZibraStructuredBufferRHIRef SurfaceGridTypeRHI;

	FTexture3DRHIRef SDFGridTextureRHI;
	FTexture3DRHIRef EmbeddingsTextureRHI;

	FTextureBridge SDFGridTextureBridge;
	FTextureBridge EmbeddingsTextureBridge;

	ZibraStructuredBufferRHIRef PositionMassCopyRHI;
	ZibraStructuredBufferRHIRef NodeParticlePairs0RHI;
	ZibraStructuredBufferRHIRef NodeParticlePairs1RHI;
	ZibraStructuredBufferRHIRef RadixGroupData1RHI;
	ZibraStructuredBufferRHIRef RadixGroupData2RHI;
	ZibraStructuredBufferRHIRef RadixGroupData3RHI;
	ZibraStructuredBufferRHIRef ManipulatorStatisticsRHI;

	FTextureBridge Color0Bridge;
	FTextureBridge Color1Bridge;
	FTextureBridge DepthBridge;
	FTextureBridge SDFRenderBridge;

	ZibraStructuredBufferRHIRef DynamicManipulatorDataRHI;
	ZibraStructuredBufferRHIRef ConstManipulatorDataRHI;
};