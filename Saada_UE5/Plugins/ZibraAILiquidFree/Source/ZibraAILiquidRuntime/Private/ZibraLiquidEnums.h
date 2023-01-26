// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

typedef enum GfxRenderer
{
	kGfxRendererD3D11 = 2,        // Direct3D 11
	kGfxRendererGCM = 3,          // PlayStation 3
	kGfxRendererNull = 4,         // "null" device (used in batch mode)
	kGfxRendererOpenGLES20 = 8,   // OpenGL ES 2.0
	kGfxRendererOpenGLES30 = 11,  // OpenGL ES 3.0
	kGfxRendererGXM = 12,         // PlayStation Vita
	kGfxRendererPS4 = 13,         // PlayStation 4
	kGfxRendererXboxOne = 14,     // Xbox One
	kGfxRendererMetal = 16,       // iOS Metal
	kGfxRendererOpenGLCore = 17,  // OpenGL core
	kGfxRendererD3D12 = 18,       // Direct3D 12
	kGfxRendererVulkan = 21,      // Vulkan
	kGfxRendererNvn = 22,         // Nintendo Switch NVN API
	kGfxRendererXboxOneD3D12 = 23 // MS XboxOne Direct3D 12
} GfxRenderer;