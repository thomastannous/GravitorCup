// Fill out your copyright notice in the Description page of Project Settings.


#include "GMainMenuGameMode.h"
#include "GravitorCup.h"
#include "GLocalPlayer.h"
#include "GPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GLocalPlayer.h"
#include "Kismet/GameplayStatics.h"

void AGMainMenuGameMode::StartPlay()
{
    Super::StartPlay();

    SelectedGameMode = ESelectedGameMode::SG_None;
    
    GGameInstance = Cast<UGGameInstance>(UGameplayStatics::GetGameInstance(this));
    if(!GGameInstance)
    {
        UE_LOG(LogPlayerManagement, Error, TEXT("Failed to obtain GameInstance"));
        return; 
    }
        

    FString Error;

    AGPlayerController* PC = Cast<AGPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
    
    UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);

    if (World)
    {
        // Always spawn 4 GLocalPlayer, even if not needed, for plug and join in the main menu
        for (int i = 1; i < 4; ++i)
        {
            UGLocalPlayer* GLocalPlayer = Cast<UGLocalPlayer>(World->GetGameInstance()->CreateLocalPlayer(i, Error, true));
            if(!GLocalPlayer)
            {
                UE_LOG(LogPlayerManagement, Error, TEXT("ULocalPlayer (id %d) is not a UGLocalPlayer."), i);
                continue; 
            }

            GLocalPlayer->Team = -1;
            GLocalPlayer->PlayerId = i;
        }

        if (Error.Len() > 0)
        {
            UE_LOG(LogPlayerManagement, Error, TEXT("Failed to Create Player: %s"), *Error);
        }
    }
}

void AGMainMenuGameMode::StoreInGameInstance()
{
    UGGameInstance* GGI = Cast<UGGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (GGI)
    {
        GGI->RemoveAllUnutilizedPlayers();
        // for (int i = 0; i < JoinedPlayerControllers.Num(); ++i)
        // {
        //     UGLocalPlayer* GPlayer = Cast<UGLocalPlayer>(JoinedPlayerControllers[i]->Player);
        //     if (GPlayer)
        //     {
        //         //GGI->SetPlayerDataByControllerId(GPlayer->GetControllerId(), PlayerDataArray[i]);
        //     }
        // }
    }
    else
    {
        UE_LOG(LogPlayerManagement, Error, TEXT("StoreInGameInstace(): Couldn't retrieve GGameInstance"));
    }
}

void AGMainMenuGameMode::RestoreFromGameInstance()
{
}

bool AGMainMenuGameMode::CanStart1v1()
{
    int32 TeamACount = 0;
    int32 TeamBCount = 0;

    for (UGLocalPlayer *GLP : GGameInstance->JoinedLocalPlayers)
    {
        if (GLP->Team == -1)
        {
            continue;
        }
        GLP->Team ? ++TeamACount : ++TeamBCount;
        // TODO: Check that Gravitor is set
    }

    return TeamACount == 1 && TeamBCount == 1 && GGameInstance->JoinedLocalPlayers.Num() == 2;
}

bool AGMainMenuGameMode::CanStart2v2()
{
    int32 TeamACount = 0;
    int32 TeamBCount = 0;
    
    for (UGLocalPlayer *GLP : GGameInstance->JoinedLocalPlayers)
    {
        if (GLP->Team == -1)
        {
            continue;
        }
        GLP->Team ? ++TeamACount : ++TeamBCount;
        // TODO: Check that Gravitor is set
    }

    return TeamACount == 2 && TeamBCount == 2;
}

void AGMainMenuGameMode::SetSelectedGameMode(ESelectedGameMode NewSelectedGameMode)
{
    SelectedGameMode = NewSelectedGameMode;
}

void AGMainMenuGameMode::PlayerSelectTeam(UGLocalPlayer* Player, int32 NewTeam)
{
    int32 Id = Player->GetControllerId();

    if (Id == -1)
    {
        return;
    }

    NewTeam = FMath::Clamp(NewTeam, -1, 2);

    Player->Team = NewTeam;
}

void AGMainMenuGameMode::PlayerInputRightInTeamSelection_Implementation()
{
}

void AGMainMenuGameMode::PlayerInputLeftInTeamSelection_Implementation()
{
}

void AGMainMenuGameMode::PlayerPressedMenuUp(AGPlayerController* PlayerController)
{
}

void AGMainMenuGameMode::PlayerPressedMenuDown(AGPlayerController* PlayerController)
{
}

void AGMainMenuGameMode::PlayerPressedMenuRight(AGPlayerController* PlayerController)
{
    if (bIsTeamAssignmentActive)
    {
        UGLocalPlayer* GPL = Cast<UGLocalPlayer>(PlayerController->Player);
        if (GPL)
        {
            int32 PTeam = GPL->Team;
            // If left team -> no team, else (no team or right team) -> right team
            PTeam = FMath::Min(1,++PTeam);
            ChangeTeamForPlayer(GPL, PTeam);
            UpdateTeamSelectionGUI();
        }
    }
}

void AGMainMenuGameMode::PlayerPressedMenuLeft(AGPlayerController* PlayerController)
{
    if (bIsTeamAssignmentActive)
    {
        UGLocalPlayer* GPL = Cast<UGLocalPlayer>(PlayerController->Player);
        if (GPL)
        {
            int32 PTeam = GPL->Team;
            // If right team -> no team, else (no team or left team) -> left team
            PTeam = FMath::Max(0,--PTeam);
            ChangeTeamForPlayer(GPL, PTeam);
            UpdateTeamSelectionGUI();
        }
    }
}

void AGMainMenuGameMode::PlayerPressedJoin(AGPlayerController* PlayerController)
{
    UGLocalPlayer *GPL = Cast<UGLocalPlayer>(PlayerController->Player);
    if(!GPL || GGameInstance->JoinedLocalPlayers.Contains(GPL))
    {
        UE_LOG(LogPlayerManagement, Warning, TEXT("GLocalPlayer %s can't join, because is has already joined."), *GetNameSafe(GPL));
        return;
    }

    if (SelectedGameMode == ESelectedGameMode::SG_1v1)
    {
        if (GGameInstance->JoinedLocalPlayers.Num() >= 2)
        {
            WarnLobbyIsFull();
        }
        else
        {
            GPL->Team = GGameInstance->JoinedLocalPlayers.Num(); //
            GPL->PlayerId = GPL->Team; // Not really necessary for 1v1
            GPL->PlayerName = FName(PlayerController->GetName());
            
            GGameInstance->JoinedLocalPlayers.AddUnique(GPL);
            OnPlayerJoin(PlayerController);
        }
    }
    else if (SelectedGameMode == ESelectedGameMode::SG_2v2)
    {
        if(GGameInstance->JoinedLocalPlayers.Num() >= 4)
        {
            WarnLobbyIsFull();
        }
        else
        {
            GPL->Team = 0; //
            GPL->PlayerId = GPL->Team; // Not really necessary for 1v1(ArenaCam);
            GPL->PlayerName = FName(PlayerController->GetName());
            
            GGameInstance->JoinedLocalPlayers.AddUnique(GPL);
            OnPlayerJoin(PlayerController);
        }
    }
}

void AGMainMenuGameMode::PlayerPressedLeave(AGPlayerController* PlayerController)
{
    UGLocalPlayer *GPL = Cast<UGLocalPlayer>(PlayerController->Player);
    if(GPL)
    {
        if(GGameInstance)
        {
            GGameInstance->JoinedLocalPlayers.Remove(GPL);
            OnPlayerLeave(PlayerController);
        }
    }
}

void AGMainMenuGameMode::ClearJoinedPlayers()
{
    if(GGameInstance)
    {
        GGameInstance->JoinedLocalPlayers.Empty();
    }
}

void AGMainMenuGameMode::ChangeTeamForPlayer(UGLocalPlayer* GPL, int32 NewTeam)
{
    if(GPL)
    {
        GPL->Team = NewTeam;
    }
}
