// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GArenaCamera.h"
#include "GPlayerController.h"
#include "GPlayerStart.h"
#include "GameFramework/GameModeBase.h"
#include "GGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITORCUP_API AGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void StartPlay() override;
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;
	
	virtual AActor* FindPlayerStart_Implementation(AController* PlayerController, const FString& IncomingName) override;
	virtual void RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer) override;
	virtual void RestartPlayer(AController* NewPlayer) override;

public:

	UPROPERTY()
	class UGGameInstance *GGameInstance;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GravitorCup")
	int32 DesiredPlayerNum;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GravitorCup")
	bool bWithGrass;

	/* Describes how many teams should exist */	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Stats")
	int32 TeamCount;

	/* If one team reaches the limit, the game is over */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Stats")
	int32 ScoreLimit; 

	/* Team number as array index to retrieve the score for the respective team. */	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Stats")
	TArray<int32> TeamScores; 
	
	
	TArray<class AGGravitor*> Gravitors;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GravitorCup")
	TSubclassOf<class AGBall> GBallClass;

	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GravitorCup")
	int32 BallCount;
	
	UPROPERTY(BlueprintReadOnly, Category="GravitorCup")
	TArray<class AGBall*> Balls;


	UFUNCTION(BlueprintImplementableEvent, Category="GravitorCup")
	void SpawnGrassAffectorEmitters();
	
	UPROPERTY()	
	TArray<AGPlayerController*> GPlayerControllers;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GravitorCup")
	TSubclassOf<AGArenaCamera> CamClass;

	UPROPERTY(BlueprintReadWrite, Category="GravitorCup")
	AGArenaCamera *ArenaCam;
	
	virtual void SetBallsToStart();
	virtual void SetGravitorsToStart();
	virtual void DisableGameInput();
	virtual void AllowGameInput();
	virtual void AllowAllBallMovement();
	virtual void DisableAllBallMovement();
	virtual void SpawnBall();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnRoundReset")
	void K2_OnRoundReset();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnRoundStart")
	void K2_OnRoundStart();

	void SpawnArenaCamera(UWorld *World);
};


