// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "Runtime/Launch/Resources/Version.h"

#if ENGINE_MAJOR_VERSION == 4
	#define ZibraVector3 FVector
	#define ZibraVector4 FVector4
	#define ZibraVector2 FVector2D
	#define ZibraMatrix FMatrix
#elif ENGINE_MAJOR_VERSION == 5
	#define ZibraVector3 FVector3f
	#define ZibraVector4 FVector4f
	#define ZibraVector2 FVector2f
    #define ZibraMatrix FMatrix44f
#else
#error Unknown UE version
#endif