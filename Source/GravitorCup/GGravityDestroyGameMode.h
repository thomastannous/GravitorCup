// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GGameModeBase.h"
#include "GGravityDestroyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITORCUP_API AGGravityDestroyGameMode : public AGGameModeBase
{
	GENERATED_BODY()
public:

	virtual void StartPlay() override;

	FTimerHandle AfterRoundFinishTimerHandle;
	FTimerHandle RestartCountdownTimerHandle;

	UFUNCTION(BlueprintCallable)
	float GetAfterDestroyRemainingTime() const;
	UFUNCTION(BlueprintCallable)
	float GetRestartCountdownRemainingTime() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GravityDestroy")
	float AfterRoundFinishTime;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GravityDestroy")
	float RestartCountdownTime;


	void OnAfterRoundFinishEnd();
	
	void OnRestartCountdownEnd();

	void RegisterDestroy(int32 Team, class AGBall *Ball);

	/* Increments the score for the given team */
	void IncrementScore(int32 Team);

	void OnDestroy(class AGBall *Ball);

	void FinishRound();

	void FinishGame();

	UFUNCTION(BlueprintNativeEvent)
	void OnFinishGame();

	void RegisterBallExpired(class AGBall *Ball);
};
