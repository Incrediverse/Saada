// Copyright Zibra AI, Inc 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#ifndef WITH_ZIBRA_AI
#define WITH_ZIBRA_AI ((PLATFORM_WINDOWS && ZIBRA_AI_NATIVE_SUPPORT))
#endif

#ifndef TEST_PAID_VERSION
#define TEST_PAID_VERSION 1
#endif

#ifndef ZIBRA_LIQUID_PAID_VERSION
#define ZIBRA_LIQUID_PAID_VERSION TEST_PAID_VERSION
#endif 
