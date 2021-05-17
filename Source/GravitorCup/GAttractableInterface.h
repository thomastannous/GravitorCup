// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAttractableInterface.generated.h"

/**
 * 
 */
UINTERFACE(BlueprintType)
class GRAVITORCUP_API UGAttractableInterface : public UInterface
{
  GENERATED_BODY()
 
};

class IGAttractableInterface
{
    GENERATED_BODY()
public:
    virtual void AddGravityForce(FVector Force, float Distance){}
};
