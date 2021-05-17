// Fill out your copyright notice in the Description page of Project Settings.


#include "GGameModeBase.h"


#include "EngineUtils.h"
#include "GGameInstance.h"
#include "GPlayerController.h"
#include "GameFramework/GameSession.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "GForceFieldBorder.h"
#include "GGravitor.h"
#include "GBall.h"
#include "Kismet/GameplayStatics.h"
#include "GravitorCup.h"

void AGGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);
}

void AGGameModeBase::StartPlay()
{
    Super::StartPlay();


    UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);

    if (World)
    {
        GGameInstance = Cast<UGGameInstance>(UGameplayStatics::GetGameInstance(this));
        if (!GGameInstance)
        {
            return;
        }

        for (int i = 0; i < GGameInstance->JoinedLocalPlayers.Num(); ++i)
        {
            // UGLocalPlayer *GPlayer = GGI->JoinedLocalPlayers[i];
            //
            // GPlayer->SpawnPlayActor(FString(""), Error, World);

            FString Error;

            AGPlayerController* GPC = Cast<AGPlayerController>(GGameInstance->JoinedLocalPlayers[i]->GetPlayerController(World));
            AGGravitor* Gravitor = Cast<AGGravitor>(GPC->GetPawn());
            if (Gravitor)
            {
                Gravitor->SetToStandardHeight();
            }
            GPlayerControllers.Add(GPC);

            if (Error.Len() > 0)
            {
                UE_LOG(LogPlayerManagement, Error, TEXT("Failed to Create Player: %s"), *Error);
            }
        }


        SpawnArenaCamera(World);
    }

    TeamScores.Empty();

    for (int i = 0; i < TeamCount; ++i)
    {
        TeamScores.Add(0); // Team 0
    }

}


void AGGameModeBase::SpawnArenaCamera(UWorld* World)
{
    FActorSpawnParameters SpawnInfo;
    ArenaCam = World->SpawnActor<AGArenaCamera>(CamClass, FVector(0.f, 0.f, 5000.f), FRotator(-55.f, 0.f, 0.f),
                                                SpawnInfo);
    if (ArenaCam)
    {
        TArray<AActor*> InActors;
        for (AGPlayerController* PC : GPlayerControllers)
        {
            APawn* Pawn = PC->GetPawn();
            if (Pawn)
            {
                InActors.Add(Pawn);
            }
        }
        ArenaCam->SetTrackedObjects(InActors);
        ArenaCam->bIsEnabled = true;
        ArenaCam->AutoReceiveInput = EAutoReceiveInput::Player0;
        
        UGGameInstance *GGI = Cast<UGGameInstance>(GetGameInstance());

        if(GGI)
        {
            for (AGPlayerController* PC : GPlayerControllers)
            {
                if(GGI->JoinedLocalPlayers.Contains(PC->GetGPlayer()))
                {
                    PC->SetViewTarget(ArenaCam);
                }
            }
        }
    }
    else
    {
        UE_LOG(LogG, Error, TEXT("GGameModeBase: Failed to Spawn ArenaCam"));
    }
}

FString AGGameModeBase::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId,
                                      const FString& Options, const FString& Portal)
{
    check(NewPlayerController);

    // The player needs a PlayerState to register successfully
    if (NewPlayerController->PlayerState == nullptr)
    {
        return FString(TEXT("PlayerState is null"));
    }

    FString ErrorMessage;

    // Register the player with the session
    GameSession->RegisterPlayer(NewPlayerController, UniqueId.GetUniqueNetId(),
                                UGameplayStatics::HasOption(Options, TEXT("bIsFromInvite")));

    // Set up spectating
    bool bSpectator = FCString::Stricmp(*UGameplayStatics::ParseOption(Options, TEXT("SpectatorOnly")), TEXT("1")) == 0;
    if (bSpectator || MustSpectate(NewPlayerController))
    {
        NewPlayerController->StartSpectatingOnly();
    }

    // Init player's name
    FString InName = UGameplayStatics::ParseOption(Options, TEXT("Name")).Left(20);
    if (InName.IsEmpty())
    {
        InName = FString::Printf(TEXT("%s%i"), *DefaultPlayerName.ToString(),
                                 NewPlayerController->PlayerState->GetPlayerId());
    }

    ChangeName(NewPlayerController, InName, false);

    return ErrorMessage;
}

AActor* AGGameModeBase::FindPlayerStart_Implementation(AController* PlayerController, const FString& IncomingName)
{
    AGPlayerController* PC = Cast<AGPlayerController>(PlayerController);
    if (PC)
    {
        UE_LOG(LogTemp, Warning, TEXT("ControllerId: %d"), PC->GetLocalPlayer()->GetControllerId());

        UGGameInstance* GI = Cast<UGGameInstance>(UGameplayStatics::GetGameInstance(this));
        if (GI)
        {
            UGLocalPlayer* GPL = Cast<UGLocalPlayer>(PC->Player);

            UWorld* World = GetWorld();

            PC->Team = GPL->Team;

            // If incoming start is specified, then just use it
            for (TActorIterator<APlayerStart> It(World); It; ++It)
            {
                AGPlayerStart* Start = Cast<AGPlayerStart>(*It);
                if (Start && !Start->bIsOccupied && Start->Team == GPL->Team)
                {
                    PC->StartSpot = Start;
                    return Start;
                }
            }
        }
        else
        {
            UE_LOG(LogG, Error, TEXT("GGameModeBase: GGameInstance not valid"));
            return nullptr;
        }
    }
    else
    {
        UE_LOG(LogG, Error, TEXT("GGameModeBase: PlayerController %s not valid"), *PlayerController->GetName());
    }

    UE_LOG(LogG, Error, TEXT("GGameModeBase: Couldn't find StartingSpot for %s"), *PlayerController->GetName());
    return nullptr;
}

void AGGameModeBase::RestartPlayerAtPlayerStart(AController* NewPlayer, AActor* StartSpot)
{
    if (NewPlayer == nullptr || NewPlayer->IsPendingKillPending())
    {
        return;
    }

    if (!StartSpot)
    {
        UE_LOG(LogGameMode, Warning, TEXT("RestartPlayerAtPlayerStart: Player start not found"));
        return;
    }

    FRotator SpawnRotation = StartSpot->GetActorRotation();

    UE_LOG(LogGameMode, Verbose, TEXT("RestartPlayerAtPlayerStart %s"),
           (NewPlayer && NewPlayer->PlayerState) ? *NewPlayer->PlayerState->GetPlayerName() : TEXT("Unknown"));

    if (MustSpectate(Cast<APlayerController>(NewPlayer)))
    {
        UE_LOG(LogGameMode, Verbose, TEXT("RestartPlayerAtPlayerStart: Tried to restart a spectator-only player!"));
        return;
    }

    if (NewPlayer->GetPawn() != nullptr)
    {
        // If we have an existing pawn, just use it's rotation
        SpawnRotation = NewPlayer->GetPawn()->GetActorRotation();
    }
    else if (GetDefaultPawnClassForController(NewPlayer) != nullptr)
    {
        // Try to create a pawn to use of the default class for this player
        NewPlayer->SetPawn(SpawnDefaultPawnFor(NewPlayer, StartSpot));
    }

    if (NewPlayer->GetPawn() == nullptr)
    {
        NewPlayer->FailedToSpawnPawn();
    }
    else
    {
        // Tell the start spot it was used
        InitStartSpot(StartSpot, NewPlayer);

        FinishRestartPlayer(NewPlayer, SpawnRotation);
    }
}

void AGGameModeBase::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
}

void AGGameModeBase::InitStartSpot_Implementation(AActor* StartSpot, AController* NewPlayer)
{
    AGPlayerStart* PlayerStart = Cast<AGPlayerStart>(StartSpot);
    AGPlayerController* PC = Cast<AGPlayerController>(NewPlayer);
    if (PlayerStart)
    {
        PlayerStart->bIsOccupied = true;
    }

    if (PC)
    {
        PC->StartSpot = StartSpot;
    }
}

void AGGameModeBase::RestartPlayer(AController* NewPlayer)
{
    AGPlayerController* GPC = Cast<AGPlayerController>(NewPlayer);
    if (GPC)
    {
        UGLocalPlayer* GPL = Cast<UGLocalPlayer>(GPC->Player);
        if (GPL)
        {
            UGGameInstance* GGI = Cast<UGGameInstance>(UGameplayStatics::GetGameInstance(this));
            // Only allow to spawn PlayerController if it is joined Player form Main Menu
            if (GGI)
            {
                if (!GGI->JoinedLocalPlayers.Contains(GPL))
                {
                    return; 
                }
            }
        }
    }
    
    Super::RestartPlayer(NewPlayer);
}

void AGGameModeBase::SetGravitorsToStart()
{
    for (AGGravitor* Gravitor : Gravitors)
    {
        AGPlayerController* PC = Cast<AGPlayerController>(Gravitor->GetController());
        if (PC)
        {
            Gravitor->SetActorLocation(PC->StartSpot->GetActorLocation());
            Gravitor->ResetGravitor();
        }
    }
}

void AGGameModeBase::DisableGameInput()
{
    for (AGGravitor* Gravitor : Gravitors)
    {
        Gravitor->bGameInputDisabled = true;
    }
}

void AGGameModeBase::AllowGameInput()
{
    for (AGGravitor* Gravitor : Gravitors)
    {
        Gravitor->bGameInputDisabled = false;
    }
}

void AGGameModeBase::AllowAllBallMovement()
{
    for (AGBall* Ball : Balls)
    {
        Ball->bMovementDisabled = false;
    }
}

void AGGameModeBase::DisableAllBallMovement()
{
    for (AGBall* Ball : Balls)
    {
        Ball->StopBall();
        Ball->bMovementDisabled = true;
    }
}

void AGGameModeBase::SpawnBall()
{
    UWorld* World = GetWorld();
    if (World)
    {
        for (int i = 0; i < BallCount; ++i)
        {
            FActorSpawnParameters SpawnInfo;
            AGBall* GBall = World->SpawnActor<AGBall>(GBallClass, FVector(0.f, 0.f,PLAYER_STANDARD_HEIGHT),
                                                      FRotator::ZeroRotator, SpawnInfo);
            if (GBall)
            {
                Balls.Add(GBall);
                GBall->Setup();
                if (ArenaCam)
                {
                    ArenaCam->AddTrackedObject(GBall);
                }
                for (AGPlayerController* PC : GPlayerControllers)
                {
                    AGGravitor* Gravitor = Cast<AGGravitor>(PC->GetPawn());
                    if (Gravitor)
                    {
                        Gravitors.Add(Gravitor);
                        Gravitor->AttractableObjects.AddUnique(GBall);
                    }
                }


                TArray<AActor*> OutActors;
                OutActors.Empty();
                UGameplayStatics::GetAllActorsWithTag(this, FName("ForceFieldBorder"), OutActors);

                for (AActor* Actor : OutActors)
                {
                    AGForceFieldBorder* ForceFieldBorder = Cast<AGForceFieldBorder>(Actor);
                    if (ForceFieldBorder)
                    {
                        ForceFieldBorder->ForceObjects.Add(GBall);

                        for (AGPlayerController* PC : GPlayerControllers)
                        {
                            AGGravitor* Gravitor = Cast<AGGravitor>(PC->GetPawn());
                            if (Gravitor)
                            {
                                ForceFieldBorder->ForceObjects.Add(Gravitor);
                            }
                        }
                    }
                }
            }
        }
    }
}

void AGGameModeBase::SetBallsToStart()
{
    for (AGBall* Ball : Balls)
    {
        Ball->ResetBall();
    }
}
