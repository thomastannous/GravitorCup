// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GravitorDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class GRAVITORCUP_API UGravitorDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GravitorDataAsset")
	float GravitorSpeed;
	
};
