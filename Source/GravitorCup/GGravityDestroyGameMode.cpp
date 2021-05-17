// Fill out your copyright notice in the Description page of Project Settings.


#include "GGravityDestroyGameMode.h"
#include "GGameInstance.h"
#include "GBall.h"


void AGGravityDestroyGameMode::StartPlay()
{
    Super::StartPlay();
    
    UWorld *World = GetWorld();
    if(World)
    {
        SpawnBall();
    }

    if(GGameInstance)
    {
        ScoreLimit = GGameInstance->ScoreLimit;
    }
    
    if (bWithGrass)
    {
        SpawnGrassAffectorEmitters();
    }
    
    
}

float AGGravityDestroyGameMode::GetAfterDestroyRemainingTime() const
{
    return GetWorldTimerManager().GetTimerRemaining(AfterRoundFinishTimerHandle);
}

float AGGravityDestroyGameMode::GetRestartCountdownRemainingTime() const
{
    return GetWorldTimerManager().GetTimerRemaining(RestartCountdownTimerHandle);
}

void AGGravityDestroyGameMode::OnAfterRoundFinishEnd()
{
    GetWorldTimerManager().SetTimer(RestartCountdownTimerHandle, this, &AGGravityDestroyGameMode::OnRestartCountdownEnd,
                                    RestartCountdownTime, false);


    K2_OnRoundReset();

    // Balls
    SetBallsToStart();
    DisableAllBallMovement();

    // Gravitors
    DisableGameInput();
    SetGravitorsToStart();
}

void AGGravityDestroyGameMode::OnRestartCountdownEnd()
{
    AllowAllBallMovement();
    AllowGameInput();
    K2_OnRoundStart();
}

void AGGravityDestroyGameMode::RegisterDestroy(int32 Team, AGBall* Ball)
{
    IncrementScore(Team);
    OnDestroy(Ball);
    FinishRound();
}

void AGGravityDestroyGameMode::IncrementScore(int32 Team)
{
    if(++TeamScores[Team] >= ScoreLimit)
    {
        // Game over return to main menu
        FinishGame();
    }
}

void AGGravityDestroyGameMode::OnDestroy(AGBall* Ball)
{
    
}

void AGGravityDestroyGameMode::FinishRound()
{
    GetWorldTimerManager().SetTimer(AfterRoundFinishTimerHandle, this, &AGGravityDestroyGameMode::OnAfterRoundFinishEnd,
                                    AfterRoundFinishTime, false);
    DisableAllBallMovement();
}

void AGGravityDestroyGameMode::FinishGame()
{
    OnFinishGame();
}

void AGGravityDestroyGameMode::OnFinishGame_Implementation()
{
     
}

void AGGravityDestroyGameMode::RegisterBallExpired(AGBall* Ball)
{
    IncrementScore(Ball->PossessingTeam);
    FinishRound();
}
