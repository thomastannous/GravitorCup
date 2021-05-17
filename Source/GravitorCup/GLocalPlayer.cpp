// Fill out your copyright notice in the Description page of Project Settings.


#include "GLocalPlayer.h"
#include "GGravitor.h"

UGLocalPlayer::UGLocalPlayer()
{
    PlayerId = -1;
    ChosenGravitor = nullptr;
    PlayerName = FName(FString::Printf(TEXT("Player%d"), PlayerId));
    Team = -1;
}
