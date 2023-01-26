// Copyright Zibra AI, Inc 2022. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Engine/Texture.h"
#include "PixelFormat.h"
#include "ZibraTexture2DDynamic.generated.h"

class FTextureResource;

/** A dynamic 2D texture resource. */
class FZibraTexture2DDynamicResource : public FTextureResource
{
public:
	/** Initialization constructor. */
	ZIBRAAILIQUIDRUNTIME_API FZibraTexture2DDynamicResource(class UZibraTexture2DDynamic* InOwner);

	/** Returns the width of the texture in pixels. */
	virtual uint32 GetSizeX() const override;

	/** Returns the height of the texture in pixels. */
	virtual uint32 GetSizeY() const override;

	/** Called when the resource is initialized. This is only called by the rendering thread. */
	ZIBRAAILIQUIDRUNTIME_API virtual void InitRHI() override;

	/** Called when the resource is released. This is only called by the rendering thread. */
	ZIBRAAILIQUIDRUNTIME_API virtual void ReleaseRHI() override;

	/** Returns the Texture2DRHI, which can be used for locking/unlocking the mips. */
	ZIBRAAILIQUIDRUNTIME_API FTexture2DRHIRef GetTexture2DRHI();

private:
	/** The owner of this resource. */
	class UZibraTexture2DDynamic* Owner;
	/** Texture2D reference, used for locking/unlocking the mips. */
	FTexture2DRHIRef Texture2DRHI;
};

// Helper to set properties on the UTexture2DDynamic so it doesn't need to be reinitialized.
struct FZibraTexture2DDynamicCreateInfo
{
	FZibraTexture2DDynamicCreateInfo(EPixelFormat InFormat = PF_B8G8R8A8, bool InIsResolveTarget = false, bool InSRGB = true, TextureFilter InFilter = TF_Default, ESamplerAddressMode InSamplerAddressMode = AM_Wrap)
	:	Format(InFormat)
	,	bIsResolveTarget(InIsResolveTarget)
	,	bSRGB(InSRGB)
	,	Filter(InFilter)
	,	SamplerAddressMode(InSamplerAddressMode)
	{}

	EPixelFormat Format;
	bool bIsResolveTarget;
	bool bSRGB;
	TextureFilter Filter;
	ESamplerAddressMode SamplerAddressMode;
};

UCLASS()
class ZIBRAAILIQUIDRUNTIME_API UZibraTexture2DDynamic : public UTexture
{
	GENERATED_UCLASS_BODY()

	/** The width of the texture. */
	int32 SizeX;

	/** The height of the texture. */
	int32 SizeY;

	/** The format of the texture. */
	UPROPERTY(transient)
	TEnumAsByte<enum EPixelFormat> Format;

	/** Whether the texture can be used as a resolve target. */
	uint8 bIsResolveTarget : 1;

	/** The number of mip-maps in the texture. */
	int32 NumMips;

	/** The sampler default address mode for this texture. */
	ESamplerAddressMode SamplerAddressMode;

	ETextureCreateFlags  Flags;

	FName Name;
	
public:
	//~ Begin UTexture Interface.
	virtual FTextureResource* CreateResource() override;
	virtual EMaterialValueType GetMaterialType() const override { return MCT_Texture2D; }
	virtual float GetSurfaceWidth() const override;
	virtual float GetSurfaceHeight() const override;
	//~ End UTexture Interface.
	/**
	 * Initializes the texture with 1 mip-level and creates the render resource.
	 *
	 * @param InSizeX			- Width of the texture, in texels
	 * @param InSizeY			- Height of the texture, in texels
	 * @param InFormat			- Format of the texture, defaults to PF_B8G8R8A8
	 * @param InIsResolveTarget	- Whether the texture can be used as a resolve target
	 */
	void Init(int32 InSizeX, int32 InSizeY, EPixelFormat InFormat = PF_B8G8R8A8, ETextureCreateFlags  InFlags = TexCreate_None, const FName InName = NAME_None, bool InIsResolveTarget = false);

	static UZibraTexture2DDynamic* Create(int32 InSizeX, int32 InSizeY, EPixelFormat InFormat, ETextureCreateFlags  InFlags = TexCreate_None, const FName InName = NAME_None);
};
