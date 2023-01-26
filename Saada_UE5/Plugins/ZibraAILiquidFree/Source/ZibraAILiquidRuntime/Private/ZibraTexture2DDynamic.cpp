// Copyright Zibra AI, Inc 2022. All Rights Reserved.

/*=============================================================================
	ZibraTexture2DDynamic.cpp: Implementation of UZibraTexture2DDynamic.
=============================================================================*/
#include "ZibraTexture2DDynamic.h"
#include "Engine/Texture2DDynamic.h"
#include "UObject/Package.h"
#include "TextureResource.h"
#include "DeviceProfiles/DeviceProfile.h"
#include "DeviceProfiles/DeviceProfileManager.h"

/*-----------------------------------------------------------------------------
	FTexture2DDynamicResource
-----------------------------------------------------------------------------*/

/** Initialization constructor. */
FZibraTexture2DDynamicResource::FZibraTexture2DDynamicResource(UZibraTexture2DDynamic* InOwner)
:	Owner(InOwner)
{
}

/** Returns the width of the texture in pixels. */
uint32 FZibraTexture2DDynamicResource::GetSizeX() const
{
	return Owner->SizeX;
}

/** Returns the height of the texture in pixels. */
uint32 FZibraTexture2DDynamicResource::GetSizeY() const
{
	return Owner->SizeY;
}


/** Called when the resource is initialized. This is only called by the rendering thread. */
void FZibraTexture2DDynamicResource::InitRHI()
{
	// Create the sampler state RHI resource.
	ESamplerAddressMode SamplerAddressMode = Owner->SamplerAddressMode;
	FSamplerStateInitializerRHI SamplerStateInitializer
	(
		(ESamplerFilter)UDeviceProfileManager::Get().GetActiveProfile()->GetTextureLODSettings()->GetSamplerFilter( Owner ),
		SamplerAddressMode,
		SamplerAddressMode,
		SamplerAddressMode
	);
	SamplerStateRHI = GetOrCreateSamplerState( SamplerStateInitializer );

#if ENGINE_MAJOR_VERSION == 4
	FRHIResourceCreateInfo CreateInfo;
#elif ENGINE_MAJOR_VERSION == 5
	FRHIResourceCreateInfo CreateInfo(*Owner->Name.ToString());
#else
#error Unknown engine version
#endif
	Texture2DRHI = RHICreateTexture2D(GetSizeX(), GetSizeY(), Owner->Format, Owner->NumMips, 1, Owner->Flags, CreateInfo);
	TextureRHI = Texture2DRHI;
	TextureRHI->SetName(Owner->Name);
	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI,TextureRHI);
}

/** Called when the resource is released. This is only called by the rendering thread. */
void FZibraTexture2DDynamicResource::ReleaseRHI()
{
	RHIUpdateTextureReference(Owner->TextureReference.TextureReferenceRHI, nullptr);
	FTextureResource::ReleaseRHI();
	Texture2DRHI.SafeRelease();
}

/** Returns the Texture2DRHI, which can be used for locking/unlocking the mips. */
FTexture2DRHIRef FZibraTexture2DDynamicResource::GetTexture2DRHI()
{
	return Texture2DRHI;
}


/*-----------------------------------------------------------------------------
	UZibraTexture2DDynamic
-----------------------------------------------------------------------------*/
UZibraTexture2DDynamic::UZibraTexture2DDynamic(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NeverStream = true;
	Format = PF_B8G8R8A8;
	SamplerAddressMode = AM_Wrap;
}


void UZibraTexture2DDynamic::Init(int32 InSizeX, int32 InSizeY, EPixelFormat InFormat , ETextureCreateFlags  InFlags , const FName InName , bool InIsResolveTarget )
{
	SizeX = InSizeX;
	SizeY = InSizeY;
	Format = (EPixelFormat) InFormat;
	NumMips = 1;
	bIsResolveTarget = InIsResolveTarget;
	Flags = InFlags;
	Name = InName;

	// Initialize the resource.
	UpdateResource();
}

FTextureResource* UZibraTexture2DDynamic::CreateResource()
{
	return new FZibraTexture2DDynamicResource(this);
}

float UZibraTexture2DDynamic::GetSurfaceWidth() const
{
	return SizeX;
}

float UZibraTexture2DDynamic::GetSurfaceHeight() const
{
	return SizeY;
}


UZibraTexture2DDynamic* UZibraTexture2DDynamic::Create(int32 InSizeX, int32 InSizeY, EPixelFormat InFormat, ETextureCreateFlags  InFlags , const FName InName )
{
	FZibraTexture2DDynamicCreateInfo ZibraTexture2DDynamicCreateInfo;
	EPixelFormat DesiredFormat = InFormat;
	if (InSizeX > 0 && InSizeY > 0 )
	{
		
		auto NewTexture = NewObject<UZibraTexture2DDynamic>(GetTransientPackage(), NAME_None, RF_Transient);
		if (NewTexture != NULL)
		{
			NewTexture->Filter = TF_Nearest;
			NewTexture->SamplerAddressMode = ZibraTexture2DDynamicCreateInfo.SamplerAddressMode;
			NewTexture->SRGB = ZibraTexture2DDynamicCreateInfo.bSRGB;

			// Disable compression
			NewTexture->CompressionSettings		= TC_Default;
#if WITH_EDITORONLY_DATA
			NewTexture->CompressionNone			= true;
			NewTexture->MipGenSettings			= TMGS_NoMipmaps;
			NewTexture->CompressionNoAlpha		= true;
			NewTexture->DeferCompression		= false;
#endif // #if WITH_EDITORONLY_DATA
			if (ZibraTexture2DDynamicCreateInfo.bIsResolveTarget )
			{
				NewTexture->bNoTiling			= false;
			}
			else
			{
				// Untiled format
				NewTexture->bNoTiling			= true;
			}

			NewTexture->Init(InSizeX, InSizeY, DesiredFormat, InFlags, InName, ZibraTexture2DDynamicCreateInfo.bIsResolveTarget);
		}
		return NewTexture;
	}
	else
	{
		UE_LOG(LogTexture, Warning, TEXT("Invalid parameters specified for UZibraTexture2DDynamic::Create()"));
		return NULL;
	}
}
