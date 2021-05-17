// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "GPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITORCUP_API AGPlayerStart : public APlayerStart
{
	GENERATED_BODY()

public:
	
	AGPlayerStart(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=GPlayerStart)
	int32 Team;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=GPlayerStart)
	bool bIsOccupied;
	
};
