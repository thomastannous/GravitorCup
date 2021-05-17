// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GRAVITORCUP_API AGPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AGPlayerController();

	virtual void SetupInputComponent() override;

	virtual void BeginPlay() override;

	virtual class UGLocalPlayer* GetGPlayer();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="GPlayerController")
	int32 Team;
	
	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void SpinForward(float AxisValue);
	void SpinRight(float AxisValue);
	void ActivateBoost();
	void DeactivateBoost();
	void ActivateBoostImpulse();
	void VortexRight(float Value);
	void VortexLeft(float Value);
	void UseCollectedItem();
	

	// Main Menu Related functions..

	void EnableMouse();
	void DisableMouse();
	
	void MenuInputUp();
	void MenuInputDown();
	void MenuInputRight();
	void MenuInputLeft();
	void PlayerJoin();
	void PlayerLeave();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GPlayerController")
	TSubclassOf<class UUserWidget> GameMenuWClass;

	UPROPERTY()
	class UUserWidget *GameMenuWidget;
	
	void OpenGameMenu();

	class AGGravitor* ControlledGravitor;

	class AGMainMenuGameMode *MainMenuGameMode;

	UFUNCTION(BlueprintCallable, Category="GPlayerController")
	void HideCaptureComponents();

	/* BEGIN A(Player)Controller Interface*/
	virtual void SetPawn(APawn *InPawn) override;
	/* End A(Player)Controller Interface*/
};