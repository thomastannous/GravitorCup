// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GGravitor.h"
#include "GLocalPlayer.h"

#include "GGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITORCUP_API UGGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GGameInstance")
	TArray<UGLocalPlayer*> JoinedLocalPlayers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GGameInstance")
	int32 ScoreLimit;
	
	UFUNCTION(BlueprintCallable,Category="GGameInstance")
	void ResetPlaySettings();

	virtual void Init() override;
	
	void RemoveAllUnutilizedPlayers();

	//FPlayerData GetPlayerDataByControllerId(int32 ControllerId);
	//void SetPlayerDataByControllerId(int32 ControllerId, FPlayerData NewPlayerData);
	 
};
