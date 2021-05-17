// Fill out your copyright notice in the Description page of Project Settings.


#include "GGameInstance.h"

#include "GravitorCup.h"

void UGGameInstance::ResetPlaySettings()
{
    ScoreLimit = 5;
}

void UGGameInstance::Init()
{
    Super::Init();

    ResetPlaySettings();
}

void UGGameInstance::RemoveAllUnutilizedPlayers()
{
    TArray<ULocalPlayer*> PlayersToDelete;
    for(ULocalPlayer *LP : LocalPlayers)
    {
        if(!JoinedLocalPlayers.Contains(LP))
        {
            PlayersToDelete.Add(LP);
        }
    }

    for(ULocalPlayer *LP : PlayersToDelete)
    {
        RemoveLocalPlayer(LP);
    }
}

