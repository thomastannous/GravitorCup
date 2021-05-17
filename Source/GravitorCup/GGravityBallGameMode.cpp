// Fill out your copyright notice in the Description page of Project Settings.


#include "GGravityBallGameMode.h"
#include "GBall.h"
#include "GBlackholeGoal.h"
#include "Kismet/GameplayStatics.h"

void AGGravityBallGameMode::StartPlay()
{
    Super::StartPlay();

    SpawnBall();


    AfterGoalDelayTime = 2.f;
    RestartCountdownTime = 3.f;

}


float AGGravityBallGameMode::GetAfterGoalRemainingTime() const
{
    return GetWorldTimerManager().GetTimerRemaining(AfterGoalTimerHandle);
}

float AGGravityBallGameMode::GetRestartCountdownRemainingTime() const
{
    return GetWorldTimerManager().GetTimerRemaining(RestartCountdownTimerHandle);
}


void AGGravityBallGameMode::OnAfterGoalTimeEnd()
{
    GetWorldTimerManager().SetTimer(RestartCountdownTimerHandle, this, &AGGravityBallGameMode::OnRestartCountdownEnd,
                                    RestartCountdownTime, false);


    K2_OnRoundReset();

    // Balls
    SetBallsToStart();
    DisableAllBallMovement();

    // Gravitors
    DisableGameInput();
    SetGravitorsToStart();
}

void AGGravityBallGameMode::OnRestartCountdownEnd()
{
    AllowAllBallMovement();
    AllowGameInput();
    K2_OnRoundStart();
}

void AGGravityBallGameMode::SpawnBall()
{
    Super::SpawnBall();
    UWorld* World = GetWorld();
    if (World)
    {
        for (AGBall *Ball : Balls)
        {
            if (Ball)
            {
                
                TArray<AActor*> OutActors;
                UGameplayStatics::GetAllActorsWithTag(this, FName("Blackhole"), OutActors);

                for (AActor* Actor : OutActors)
                {
                    AGBlackholeGoal* BlackholeGoal = Cast<AGBlackholeGoal>(Actor);
                    if (BlackholeGoal)
                    {
                        BlackholeGoal->AttractableObjects.Add(Ball);
                    }
                }
            }
        }
    }
}

void AGGravityBallGameMode::RegisterGoal(int32 Team, AGBall* Ball)
{
    TeamScores[Team] += 1;
    OnGoal(Ball);
}

void AGGravityBallGameMode::OnGoal(AGBall* Ball)
{
    GetWorldTimerManager().SetTimer(AfterGoalTimerHandle, this, &AGGravityBallGameMode::OnAfterGoalTimeEnd,
                                    AfterGoalDelayTime, false);
    DisableAllBallMovement();
}
