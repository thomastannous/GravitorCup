// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"

#define PLAYER_STANDARD_HEIGHT 0.f

DECLARE_LOG_CATEGORY_EXTERN(LogG, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGravitor, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGBall, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGCamera, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGLevel, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(LogGHUD, Log, All);


FORCEINLINE static float CalcPowerDecayByDistance(float Distance, float GravityPower, float DistanceDecay)
{
    //return FMath::Clamp(0.00001f/(-FMath::Exp(-(Distance/DistanceDecay))) + GravityPower, 0.f, GravityPower);
    //const float FinalPower = (Distance+250.f)/2000.f * GravityPower;
    //UE_LOG(LogG, VeryVerbose, TEXT("GravityPower: %f"), FinalPower);
    return GravityPower;//FinalPower;
}

static void DrawEasyLine(UWorld *World, FVector Start, FVector End, float DeltaTime, FColor Color=FColor::Red)
{
    if (World)
    {
        DrawDebugLine(World, Start, End, Color, true, DeltaTime, SDPG_Foreground, 4.f);
    }
}