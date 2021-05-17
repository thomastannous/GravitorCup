// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "GLocalPlayer.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITORCUP_API UGLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()
	
public:
	
	UGLocalPlayer();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=PlayerData)
	int32 PlayerId;
		
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=PlayerData)
	TSubclassOf<class AGGravitor> ChosenGravitor;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=PlayerData)
	FName PlayerName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category=PlayerData)
	int32 Team;

	//virtual bool SpawnPlayActor(const FString& URL,FString& OutError, UWorld* InWorld) override;
};
