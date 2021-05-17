// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GGameModeBase.h"
#include "GGravityBallGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITORCUP_API AGGravityBallGameMode : public AGGameModeBase
{
	GENERATED_BODY()

	virtual void StartPlay() override;

public:

	FTimerHandle AfterGoalTimerHandle;
	FTimerHandle RestartCountdownTimerHandle;

	UFUNCTION(BlueprintCallable)
	float GetAfterGoalRemainingTime() const;
	UFUNCTION(BlueprintCallable)
	float GetRestartCountdownRemainingTime() const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GravityBall")
	float AfterGoalDelayTime;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GravityBall")
	float RestartCountdownTime;


	void OnAfterGoalTimeEnd();
	
	
	void OnRestartCountdownEnd();
	virtual void SpawnBall() override;

	//void CenterBall();
	
	/* Team is the team that was defending that goal */
	void RegisterGoal(int32 Team, class AGBall *Ball);

	void OnGoal(class AGBall *Ball);
};
