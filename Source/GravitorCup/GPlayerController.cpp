// Fill out your copyright notice in the Description page of Project Settings.


#include "GPlayerController.h"
#include "GGravitor.h"
#include "GMainMenuGameMode.h"
#include "GLocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"


AGPlayerController::AGPlayerController()
{
    Team = -1;
    
    PrimaryActorTick.bTickEvenWhenPaused = true;

}

void AGPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAxis("MoveForward", this, &AGPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AGPlayerController::MoveRight);
    InputComponent->BindAxis("SpinForward", this, &AGPlayerController::SpinForward);
    InputComponent->BindAxis("SpinRight", this, &AGPlayerController::SpinRight);
    InputComponent->BindAxis("VortexLeft", this, &AGPlayerController::VortexLeft);
    InputComponent->BindAxis("VortexRight", this, &AGPlayerController::VortexRight);
    InputComponent->BindAction("ActivateBoost", IE_Pressed, this, &AGPlayerController::ActivateBoost);
    InputComponent->BindAction("ActivateBoost", IE_Released, this, &AGPlayerController::DeactivateBoost);
    InputComponent->BindAction("ActivateBoostImpulse", IE_Pressed, this, &AGPlayerController::ActivateBoostImpulse);
    InputComponent->BindAction("UseCollectedItem", IE_Pressed, this, &AGPlayerController::UseCollectedItem);

    InputComponent->BindAction("MenuUp", IE_Pressed, this, &AGPlayerController::MenuInputUp);
    InputComponent->BindAction("MenuRight", IE_Pressed, this, &AGPlayerController::MenuInputRight);
    InputComponent->BindAction("MenuLeft", IE_Pressed, this, &AGPlayerController::MenuInputLeft);
    InputComponent->BindAction("MenuDown", IE_Pressed, this, &AGPlayerController::MenuInputDown);
    InputComponent->BindAction("PlayerJoin", IE_Pressed, this, &AGPlayerController::PlayerJoin);
    InputComponent->BindAction("PlayerLeave", IE_Pressed, this, &AGPlayerController::PlayerLeave);
    InputComponent->BindAction("OpenGameMenu", IE_Pressed, this, &AGPlayerController::OpenGameMenu).bExecuteWhenPaused = true;
}

void AGPlayerController::BeginPlay()
{
    Super::BeginPlay();

    UWorld* World = GetWorld();

    if (World)
    {
        MainMenuGameMode = Cast<AGMainMenuGameMode>(World->GetAuthGameMode());

        if (MainMenuGameMode)
        {
            EnableMouse();
        }
    }
}

UGLocalPlayer* AGPlayerController::GetGPlayer()
{
    return Cast<UGLocalPlayer>(Player);
}

void AGPlayerController::MoveForward(float AxisValue)
{
    if (ControlledGravitor)
    {
        ControlledGravitor->AddMovementInput(FVector(1.0, 0.0, 0.0) * AxisValue);
    }
}

void AGPlayerController::MoveRight(float AxisValue)
{
    if (ControlledGravitor)
    {
        ControlledGravitor->AddMovementInput(FVector(0.0, 1.0, 0.0) * AxisValue);
    }
}

void AGPlayerController::SpinForward(float AxisValue)
{
    if (ControlledGravitor)
    {
        ControlledGravitor->AddSpinInput(FVector2D(1.f, 0.f) * AxisValue);
    }
}

void AGPlayerController::SpinRight(float AxisValue)
{
    if (ControlledGravitor)
    {
        ControlledGravitor->AddSpinInput(FVector2D(0.f, 1.f) * AxisValue);
    }
}

void AGPlayerController::ActivateBoost()
{
    if (ControlledGravitor)
    {
        ControlledGravitor->ActivateBoost();
    }
}

void AGPlayerController::DeactivateBoost()
{
    if (ControlledGravitor)
    {
        ControlledGravitor->DeactivateBoost();
    }
}

void AGPlayerController::ActivateBoostImpulse()
{
    if (ControlledGravitor)
    {
        ControlledGravitor->ActivateBoostImpulse();
    }
}

void AGPlayerController::VortexRight(float Value)
{
    if (ControlledGravitor)
    {
        ControlledGravitor->AddRightVortexInput(Value);
    }
}

void AGPlayerController::VortexLeft(float Value)
{
    if (ControlledGravitor)
    {
        ControlledGravitor->AddLeftVortexInput(Value);
    }
}

void AGPlayerController::UseCollectedItem()
{
    if (ControlledGravitor)
    {
        ControlledGravitor->UseNextCollectedItem();
    }
}

void AGPlayerController::EnableMouse()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

void AGPlayerController::DisableMouse()
{
    bShowMouseCursor = false;
    bEnableClickEvents = false;
    bEnableMouseOverEvents = false;
}

void AGPlayerController::MenuInputUp()
{
    if (MainMenuGameMode)
    {
        MainMenuGameMode->PlayerPressedMenuUp(this);
    }
}

void AGPlayerController::MenuInputDown()
{
    if (MainMenuGameMode)
    {
        MainMenuGameMode->PlayerPressedMenuDown(this);
    }
}

void AGPlayerController::MenuInputRight()
{
    if (MainMenuGameMode)
    {
        MainMenuGameMode->PlayerPressedMenuRight(this);
    }
}

void AGPlayerController::MenuInputLeft()
{
    if (MainMenuGameMode)
    {
        MainMenuGameMode->PlayerPressedMenuLeft(this);
    }
}

void AGPlayerController::PlayerJoin()
{
    if (MainMenuGameMode)
    {
        MainMenuGameMode->PlayerPressedJoin(this);
    }
}

void AGPlayerController::PlayerLeave()
{
    if (MainMenuGameMode)
    {
        MainMenuGameMode->PlayerPressedLeave(this);
    }
}

void AGPlayerController::OpenGameMenu()
{
    if (MainMenuGameMode)
    {
        return;
    }
    if (!GameMenuWidget)
    {
        if (GameMenuWClass)
        {
            GameMenuWidget = CreateWidget<UUserWidget>(this, GameMenuWClass);
            if (GameMenuWidget)
            {
                GameMenuWidget->AddToViewport();
                UGameplayStatics::SetGamePaused(this, true);
                EnableMouse();
                // F UI;
                // SetInputMode(UI);
            }
        }
    }
    else
    {
        GameMenuWidget->RemoveFromParent();
        GameMenuWidget = nullptr;
        FInputModeGameOnly GameInputMode;
        SetInputMode(GameInputMode);
        DisableMouse();
        UGameplayStatics::SetGamePaused(this, false);
    }
}


void AGPlayerController::HideCaptureComponents()
{
    TArray<AActor*> OutActors;

    UGameplayStatics::GetAllActorsWithTag(this, FName("GravityAffector"), OutActors);

    for (AActor* Actor : OutActors)
    {
        TArray<UActorComponent*> PrimitiveComponents = Actor->GetComponentsByTag(
            UPrimitiveComponent::StaticClass(), FName("GravityAffector"));
        for (UActorComponent* ActorComp : PrimitiveComponents)
        {
            UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(ActorComp);
            if (PrimComp)
            {
                HiddenPrimitiveComponents.Add(PrimComp);
            }
        }
    }

    TSet<FPrimitiveComponentId> OutHiddenComps;
    BuildHiddenComponentList(FVector::ZeroVector, OutHiddenComps);
}


void AGPlayerController::SetPawn(APawn* InPawn)
{
    Super::SetPawn(InPawn);

    AGGravitor* Gravitor = Cast<AGGravitor>(InPawn);
    if (Gravitor)
    {
        ControlledGravitor = Gravitor;
        if (Team != -1)
        {
            ControlledGravitor->Team = Team;
        }
    }
}
